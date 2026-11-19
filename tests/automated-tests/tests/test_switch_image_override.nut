//
// Scripted integration test for fixed railway junction images.
//

// the tool parameter is "<waytype>,<choice>"
// choice: -1 automatic, 0 standard, 16..50 junction role (the number of the image in the add-on)
// 1..4 are the choices of the first version and are read as the matching role
function choose(tool, pl, pos, choice)
{
	return tool.work(pl, pos, "" + wt_rail + "," + choice)
}

function test_switch_image_override_scripted()
{
	local pl = player_x(0)
	local rail = null
	foreach (desc in way_desc_x.get_available_ways(wt_rail, st_flat)) {
		if (desc.has_switch_image()) {
			rail = desc
			break
		}
	}
	ASSERT_TRUE(rail != null)

	local junction = coord3d(7, 4, 0)
	ASSERT_EQUAL(command_x.build_way(pl, coord3d(3, 4, 0), coord3d(11, 4, 0), rail, true), null)
	ASSERT_EQUAL(command_x.build_way(pl, coord3d(7, 1, 0), junction, rail, true), null)
	ASSERT_EQUAL(way_x(junction.x, junction.y, junction.z).get_dirs(), dir.northeastwest)
	// roles of this T junction (north-east-west): switch state 1 and 2, Y junction
	local state1 = 17, state2 = 22, y_junction = 27

	local set_image = command_x(tool_set_switch_image)
	local way = way_x(junction.x, junction.y, junction.z)
	ASSERT_EQUAL(way.get_switch_image_override(), -1)
	// invalid choices and choices without images in the add-on are rejected
	foreach (v in ["5", "15", "51", "-2"]) {
		ASSERT_EQUAL(choose(set_image, pl, junction, v), "")
	}
	ASSERT_EQUAL(set_image.work(pl, junction, "1"), "")
	ASSERT_EQUAL(set_image.work(pl, junction, "" + wt_road + ",1"), "")
	// roles of other tile shapes
	foreach (v in ["16", "20", "21", "26", "30", "31", "35", "36", "50"]) {
		ASSERT_EQUAL(choose(set_image, pl, junction, v), "")
	}
	// the three-way switch of the first version (3) is a four-way role
	ASSERT_EQUAL(choose(set_image, pl, junction, "3"), "")
	if (!rail.has_switch_ex_image()) {
		ASSERT_EQUAL(choose(set_image, pl, junction, "" + y_junction), "")
		ASSERT_EQUAL(choose(set_image, pl, junction, "4"), "")
	}
	ASSERT_EQUAL(way.get_switch_image_override(), -1)
	ASSERT_EQUAL(choose(set_image, pl, junction, "0"), null)
	ASSERT_EQUAL(way.get_switch_image_override(), 0) // standard
	ASSERT_EQUAL(choose(set_image, pl, junction, "" + state1), null)
	ASSERT_EQUAL(way.get_switch_image_override(), state1)
	ASSERT_EQUAL(choose(set_image, pl, junction, "" + state2), null)
	ASSERT_EQUAL(way.get_switch_image_override(), state2)
	// choices of the first version
	ASSERT_EQUAL(choose(set_image, pl, junction, "1"), null)
	ASSERT_EQUAL(way.get_switch_image_override(), state2)
	ASSERT_EQUAL(choose(set_image, pl, junction, "2"), null)
	ASSERT_EQUAL(way.get_switch_image_override(), state1)

	// Each fixed choice must draw its own image: standard, switch state 1 and switch state 2.
	local images = []
	foreach (v in ["0", "" + state1, "" + state2]) {
		ASSERT_EQUAL(choose(set_image, pl, junction, v), null)
		images.append(way.get_image_id())
	}
	ASSERT_TRUE(images[0] != images[1])
	ASSERT_TRUE(images[0] != images[2])
	ASSERT_TRUE(images[1] != images[2])

	ASSERT_EQUAL(choose(set_image, pl, junction, "-1"), null)
	ASSERT_EQUAL(way.get_switch_image_override(), -1)
	ASSERT_EQUAL(choose(set_image, pl, junction, "" + state2), null)
	ASSERT_EQUAL(way.get_switch_image_override(), state2)
	ASSERT_EQUAL(way.get_image_id(), images[2])

	// Rebuilding the neighbouring image must keep the chosen image.
	ASSERT_EQUAL(command_x.build_way(pl, coord3d(8, 4, 0), coord3d(8, 5, 0), rail, true), null)
	ASSERT_EQUAL(way.get_switch_image_override(), state2)
	ASSERT_EQUAL(way.get_image_id(), images[2])
	ASSERT_EQUAL(command_x(tool_remove_way).work(pl, coord3d(8, 4, 0), coord3d(8, 5, 0), "" + wt_rail), null)
	ASSERT_EQUAL(way.get_image_id(), images[2])

	// Growing the T junction to a four-way must discard the fixed choice.
	ASSERT_EQUAL(command_x.build_way(pl, junction, coord3d(7, 7, 0), rail, true), null)
	ASSERT_EQUAL(way.get_dirs(), dir.all)
	ASSERT_EQUAL(way.get_switch_image_override(), -1)
	ASSERT_EQUAL(choose(set_image, pl, junction, "0"), null)
	ASSERT_EQUAL(way.get_switch_image_override(), 0)
	ASSERT_EQUAL(choose(set_image, pl, junction, "" + state1), "") // a role of the T junction
	ASSERT_EQUAL(choose(set_image, pl, junction, "2"), null)
	ASSERT_EQUAL(way.get_switch_image_override(), 20) // four-way, switch state 1
	ASSERT_EQUAL(choose(set_image, pl, junction, "20"), null)
	ASSERT_EQUAL(way.get_switch_image_override(), 20)

	// Removing an arm resolves the junction; a new override must be discarded too.
	ASSERT_EQUAL(command_x(tool_remove_way).work(pl, junction, coord3d(7, 7, 0), "" + wt_rail), null)
	ASSERT_TRUE(dir.is_threeway(way.get_dirs()))
	ASSERT_EQUAL(way.get_switch_image_override(), -1)
	ASSERT_EQUAL(choose(set_image, pl, junction, "" + state2), null)
	ASSERT_EQUAL(way.get_switch_image_override(), state2)
	ASSERT_EQUAL(command_x(tool_remove_way).work(pl, coord3d(7, 1, 0), junction, "" + wt_rail), null)
	ASSERT_FALSE(dir.is_threeway(way.get_dirs()))
	ASSERT_EQUAL(way.get_switch_image_override(), -1)

	ASSERT_EQUAL(command_x(tool_remove_way).work(pl, coord3d(3, 4, 0), coord3d(11, 4, 0), "" + wt_rail), null)
	RESET_ALL_PLAYER_FUNDS()
}
