//
// This file is part of the Simutrans project under the Artistic License.
// (see LICENSE.txt)
//

//
// Two-lane one-way roundabout (OTRP oneway_mode), used as the fixture for the
// merge control tests and benchmarks. Drive on the right: circulation is
// counter-clockwise, entering vehicles turn right.
//
//   y=2      . . . . . . . 7 8 9          north loop, depot at (9,2)
//   y=3..5   . . . . . . . I O            I = inflow branch, O = outflow branch
//   y=6      . . . . . . + J J +          ring top    (travel west)
//   y=7      . . W O O O O J . . J I I I I    ring left   (travel south)
//   y=8      . . W I I I I J . . J O O O E    ring right  (travel north)
//   y=9      . . . . . . + J J +          ring bottom (travel east)
//   y=10..12 . . . . . . . O I
//   y=13     . . . . . . . S S
//
// Every side has an outflow junction followed by an inflow junction in the
// direction of circulation. The branches of a side are joined at their far
// end, so vehicles leaving the ring come back to it.
//
// oneway_mode roads only allow travel in the direction they were built in, so
// every segment below is built in its direction of travel.
//

local RA = {
	depot = coord3d(9, 2, 0)
	// far ends of the four loops, in the order a bus visits them
	loop_ends = [ coord3d(8, 2, 0), coord3d(2, 7, 0), coord3d(7, 13, 0), coord3d(13, 8, 0) ]
}

// [junction x, y, outflow?, ring travel dir, branch travel dir]
function _ra_junctions()
{
	return [
		[8, 6, true,  dir.west,  dir.north],
		[7, 6, false, dir.west,  dir.south],
		[6, 7, true,  dir.south, dir.west],
		[6, 8, false, dir.south, dir.east],
		[7, 9, true,  dir.east,  dir.south],
		[8, 9, false, dir.east,  dir.north],
		[9, 8, true,  dir.north, dir.east],
		[9, 7, false, dir.north, dir.west]
	]
}

// branch tiles next to the ring and the travel direction on them
function _ra_branch_tiles()
{
	return [
		[8, 5, dir.north], [7, 5, dir.south],
		[5, 7, dir.west],  [5, 8, dir.east],
		[7, 10, dir.south], [8, 10, dir.north],
		[10, 8, dir.east],  [10, 7, dir.west]
	]
}

function _ra_reverse(d)
{
	return d == dir.north ? dir.south : d == dir.south ? dir.north : d == dir.east ? dir.west : dir.east
}

function _ra_road()
{
	return way_desc_x.get_available_ways(wt_road, st_flat)[0]
}

function _ra_build()
{
	local pl = player_x(0)
	local road = _ra_road()
	local oneway = {overtaking_mode = oneway_mode}
	local segments = [
		// ring
		[9,6, 6,6], [6,6, 6,9], [6,9, 9,9], [9,9, 9,6],
		// north loop
		[8,6, 8,2], [8,2, 7,2], [7,2, 7,6],
		// west loop
		[6,7, 2,7], [2,7, 2,8], [2,8, 6,8],
		// south loop
		[7,9, 7,13], [7,13, 8,13], [8,13, 8,9],
		// east loop
		[9,8, 13,8], [13,8, 13,7], [13,7, 9,7]
	]
	foreach (s in segments) {
		ASSERT_EQUAL(command_x.build_road(pl, coord3d(s[0], s[1], 0), coord3d(s[2], s[3], 0), road, false, oneway), null)
	}
	// a depot is left in the direction opposite to the one it is entered in
	ASSERT_EQUAL(command_x.build_road(pl, coord3d(8, 2, 0), RA.depot, road, false, {overtaking_mode = twoway_mode}), null)
	ASSERT_EQUAL(command_x.build_depot(pl, RA.depot, building_desc_x("CarDepot")), null)
}

// inflow junctions with the branch tile in front of them
function _ra_inflows()
{
	local res = []
	foreach (j in _ra_junctions()) {
		if (!j[2]) {
			local pos = coord3d(j[0], j[1], 0)
			local back = _ra_reverse(j[4])
			local approach = coord3d(pos.x + (back == dir.east ? 1 : back == dir.west ? -1 : 0), pos.y + (back == dir.south ? 1 : back == dir.north ? -1 : 0), 0)
			res.append({ junction = pos, approach = approach, ring = j[3], branch = j[4] })
		}
	}
	return res
}

// starts a bus from the fixture depot driving through all loops
function _ra_start_bus()
{
	local pl = player_x(0)
	local depot = depot_x(RA.depot.x, RA.depot.y, RA.depot.z)
	depot.append_vehicle(pl, convoy_x(0), vehicle_desc_x("Buessig"))
	local cnv = depot.get_convoy_list()[0]
	local entries = []
	foreach (p in RA.loop_ends) {
		entries.append(schedule_entry_x(p, 0, 0))
	}
	cnv.change_schedule(pl, schedule_x(wt_road, entries))
	depot.start_all_convoys(pl)
	return cnv
}


