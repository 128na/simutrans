/*
 * This file is part of the Simutrans project under the Artistic License.
 * (see LICENSE.txt)
 */

#include <chrono>
#include <string.h>

#include "merge_control.h"
#include "simvehicle.h"
#include "simroadtraffic.h"
#include "../simconvoi.h"
#include "../simworld.h"
#include "../boden/grund.h"
#include "../boden/wege/strasse.h"
#include "../dataobj/environment.h"
#include "../dataobj/route.h"
#include "../obj/roadsign.h"
#include "../utils/for.h"


static karte_ptr_t welt;

uint8 merge_control_t::yield_lookahead = 1;

// longest lookahead accepted by set_yield_lookahead()
#define MAX_YIELD_LOOKAHEAD 3


void merge_control_t::set_yield_lookahead(uint8 tiles)
{
	if(  env_t::networkmode  ) {
		return;
	}
	yield_lookahead = tiles < 1 ? 1 : (tiles > MAX_YIELD_LOOKAHEAD ? MAX_YIELD_LOOKAHEAD : tiles);
}


static bool is_junction(const grund_t *gr)
{
	const strasse_t *str = gr ? (const strasse_t *)gr->get_weg(road_wt) : NULL;
	if(  !str  ) {
		return false;
	}
	const ribi_t::ribi r = str->get_ribi_unmasked();
	return r == ribi_t::all  ||  ribi_t::is_threeway(r);
}


const roadsign_t *merge_control_t::get_yield_sign(const grund_t *from, const grund_t *junction)
{
	const weg_t *w = from ? from->get_weg(road_wt) : NULL;
	if(  !w  ||  !w->has_sign()  ) {
		return NULL;
	}
	const roadsign_t *rs = from->find<roadsign_t>();
	if(  !rs  ||  !rs->is_merge_yield()  ||  rs->get_governed_waytype() != road_wt  ||  !rs->get_desc()->is_single_way()  ) {
		return NULL;
	}
	if(  koord_distance(from->get_pos(), junction->get_pos()) != 1  ||  !is_junction(junction)  ) {
		return NULL;
	}
	return rs;
}


const grund_t *merge_control_t::get_junction_ahead(const roadsign_t *rs)
{
	if(  !rs  ||  rs->get_governed_waytype() != road_wt  ||  !rs->get_desc()->is_single_way()  ) {
		return NULL;
	}
	const grund_t *gr = welt->lookup(rs->get_pos());
	const weg_t *w = gr ? gr->get_weg(road_wt) : NULL;
	if(  !w  ) {
		return NULL;
	}
	// the direction of a one-way sign is the one it closes
	const ribi_t::ribi open = w->get_ribi_unmasked() & ~rs->get_dir();
	for(  uint8 r = 0;  r < 4;  r++  ) {
		grund_t *to;
		if(  (open & ribi_t::nesw[r])  &&  gr->get_neighbour(to, road_wt, ribi_t::nesw[r])  &&  is_junction(to)  ) {
			return to;
		}
	}
	return NULL;
}


/**
 * Where does vehicle @p v go after @p junction?
 * @returns the direction, or ribi_t::none if the vehicle does not know it (yet)
 */
static ribi_t::ribi exit_direction(vehicle_base_t *v, koord3d junction)
{
	if(  road_vehicle_t *rv = obj_cast<road_vehicle_t>(v)  ) {
		const convoi_t *cnv = rv->get_convoi();
		const route_t *r = cnv ? cnv->get_route() : NULL;
		if(  !r  ||  r->empty()  ) {
			return ribi_t::none;
		}
		// the junction lies shortly ahead of the front of the convoy, or below it for a trailing car.
		// Search from the far end back, so a route looping a roundabout gives the coming visit and
		// not the one of the previous lap.
		const uint32 front = cnv->front()->get_route_index();
		const uint32 first = front > 16 ? front - 16 : 0;
		if(  r->get_count() < 2  ) {
			return ribi_t::none;
		}
		uint32 end = front + MAX_YIELD_LOOKAHEAD + 2;
		if(  end > r->get_count() - 1u  ) {
			end = r->get_count() - 1u;
		}
		for(  uint32 i = end;  i-- > first;  ) {
			if(  r->at((uint16)i) == junction  ) {
				return ribi_type(junction, r->at((uint16)(i + 1)));
			}
		}
		return ribi_t::none;
	}
	if(  private_car_t *car = obj_cast<private_car_t>(v)  ) {
		const koord3d next = car->get_route_after(junction);
		return next != koord3d::invalid ? ribi_type(junction, next) : (ribi_t::ribi)ribi_t::none;
	}
	return ribi_t::none;
}


