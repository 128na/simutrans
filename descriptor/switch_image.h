/*
 * This file is part of the Simutrans project under the Artistic License.
 * (see LICENSE.txt)
 */

#ifndef DESCRIPTOR_SWITCH_IMAGE_H
#define DESCRIPTOR_SWITCH_IMAGE_H


#include "../simtypes.h"
#include "../dataobj/ribi.h"


/**
 * Junction (switch) images of a way (OTRP).
 *
 * The images of a way are one list per season and back/front. Numbers 0..15 are the
 * standard images by ribi. A junction image is identified by its number in this list,
 * which is also its "role": the same number is used for the fixed choice of a tile,
 * the save game and makeobj. Every role belongs to one tile shape (a T junction of one
 * direction or the four-way crossing) and is never drawn on a tile of another shape.
 *
 * Shape index: 0 = nse, 1 = new, 2 = nsw, 3 = sew, 4 = nsew.
 * The trunk of a three-way switch is named by the T shape whose branch points the same way:
 * nse = east, new = north, nsw = west, sew = south.
 */
namespace switch_image
{
	enum {
		STANDARD_IMAGE = 0,
		SWITCH1       = 16, ///< + shape (0..4): switch state 1, curve through the north-east/south-west corners (shared with standard Simutrans)
		SWITCH2       = 21, ///< + shape (0..4): switch state 2, curve through the north-west/south-east corners (shared with standard Simutrans)
		Y_JUNCTION    = 26, ///< + T shape (0..3): both sides of the main line turn away from the branch
		PLAIN_CROSSING= 30, ///< four-way: two lines crossing without any turning curves
		THREE_WAY     = 31, ///< + T shape (0..3): four-way three-way switch, trunk on the branch side of the T shape
		CROSSOVER     = 35, ///< four-way: the crossover of the KUTA version
		LANE_BASE     = 35, ///< + mask (1..15) of the neighbouring junction tiles: multi-lane crossings
		FIRST_ROLE    = 16,
		LAST_ROLE     = 50,
		COUNT         = 51  ///< number of images in a list with all roles
	};

	/// shape index of a junction ribi, or 255 if it is no junction
	inline uint8 shape_of(ribi_t::ribi ribi)
	{
		static const uint8 shape[16] = {
			255, 255, 255, 255, 255, 255, 255, 0,
			255, 255, 255, 1, 255, 2, 3, 4
		};
		return shape[ribi & 15];
	}

	/// ribi of a shape index (0..4)
	inline ribi_t::ribi ribi_of_shape(uint8 shape)
	{
		static const ribi_t::ribi ribi[5] = { ribi_t::northsoutheast, ribi_t::northeastwest, ribi_t::northsouthwest, ribi_t::southeastwest, ribi_t::all };
		return ribi[shape < 5 ? shape : 4];
	}

	/// ribi of the tile shape this role is drawn on (0 if @p nr is no role)
	inline ribi_t::ribi shape_ribi_of_role(uint8 nr)
	{
		if(  nr >= SWITCH1  &&  nr < Y_JUNCTION  ) {
			return ribi_of_shape( (uint8)((nr - SWITCH1) % 5) );
		}
		if(  nr >= Y_JUNCTION  &&  nr < PLAIN_CROSSING  ) {
			return ribi_of_shape( nr - Y_JUNCTION );
		}
		if(  nr >= PLAIN_CROSSING  &&  nr <= LAST_ROLE  ) {
			return ribi_t::all;
		}
		return ribi_t::none;
	}

	/// true if role @p nr is drawn on a tile with the connections @p ribi
	inline bool fits(uint8 nr, ribi_t::ribi ribi)
	{
		return nr >= FIRST_ROLE  &&  nr <= LAST_ROLE  &&  shape_ribi_of_role(nr) == (ribi & 15);
	}

	/// role of the fixed choices 1..4 of the first version (before the roles), 0 if it fits no role
	inline uint8 from_old_choice(sint8 value, ribi_t::ribi ribi)
	{
		const uint8 shape = shape_of(ribi);
		if(  shape > 4  ) {
			return 0;
		}
		uint8 nr = 0;
		switch(  value  ) {
			case 1: nr = SWITCH2 + shape; break;
			case 2: nr = SWITCH1 + shape; break;
			case 3: nr = 31 + shape; break; // extended image, nw = true
			case 4: nr = 26 + shape; break; // extended image, nw = false
			default: return 0;
		}
		return fits(nr, ribi) ? nr : 0;
	}

	/// role after the map was rotated by 90 degrees (like ribi_t::rotate90)
	inline uint8 rotate90(uint8 nr)
	{
		// nse -> sew, new -> nse, nsw -> new, sew -> nsw, nsew -> nsew
		static const uint8 rotated_shape[5] = { 3, 0, 1, 2, 4 };
		if(  nr >= SWITCH1  &&  nr < Y_JUNCTION  ) {
			// the switch states are tied to the corners of the screen: state 1 and 2 swap
			const uint8 shape = (uint8)((nr - SWITCH1) % 5);
			return (uint8)((nr >= SWITCH2 ? SWITCH1 : SWITCH2) + rotated_shape[shape]);
		}
		if(  nr >= Y_JUNCTION  &&  nr < PLAIN_CROSSING  ) {
			return Y_JUNCTION + rotated_shape[nr - Y_JUNCTION];
		}
		if(  nr >= THREE_WAY  &&  nr < CROSSOVER  ) {
			return THREE_WAY + rotated_shape[nr - THREE_WAY];
		}
		if(  nr > LANE_BASE  &&  nr <= LAST_ROLE  ) {
			return LANE_BASE + ribi_t::rotate90( nr - LANE_BASE );
		}
		return nr; // plain crossing, crossover
	}

	/// key of image @p nr in the dat file, e.g. "nse1" or "nsew_lane_ne" (NULL if out of range)
	inline const char *key(uint8 nr)
	{
		static const char *const keys[COUNT] = {
			"-", "n",  "e",  "ne",  "s",  "ns",  "se",  "nse",
			"w", "nw", "ew", "new", "sw", "nsw", "sew", "nsew",
			"nse1", "new1", "nsw1", "sew1", "nsew1", // different crossings: northwest/southeast is straight
			"nse2", "new2", "nsw2", "sew2", "nsew2",
			"nse_y", "new_y", "nsw_y", "sew_y",
			"nsew_plain",
			"nsew_3e", "nsew_3n", "nsew_3w", "nsew_3s",
			"nsew_xover",
			"nsew_lane_n", "nsew_lane_e", "nsew_lane_ne", "nsew_lane_s", "nsew_lane_ns", "nsew_lane_se", "nsew_lane_nse",
			"nsew_lane_w", "nsew_lane_nw", "nsew_lane_ew", "nsew_lane_new", "nsew_lane_sw", "nsew_lane_nsw", "nsew_lane_sew", "nsew_lane_nsew"
		};
		return nr < COUNT ? keys[nr] : NULL;
	}
}

#endif