//
// Tests
//

function test_roundabout_fixture_directions()
{
	_ra_build()
	foreach (b in _ra_branch_tiles()) {
		ASSERT_EQUAL(tile_x(b[0], b[1], 0).get_way_dirs_masked(wt_road), b[2])
	}
	foreach (j in _ra_junctions()) {
		local t = tile_x(j[0], j[1], 0)
		local branch_side = j[2] ? j[4] : _ra_reverse(j[4])
		ASSERT_EQUAL(t.get_way_dirs(wt_road), _ra_reverse(j[3]) | j[3] | branch_side)
		// ring traffic goes on, at an outflow it may also leave
		ASSERT_EQUAL(t.get_way_dirs_masked(wt_road), j[2] ? (j[3] | j[4]) : j[3])
	}
	ASSERT_EQUAL(_ra_inflows().len(), 4)
}

// One bus visits all four loops in order. It has to use the inflows and
// outflows and travel the ring in the direction of circulation.
function test_roundabout_fixture_circulates()
{
	_ra_build()
	local watch = [coord3d(6, 6, 0), coord3d(6, 9, 0), coord3d(9, 9, 0), coord3d(9, 6, 0)]
	foreach (p in watch) {
		debug.watch_lane_passes(p)
	}
	debug.reset_merge_stats()
	debug.set_merge_stats_enabled(true)

	local cnv = _ra_start_bus()
	debug.set_game_speed(5)

	// the schedule index moves on whenever an entry has been reached
	local reached = 0
	local last = 0
	for (local i = 0; i < 6000  &&  reached < RA.loop_ends.len(); i++) {
		sleep()
		ASSERT_TRUE(cnv.is_valid())
		local cur = cnv.get_schedule().current
		if (cur != last) {
			reached++
			last = cur
		}
	}
	debug.set_merge_stats_enabled(false)
	ASSERT_EQUAL(reached, RA.loop_ends.len())

	// the legs north->west, west->south and south->east each take one corner of the ring;
	// the north-east corner only lies on the way back to the north loop, not driven yet.
	local expected = [1, 1, 1, 0]
	foreach (i, p in watch) {
		ASSERT_EQUAL(debug.get_lane_passes(p, false) + debug.get_lane_passes(p, true), expected[i])
	}
	ASSERT_GREATER(debug.get_merge_stat("enter_checks"), 0)
	ASSERT_EQUAL(debug.get_merge_stat("no_such_counter"), -1)
}


//
// Benchmarks. Not part of all_tests: run them one at a time with
//   tests/run-automated-tests.sh bench_roundabout_uniform
// and read the "BENCH" lines of script-scenario.log.
//

// starts a bus with the given loop ends as waypoints (indices into RA.loop_ends)
function _ra_bench_bus(stops, min_load = 0)
{
	local pl = player_x(0)
	local depot = depot_x(RA.depot.x, RA.depot.y, RA.depot.z)
	depot.append_vehicle(pl, convoy_x(0), vehicle_desc_x("Buessig"))
	local list = depot.get_convoy_list()
	local cnv = list[list.len() - 1]
	local entries = []
	foreach (s in stops) {
		entries.append(typeof s == "integer" ? schedule_entry_x(RA.loop_ends[s], 0, 0) : schedule_entry_x(s, min_load, 0))
	}
	cnv.change_schedule(pl, schedule_x(wt_road, entries))
	depot.start_convoy(pl, cnv)
	return cnv
}

function _ra_percentile(values, p)
{
	if (values.len() == 0) return 0
	values.sort()
	local i = (values.len() * p) / 100
	return values[i < values.len() ? i : values.len() - 1]
}