/**
 * Is vehicle @p v, which is heading to @p target on its way to the junction, a conflict for us?
 * @param d_out our direction after the junction (none = unknown)
 * @param lane our lane on the junction
 * @param sweeps_both_lanes we cross the other lane on the junction to get to ours
 */
static bool is_conflicting(vehicle_base_t *v, const vehicle_base_t *self, koord3d target, koord3d junction, ribi_t::ribi d_out, bool lane, bool sweeps_both_lanes)
{
	bool passing;
	bool changing_lane;
	if(  road_vehicle_t *rv = obj_cast<road_vehicle_t>(v)  ) {
		convoi_t *cnv = rv->get_convoi();
		const road_vehicle_t *self_rv = obj_cast<road_vehicle_t>(self);
		if(  !cnv  ||  (self_rv  &&  self_rv->get_convoi() == cnv)  ) {
			return false;
		}
		if(  cnv->get_akt_speed() <= kmh_to_speed(1)  ) {
			// a stopped vehicle may well be waiting for us, so yielding to it could lock both
			return false;
		}
		passing = cnv->is_overtaking();
		changing_lane = cnv->get_next_cross_lane();
	}
	else if(  private_car_t *car = obj_cast<private_car_t>(v)  ) {
		if(  car == self  ) {
			return false;
		}
		if(  car->get_current_speed() <= kmh_to_speed(1)  ) {
			return false;
		}
		passing = car->is_overtaking();
		changing_lane = car->get_next_cross_lane();
	}
	else {
		return false;
	}
	if(  v->get_pos_next() != target  ) {
		// not on its way to the junction (e.g. leaving the road just before it)
		return false;
	}
	const ribi_t::ribi o_out = exit_direction(v, junction);
	if(  d_out == ribi_t::none  ||  o_out != d_out  ) {
		// different or unknown ways out of the junction: assume our paths cross
		return true;
	}
	// both join the same road: only lanes matter
	return changing_lane  ||  sweeps_both_lanes  ||  passing == lane;
}


