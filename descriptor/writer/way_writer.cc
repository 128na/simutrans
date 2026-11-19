/*
 * This file is part of the Simutrans project under the Artistic License.
 * (see LICENSE.txt)
 */

#include <string>
#include "../../dataobj/tabfile.h"
#include "../../utils/simstring.h"
#include "obj_node.h"
#include "obj_pak_exception.h"
#include "../way_desc.h"
#include "text_writer.h"
#include "imagelist_writer.h"
#include "skin_writer.h"
#include "get_waytype.h"
#include "way_writer.h"

using std::string;


/// image of the flat way list @p nr ("image[nse1]" or "image[nse1][season]" if @p season >= 0)
static string get_flat_image(tabfileobj_t& obj, const char* type, uint8 nr, int season)
{
	char buf[64];
	if(  season < 0  ) {
		sprintf(buf, "%simage[%s]", type, switch_image::key(nr));
	}
	else {
		sprintf(buf, "%simage[%s][%d]", type, switch_image::key(nr), season);
	}
	return obj.get(buf);
}


/**
 * Number of images in the lists of flat ways: the 16 standard images, and the junction
 * images up to the highest one in the dat file (OTRP, see switch_image.h).
 * If there are junction images, at least the 26 images of standard Simutrans are written,
 * and if there are extended junction images, at least the 36 images older OTRP versions read.
 */
static uint8 get_flat_image_count(tabfileobj_t& obj, const char* const* image_type, int first_season, int last_season)
{
	uint8 highest = 0;
	for(  int backtofront = 0;  backtofront < 2;  backtofront++  ) {
		for(  int season = first_season;  season <= last_season;  season++  ) {
			for(  uint8 nr = switch_image::FIRST_ROLE;  nr < switch_image::COUNT;  nr++  ) {
				if(  nr > highest  &&  !get_flat_image(obj, image_type[backtofront], nr, season).empty()  ) {
					highest = nr;
				}
			}
		}
	}
	if(  highest >= switch_image::Y_JUNCTION  ) {
		return (uint8)max(highest + 1, switch_image::CROSSOVER + 1);
	}
	if(  highest >= switch_image::FIRST_ROLE  ) {
		return (uint8)max(highest + 1, switch_image::Y_JUNCTION);
	}
	return 16;
}


/**
 * Image @p nr of the flat way list. A missing junction image is filled with the standard image
 * of the tile shape it belongs to, so the game (and older versions, which draw some of these
 * images on other tiles) never shows an empty image. Identical images are recognized as
 * "not drawn" by the game.
 */
static string get_flat_image_or_standard(tabfileobj_t& obj, const char* type, uint8 nr, int season, uint8 wtyp)
{
	string str = get_flat_image(obj, type, nr, season);
	if(  str.empty()  &&  nr >= switch_image::FIRST_ROLE  ) {
		ribi_t::ribi shape = switch_image::shape_ribi_of_role(nr);
		if(  wtyp == road_wt  &&  nr >= switch_image::THREE_WAY  &&  nr < switch_image::CROSSOVER  ) {
			// older OTRP versions draw these images on T junctions of roads
			shape = switch_image::ribi_of_shape(nr - switch_image::THREE_WAY);
		}
		str = get_flat_image(obj, type, shape, season);
	}
	return str;
}

/**
 * Write a waytype description node
 */