// pattern: "uniform", "ring" or "blocked"; citycars: number of city cars added at the start
function _ra_bench(name, pattern, citycars, setup = null, bus_count = 24)
{
	_ra_build()
	if (setup) setup()
	local pl = player_x(0)

	// counting points: branch tiles behind the outflows, and the ring corners per lane
	local exits = []
	foreach (b in _ra_branch_tiles()) {
		local p = coord3d(b[0], b[1], 0)
		// the outflow branch tile is the one leading away from the ring
		if ((b[2] == dir.north && b[1] < 6) || (b[2] == dir.west && b[0] < 6) || (b[2] == dir.south && b[1] > 9) || (b[2] == dir.east && b[0] > 9)) {
			exits.append(p)
			debug.watch_lane_passes(p)
		}
	}
	local corners = [coord3d(6, 6, 0), coord3d(6, 9, 0), coord3d(9, 9, 0), coord3d(9, 6, 0)]
	foreach (p in corners) {
		debug.watch_lane_passes(p)
	}
	local approaches = []
	foreach (f in _ra_inflows()) {
		approaches.append(f.approach)
	}

	debug.set_game_speed(5)
	local stop = coord3d(12, 8, 0)
	local blockers = []
	if (pattern == "blocked") {
		ASSERT_EQUAL(command_x(tool_build_station).work(pl, stop, "BusStop"), null)
	}

	// buses leave the depot one after the other
	local buses = []
	for (local k = 0; k < bus_count; k++) {
		local stops
		if (pattern == "ring") {
			// north -> east takes three quarters of the ring
			stops = k % 2 == 0 ? [0, 3] : [3, 0]
		}
		else {
			stops = [k % 4, (k + 1) % 4, (k + 2) % 4, (k + 3) % 4]
		}
		buses.append(_ra_bench_bus(stops))
		for (local i = 0; i < 25; i++) sleep()
	}
	if (pattern == "blocked") {
		// these wait for a full load forever at the east outflow and back up into the ring
		for (local k = 0; k < 4; k++) {
			blockers.append(_ra_bench_bus([stop, 0], 100))
			for (local i = 0; i < 40; i++) sleep()
		}
	}
	local cars_added = 0
	for (local k = 0; k < citycars; k++) {
		local b = _ra_branch_tiles()[k % 8]
		try {
			if (command_x(tool_add_citycar).work(pl, coord3d(b[0], b[1], 0)) == null) cars_added++
		} catch (e) {}
		for (local i = 0; i < 10; i++) sleep()
	}

	debug.reset_merge_stats()
	debug.set_merge_stats_enabled(true)

	local t0 = world.get_time().ticks
	local waits = {}        // convoy id -> tick when it stopped in front of an inflow
	local wait_samples = [] // finished waits in ticks
	local max_wait = [0, 0, 0, 0]
	local last_total = 0
	local last_progress = t0
	local longest_stall = 0
	local released = -1
	local recovered = -1

	local steps = 3000
	for (local i = 0; i < steps; i++) {
		sleep()
		local now = world.get_time().ticks
		foreach (cnv in buses) {
			if (!cnv.is_valid()) continue
			local p = cnv.get_pos()
			local at = -1
			foreach (k, a in approaches) {
				if (a.x == p.x  &&  a.y == p.y) at = k
			}
			local id = cnv.id
			if (at >= 0  &&  cnv.get_speed() == 0) {
				if (!(id in waits)) waits[id] <- [now, at]
			}
			else if (id in waits) {
				local w = now - waits[id][0]
				wait_samples.append(w)
				if (w > max_wait[waits[id][1]]) max_wait[waits[id][1]] = w
				delete waits[id]
			}
		}
		local total = 0
		foreach (p in exits) {
			total += debug.get_lane_passes(p, false) + debug.get_lane_passes(p, true)
		}
		if (total != last_total) {
			if (now - last_progress > longest_stall) longest_stall = now - last_progress
			last_progress = now
			if (released >= 0  &&  recovered < 0) recovered = now - released
			last_total = total
		}
		if (pattern == "blocked"  &&  i == steps / 2) {
			// lift the blockage
			foreach (cnv in blockers) {
				if (cnv.is_valid()) cnv.change_schedule(pl, schedule_x(wt_road, [schedule_entry_x(RA.loop_ends[0], 0, 0)]))
			}
			released = now
		}
	}
	debug.set_merge_stats_enabled(false)
	local now = world.get_time().ticks
	if (now - last_progress > longest_stall) longest_stall = now - last_progress

	local ring = [0, 0]
	foreach (p in corners) {
		ring[0] += debug.get_lane_passes(p, false)
		ring[1] += debug.get_lane_passes(p, true)
	}
	local exits_per_branch = ""
	foreach (p in exits) {
		exits_per_branch += " " + (debug.get_lane_passes(p, false) + debug.get_lane_passes(p, true))
	}
	print("BENCH " + name + " citycars=" + cars_added + " ticks=" + (now - t0))
	print("BENCH " + name + " exits_total=" + last_total + " exits_per_branch=" + exits_per_branch)
	print("BENCH " + name + " ring_traffic_lane=" + ring[0] + " ring_passing_lane=" + ring[1])
	print("BENCH " + name + " wait_max_per_inflow=" + max_wait[0] + " " + max_wait[1] + " " + max_wait[2] + " " + max_wait[3] + " wait_p95=" + _ra_percentile(wait_samples, 95) + " waits=" + wait_samples.len())
	print("BENCH " + name + " longest_stall=" + longest_stall + " recovery=" + recovered)
	foreach (c in ["enter_checks", "enter_time_us", "citycar_checks", "citycar_time_us", "yield_checks", "yield_waits", "yield_time_us", "exit_checks", "exit_waits", "exit_time_us"]) {
		print("BENCH " + name + " " + c + "=" + debug.get_merge_stat(c))
	}
	ASSERT_GREATER(last_total, 0)
}

function bench_roundabout_uniform()        { _ra_bench("uniform", "uniform", 0) }
function bench_roundabout_ring()           { _ra_bench("ring", "ring", 0) }
function bench_roundabout_blocked()        { _ra_bench("blocked", "blocked", 0) }
function bench_roundabout_mixed()          { _ra_bench("mixed", "uniform", 16) }
