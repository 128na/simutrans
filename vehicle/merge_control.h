/*
 * This file is part of the Simutrans project under the Artistic License.
 * (see LICENSE.txt)
 */

#ifndef VEHICLE_MERGE_CONTROL_H
#define VEHICLE_MERGE_CONTROL_H


#include "../simtypes.h"
#include "../dataobj/koord3d.h"
#include "../tpl/vector_tpl.h"

class grund_t;
class roadsign_t;
class road_vehicle_t;
class route_t;
class vehicle_base_t;


/**
 * Merge control for road junctions, e.g. the entrances of a roundabout.
 *
 * The rules are set on a one-way sign on the tile right in front of the junction,
 * so they apply to the vehicles entering the junction from that tile only:
 * - yield: wait while a vehicle heading to the junction on another road would have to
 *   share a lane with us or cross our path there.
 * - exit clear: a convoy enters only when there is room for all of it behind the junction,
 *   so that it does not stop with its tail on the junction.
 *
 * All checks are free of side effects, so they can be used in the recursive
 * "can the convoy ahead move" checks as well.
 */
class merge_control_t
{
	static uint8 yield_lookahead;

public:
	/// @returns the sign with merge rules for vehicles entering @p junction from @p from, or NULL
	static const roadsign_t *get_merge_sign(const grund_t *from, const grund_t *junction);

	/**
	 * Decides whether @p self, standing on @p from, has to wait before entering @p junction.
	 * @param to tile @p self heads for after the junction, koord3d::invalid if not known yet
	 * @param passing_lane the lane @p self is going to use on the junction
	 */
	static bool must_yield(const vehicle_base_t *self, const grund_t *from, const grund_t *junction, koord3d to, bool passing_lane);

	/**
	 * Decides whether convoy @p self has to wait before entering the junction at route index
	 * @p junction_index, because a stopped vehicle behind the junction would make it stop
	 * before all of it has left the junction(s).
	 * @param passing_lane our lane on the junction: entering on the traffic lane, we pass the
	 *        vehicles stopped on the passing lane of a one-way road
	 */
	static bool exit_blocked(const road_vehicle_t *self, const route_t *route, uint32 junction_index, bool passing_lane);

	/// how many tiles of the other roads are looked at for approaching vehicles (1 = the tiles next to the junction)
	static uint8 get_yield_lookahead() { return yield_lookahead; }
	/// for benchmarks only; ignored in network games, where it is not synchronised
	static void set_yield_lookahead(uint8 tiles);
};


/**
 * Runtime-only measurement for road merges (e.g. roundabout entrances).
 *
 * The counters are meant for automated benchmarks driven by scripts. They are
 * never saved and never read by the simulation itself, so collecting them cannot
 * change the course of a (network) game. Nothing is recorded unless enabled.
 */
class merge_stats_t
{
public:
	enum counter_t {
		ENTER_CHECKS = 0,  ///< top-level can_enter_tile() calls of road convoys
		ENTER_TIME_US,     ///< time spent in them, including the recursive checks of convoys ahead
		CITYCAR_CHECKS,    ///< ist_weg_frei() calls of city cars
		CITYCAR_TIME_US,   ///< time spent in them
		YIELD_CHECKS,      ///< evaluations of a yield sign at a merge
		YIELD_WAITS,       ///< of those, the ones that made the vehicle wait
		YIELD_TIME_US,     ///< time spent in the yield evaluation alone
		EXIT_CHECKS,       ///< evaluations of the "enter only if the exit is clear" rule
		EXIT_WAITS,        ///< of those, the ones that made the vehicle wait
		EXIT_TIME_US,      ///< time spent in the exit evaluation alone
		MAX_COUNTER
	};

private:
	struct watched_tile_t {
		koord3d pos;
		uint32 passes[2]; ///< [0] traffic lane, [1] passing lane
		watched_tile_t() : pos(koord3d::invalid) { passes[0] = passes[1] = 0; }
		watched_tile_t(koord3d p) : pos(p) { passes[0] = passes[1] = 0; }
		bool operator==(const watched_tile_t &o) const { return pos == o.pos; }
	};

	static bool enabled;
	static sint64 counters[MAX_COUNTER];
	static vector_tpl<watched_tile_t> watched_tiles;

public:
	static bool is_enabled() { return enabled; }
	static void set_enabled(bool yesno) { enabled = yesno; }

	/// clears all counters and pass counts, but keeps the list of watched tiles
	static void reset();

	static void add(counter_t c, sint64 v = 1) { if(  enabled  ) { counters[c] += v; } }
	static sint64 get(counter_t c) { return (c >= 0  &&  c < MAX_COUNTER) ? counters[c] : 0; }
	/// @returns the counter with the given (script facing) name, -1 for an unknown name
	static sint64 get(const char *name);

	/// count lane usage on this tile from now on
	static void watch_tile(koord3d pos);
	/// a vehicle (the front of a convoy, or a city car) entered pos on the given lane
	static void book_lane_pass(koord3d pos, bool passing_lane);
	static uint32 get_lane_passes(koord3d pos, bool passing_lane);

	/// monotonic clock for the *_TIME_US counters
	static sint64 now_us();
};


/**
 * Adds the time between construction and destruction to a *_TIME_US counter.
 * Does not even read the clock while measurement is disabled.
 * Pass MAX_COUNTER to measure nothing.
 */
class merge_timer_t
{
	merge_stats_t::counter_t counter;
	sint64 start;
public:
	explicit merge_timer_t(merge_stats_t::counter_t c) : counter(c), start(merge_stats_t::is_enabled()  &&  c < merge_stats_t::MAX_COUNTER ? merge_stats_t::now_us() : -1) {}
	~merge_timer_t() { if(  start >= 0  ) { merge_stats_t::add(counter, merge_stats_t::now_us() - start); } }
};

#endif
