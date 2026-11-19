/*
 * This file is part of the Simutrans project under the Artistic License.
 * (see LICENSE.txt)
 */

#ifndef DESCRIPTOR_WAY_OBJ_DESC_H
#define DESCRIPTOR_WAY_OBJ_DESC_H


#include "image_list.h"
#include "obj_base_desc.h"
#include "skin_desc.h"
#include "../dataobj/ribi.h"
#include "switch_image.h"
#include "../network/checksum.h"


class tool_t;
class checksum_t;

/**
 * Way objects type description (like overhead lines).
 *
 * Child nodes:
 *  0  Name
 *  1  Copyright
 *  2  Image on flat ways
 *  3  Image on sloped ways
 *  4  Image on diagonal ways
 *  5  Skin (cursor and icon)
 */
class way_obj_desc_t : public obj_desc_transport_infrastructure_t {
	friend class way_obj_reader_t;

private:

	/**
	 * Type of the object, only overheadlines_wt is currently used.
	 */
	uint8 own_wtyp;

	// cached, set in way_obj_reader_t::register_obj()
	bool diagonals : 1;
	bool close_diagonals : 1;

public:

	bool is_overhead_line() const { return (waytype_t)own_wtyp == overheadlines_wt; }

	/**
	* Maintenance cost scaled by settings_t::maintenance_cost_multiplier_overhead [%].
	* Hides obj_desc_transport_related_t::get_maintenance() on purpose, so that
	* every user of a way_obj_desc_t sees the scaled value.
	* Implemented in way_desc.cc.
	*/
	sint64 get_maintenance() const;

	// way objects can have a front and a backimage, unlike ways ...
	image_id get_front_image_id(ribi_t::ribi ribi) const { return get_child<image_list_t>(2)->get_image_id(ribi); }

	/// image of the junction role @p nr (see switch_image.h) in the front or back list
	image_id get_switch_role_image_id(uint8 nr, bool front) const
	{
		return get_child<image_list_t>(front ? 2 : 3)->get_image_id(nr);
	}

	/**
	 * True if the add-on draws junction role @p nr: in the front or back list the image is not
	 * empty and not the same image as the standard image of that tile shape (identical images
	 * are merged while loading, so a server without graphics gives the same answer).
	 */
	bool has_switch_role_image(uint8 nr) const
	{
		const ribi_t::ribi shape = switch_image::shape_ribi_of_role(nr);
		if(  shape == ribi_t::none  ) {
			return false;
		}
		for(  int i = 2;  i <= 3;  i++  ) {
			image_list_t const* const list = get_child<image_list_t>(i);
			image_t const* const role = list->get_image(nr);
			if(  role  &&  role->imageid != IMG_EMPTY  &&  role != list->get_image(shape)  ) {
				return true;
			}
		}
		return false;
	}

	image_id get_back_image_id(ribi_t::ribi ribi) const { return get_child<image_list_t>(3)->get_image_id(ribi); }

	image_id get_front_slope_image_id(slope_t::type slope) const
	{
		uint16 nr;
		switch(slope) {
			case 4:
				nr = 0;
				break;
			case 12:
				nr = 1;
				break;
			case 28:
				nr = 2;
				break;
			case 36:
				nr = 3;
				break;
			case 8:
				nr = 4;
				break;
			case 24:
				nr = 5;
				break;
			case 56:
				nr = 6;
				break;
			case 72:
				nr = 7;
				break;
			default:
				return IMG_EMPTY;
		}
		image_id slope_img = get_child<image_list_t>(4)->get_image_id(nr);
		if(  nr > 3  &&  slope_img == IMG_EMPTY  ) {
			// hack for old ways without double height images to use single slope images for both
			nr -= 4;
			slope_img = get_child<image_list_t>(4)->get_image_id(nr);
		}
		return slope_img;
	}

	image_id get_back_slope_image_id(slope_t::type slope) const
	{
		uint16 nr;
		switch(slope) {
			case 4:
				nr = 0;
				break;
			case 12:
				nr = 1;
				break;
			case 28:
				nr = 2;
				break;
			case 36:
				nr = 3;
				break;
			case 8:
				nr = 4;
				break;
			case 24:
				nr = 5;
				break;
			case 56:
				nr = 6;
				break;
			case 72:
				nr = 7;
				break;
			default:
				return IMG_EMPTY;
		}
		image_id slope_img = get_child<image_list_t>(5)->get_image_id(nr);
		if(  nr > 3  &&  slope_img == IMG_EMPTY  ) {
			// hack for old ways without double height images to use single slope images for both
			nr -= 4;
			slope_img = get_child<image_list_t>(5)->get_image_id(nr);
		}
		return slope_img;
	}

	image_id get_front_diagonal_image_id(ribi_t::ribi ribi) const
	{
		if(!ribi_t::is_bend(ribi)) {
			return IMG_EMPTY;
		}
		return get_child<image_list_t>(6)->get_image_id(ribi / 3 - 1);
	}

	image_id get_back_diagonal_image_id(ribi_t::ribi ribi) const
	{
		if(!ribi_t::is_bend(ribi)) {
			return IMG_EMPTY;
		}
		return get_child<image_list_t>(7)->get_image_id(ribi / 3 - 1);
	}

	// the back diagonal image, or the front one when only that exists
	image_id get_back_or_front_diagonal_image_id(ribi_t::ribi ribi) const
	{
		const image_id back = get_back_diagonal_image_id(ribi);
		return back != IMG_EMPTY ? back : get_front_diagonal_image_id(ribi);
	}

	/**
	 * close diagonals: two parallel diagonals one tile apart (on a fourway way tile)
	 * nr=0: close1 (northwest+southeast), nr=1: close2 (northeast+southwest)
	 */
	image_id get_front_close_diagonal_image_id(uint8 nr) const
	{
		return nr < 2 ? get_child<image_list_t>(6)->get_image_id(4 + nr) : IMG_EMPTY;
	}

	image_id get_back_close_diagonal_image_id(uint8 nr) const
	{
		return nr < 2 ? get_child<image_list_t>(7)->get_image_id(4 + nr) : IMG_EMPTY;
	}

	bool has_diagonal_image() const { return diagonals; }
	bool has_close_diagonal_image() const { return close_diagonals; }

	/// true if the lists have junction images (number 16 and later)
	bool has_switch_image() const {
		return get_child<image_list_t>(2)->get_count() > switch_image::FIRST_ROLE
		||     get_child<image_list_t>(3)->get_count() > switch_image::FIRST_ROLE;
	}

	/**
	* Skin: cursor (index 0) and icon (index 1)
	*/
	skin_desc_t const* get_cursor() const { return get_child<skin_desc_t>(8); }


	void calc_checksum(checksum_t *chk) const
	{
		obj_desc_transport_infrastructure_t::calc_checksum(chk);
		chk->input(own_wtyp);
	}
};

#endif