bool merge_control_t::must_yield(const vehicle_base_t *self, const grund_t *from, const grund_t *junction, koord3d to, bool passing_lane)
{
	merge_stats_t::add(merge_stats_t::YIELD_CHECKS);
	merge_timer_t timer(merge_stats_t::YIELD_TIME_US);

	const koord3d j = junction->get_pos();
	const ribi_t::ribi d_in = ribi_type(from->get_pos(), j);
	const ribi_t::ribi d_out = to != koord3d::invalid ? ribi_type(j, to) : (ribi_t::ribi)ribi_t::none;

	// The side we come from decides which lane of the joined road we reach first: turning right
	// (when driving on the right) we come in on the traffic lane, turning left on the passing lane.
	// Going straight or not knowing where to go, we assume to cross both lanes.
	bool sweeps_both_lanes = true;
	if(  d_out != ribi_t::none  &&  d_out != d_in  &&  ribi_t::is_single(d_out)  &&  ribi_t::is_single(d_in)  ) {
		const bool right_turn = d_out == ribi_t::rotate90(d_in);
		const bool first_lane_is_passing = right_turn == welt->get_settings().is_drive_left();
		sweeps_both_lanes = first_lane_is_passing != passing_lane;
	}

	// tiles from which vehicles head to the junction, with the tile they head to
	struct feeder_t { const grund_t *gr; koord3d target; };
	feeder_t feeders[4 * MAX_YIELD_LOOKAHEAD * 3];
	uint8 count = 0;
	uint8 level_start = 0;
	{
		// the first level: the other roads joining the junction
		const strasse_t *str = (const strasse_t *)junction->get_weg(road_wt);
		const ribi_t::ribi others = str ? str->get_ribi_unmasked() & ~ribi_t::backward(d_in) : (ribi_t::ribi)ribi_t::none;
		for(  uint8 i = 0;  i < 4;  i++  ) {
			const ribi_t::ribi d = ribi_t::nesw[i];
			grund_t *n;
			if(  (others & d)  &&  junction->get_neighbour(n, road_wt, d)  ) {
				const weg_t *w = n->get_weg(road_wt);
				if(  w  &&  (w->get_ribi() & ribi_t::backward(d))  ) {
					feeders[count].gr = n;
					feeders[count].target = j;
					count++;
				}
			}
		}
	}
	for(  uint8 level = 1;  level < yield_lookahead;  level++  ) {
		const uint8 level_end = count;
		for(  uint8 k = level_start;  k < level_end;  k++  ) {
			const grund_t *gr = feeders[k].gr;
			const weg_t *w = gr->get_weg(road_wt);
			const ribi_t::ribi back = w ? w->get_ribi_unmasked() & ~ribi_type(gr->get_pos(), feeders[k].target) : (ribi_t::ribi)ribi_t::none;
			for(  uint8 i = 0;  i < 4  &&  count < lengthof(feeders);  i++  ) {
				const ribi_t::ribi d = ribi_t::nesw[i];
				grund_t *n;
				if(  (back & d)  &&  gr->get_neighbour(n, road_wt, d)  &&  n != from  ) {
					const weg_t *wn = n->get_weg(road_wt);
					if(  wn  &&  (wn->get_ribi() & ribi_t::backward(d))  ) {
						feeders[count].gr = n;
						feeders[count].target = gr->get_pos();
						count++;
					}
				}
			}
		}
		level_start = level_end;
	}

	for(  uint8 k = 0;  k < count;  k++  ) {
		const grund_t *gr = feeders[k].gr;
		const uint8 top = gr->get_top();
		for(  uint8 pos = 1;  pos < top;  pos++  ) {
			if(  vehicle_base_t *v = obj_cast<vehicle_base_t>(gr->obj_bei(pos))  ) {
				if(  is_conflicting(v, self, feeders[k].target, j, d_out, passing_lane, sweeps_both_lanes)  ) {
					return true;
				}
			}
		}
	}
	return false;
}


bool merge_stats_t::enabled = false;
sint64 merge_stats_t::counters[merge_stats_t::MAX_COUNTER];
vector_tpl<merge_stats_t::watched_tile_t> merge_stats_t::watched_tiles;


static const char *const counter_names[merge_stats_t::MAX_COUNTER] = {
	"enter_checks",
	"enter_time_us",
	"citycar_checks",
	"citycar_time_us",
	"yield_checks",
	"yield_waits",
	"yield_time_us",
	"exit_checks",
	"exit_waits",
	"exit_time_us"
};


void merge_stats_t::reset()
{
	for(  int i = 0;  i < MAX_COUNTER;  i++  ) {
		counters[i] = 0;
	}
	FOR(vector_tpl<watched_tile_t>, &t, watched_tiles) {
		t.passes[0] = t.passes[1] = 0;
	}
}


sint64 merge_stats_t::get(const char *name)
{
	for(  int i = 0;  i < MAX_COUNTER;  i++  ) {
		if(  name  &&  strcmp(name, counter_names[i]) == 0  ) {
			return counters[i];
		}
	}
	return -1;
}


void merge_stats_t::watch_tile(koord3d pos)
{
	watched_tiles.append_unique(watched_tile_t(pos));
}


void merge_stats_t::book_lane_pass(koord3d pos, bool passing_lane)
{
	if(  !enabled  ) {
		return;
	}
	FOR(vector_tpl<watched_tile_t>, &t, watched_tiles) {
		if(  t.pos == pos  ) {
			t.passes[passing_lane ? 1 : 0]++;
			return;
		}
	}
}


uint32 merge_stats_t::get_lane_passes(koord3d pos, bool passing_lane)
{
	FOR(vector_tpl<watched_tile_t>, const &t, watched_tiles) {
		if(  t.pos == pos  ) {
			return t.passes[passing_lane ? 1 : 0];
		}
	}
	return 0;
}


sint64 merge_stats_t::now_us()
{
	return (sint64)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}