void way_writer_t::write_obj(FILE* outfp, obj_node_t& parent, tabfileobj_t& obj)
{
	static const char* const ribi_codes[16] = {
		"-", "n",  "e",  "ne",  "s",  "ns",  "se",  "nse",
		"w", "nw", "ew", "new", "sw", "nsw", "sew", "nsew",
	};
	int ribi, slope;

	obj_node_t node(this, 28, &parent);


	// Version needs high bit set as trigger -> this is required
	// as marker because formerly nodes were unversionend
	uint16 version     = 0x8006;
	uint32 price       = obj.get_int("cost",        100);
	uint32 maintenance = obj.get_int("maintenance", 100);
	sint32 topspeed    = obj.get_int("topspeed",    999);
	uint32 max_weight  = obj.get_int("max_weight",  999);
	uint16 axle_load   = obj.get_int("axle_load",  9999);

	uint16 intro = obj.get_int("intro_year", DEFAULT_INTRO_DATE) * 12;
	intro += obj.get_int("intro_month", 1) - 1;

	uint16 retire = obj.get_int("retire_year", DEFAULT_RETIRE_DATE) * 12;
	retire += obj.get_int("retire_month", 1) - 1;

	uint8 wtyp = get_waytype(obj.get("waytype"));
	uint8 styp = obj.get_int("system_type", 0);
	// compatibility conversions
	if (wtyp == track_wt && styp == 5) {
		wtyp = monorail_wt;
	}
	else if (wtyp == track_wt && styp == 7) {
		wtyp = tram_wt;
	}

	// true to draw as foregrund and not much earlier (default)
	uint8 draw_as_ding = (obj.get_int("draw_as_ding", 0) == 1);
	sint8 number_of_seasons = 0;

	node.write_uint16(outfp, version,       0);
	node.write_uint32(outfp, price,         2);
	node.write_uint32(outfp, maintenance,   6);
	node.write_sint32(outfp, topspeed,     10);
	node.write_uint32(outfp, max_weight,   14);
	node.write_uint16(outfp, intro,        18);
	node.write_uint16(outfp, retire,       20);
	node.write_uint16(outfp, axle_load,    22);
	node.write_uint8 (outfp, wtyp,         24);
	node.write_uint8 (outfp, styp,         25);
	node.write_uint8 (outfp, draw_as_ding, 26);

	static const char* const image_type[] = { "", "front" };

	slist_tpl<string> keys;
	char buf[40];
	sprintf(buf, "image[%s][0]", ribi_codes[0]);
	string str = obj.get(buf);
	if (str.empty()) {
		node.write_data_at(outfp, &number_of_seasons, 27, 1);
		write_head(outfp, node, obj);

		sprintf(buf, "image[%s]", ribi_codes[0]);
		string str = obj.get(buf);
		if (str.empty()) {
			dbg->fatal("way_writer_t::write_obj", "image with label %s missing", buf);
		}
		// standard images and junction images (way images defined without seasons)
		const uint8 ribinr = get_flat_image_count(obj, image_type, -1, -1);
		for(int backtofront = 0; backtofront<2; backtofront++) {
			for (ribi = 0; ribi < ribinr; ribi++) {
				keys.append(get_flat_image_or_standard(obj, image_type[backtofront], ribi, -1, wtyp));
			}
			imagelist_writer_t::instance()->write_obj(outfp, node, keys);

			keys.clear();
			for(  slope = 3;  slope <= 12;  slope += 3  ) {
				char buf[40];

				sprintf( buf, "%simageup[%d]", image_type[backtofront], slope );
				string str = obj.get(buf);
				keys.append(str);
			}
			for(  slope = 3;  slope <= 12;  slope += 3  ) {
				char buf[40];

				sprintf( buf, "%simageup2[%d]", image_type[backtofront], slope );
				string str = obj.get(buf);
				if(  !str.empty()  ) {
					keys.append(str);
				}
			}
			imagelist_writer_t::instance()->write_obj(outfp, node, keys);

			keys.clear();
			for (ribi = 3; ribi <= 12; ribi += 3) {
				char buf[40];

				sprintf(buf, "%sdiagonal[%s]", image_type[backtofront], ribi_codes[ribi]);
				string str = obj.get(buf);
				keys.append(str);
			}
			imagelist_writer_t::instance()->write_obj(outfp, node, keys);
			keys.clear();


			if(backtofront == 0) {
				slist_tpl<string> cursorkeys;

				cursorkeys.append(string(obj.get("cursor")));
				cursorkeys.append(string(obj.get("icon")));

				cursorskin_writer_t::instance()->write_obj(outfp, node, obj, cursorkeys);
			}
		}
	} else {
		sprintf(buf, "image[%s][%d]", ribi_codes[0], number_of_seasons+1);
		if (!strempty(obj.get(buf))) {
			number_of_seasons++;
		}

		node.write_data_at(outfp, &number_of_seasons, 27, 1);
		write_head(outfp, node, obj);

		// standard images and junction images
		const uint8 ribinr = get_flat_image_count(obj, image_type, 0, number_of_seasons);

		for(int backtofront = 0; backtofront<2; backtofront++) {
			for (uint8 season = 0; season <= number_of_seasons ; season++) {
				for (ribi = 0; ribi < ribinr; ribi++) {
					keys.append(get_flat_image_or_standard(obj, image_type[backtofront], ribi, season, wtyp));
				}
				imagelist_writer_t::instance()->write_obj(outfp, node, keys);

				keys.clear();
				for(  slope = 3;  slope <= 12;  slope += 3  ) {
					char buf[40];

					sprintf( buf, "%simageup[%d][%d]", image_type[backtofront], slope, season );
					string str = obj.get(buf);
					keys.append(str);
				}
				for(  slope = 3;  slope <= 12;  slope += 3  ) {
					char buf[40];

					sprintf( buf, "%simageup2[%d][%d]", image_type[backtofront], slope, season );
					string str = obj.get(buf);
					if(  !str.empty()  ) {
						keys.append(str);
					}
				}
				imagelist_writer_t::instance()->write_obj(outfp, node, keys);

				keys.clear();
				for (ribi = 3; ribi <= 12; ribi += 3) {
					char buf[40];

					sprintf(buf, "%sdiagonal[%s][%d]", image_type[backtofront], ribi_codes[ribi], season);
					string str = obj.get(buf);
					keys.append(str);
				}
				imagelist_writer_t::instance()->write_obj(outfp, node, keys);

				keys.clear();
				if(season == 0  &&  backtofront == 0) {
					slist_tpl<string> cursorkeys;

					cursorkeys.append(string(obj.get("cursor")));
					cursorkeys.append(string(obj.get("icon")));

					cursorskin_writer_t::instance()->write_obj(outfp, node, obj, cursorkeys);
				}
			}
		}
	}

	node.write(outfp);
}
