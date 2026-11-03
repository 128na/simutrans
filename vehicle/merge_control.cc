/*
 * This file is part of the Simutrans project under the Artistic License.
 * (see LICENSE.txt)
 */

#include <chrono>
#include <string.h>

#include "merge_control.h"
#include "../utils/for.h"


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
