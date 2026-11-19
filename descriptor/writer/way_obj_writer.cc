/*
 * This file is part of the Simutrans project under the Artistic License.
 * (see LICENSE.txt)
 */

#include <string>
#include "../../dataobj/tabfile.h"
#include "obj_node.h"
#include "obj_pak_exception.h"
#include "../way_obj_desc.h"
#include "text_writer.h"
#include "imagelist_writer.h"
#include "skin_writer.h"
#include "get_waytype.h"
#include "way_obj_writer.h"

using std::string;


/// image @p nr of the flat list, e.g. "frontimage[nse1]" (@p type is "front" or "back")
static string get_image(tabfileobj_t& obj, const char* type, uint8 nr)
{
	char buf[64];
	sprintf(buf, "%simage[%s]", type, switch_image::key(nr));
	return obj.get(buf);
}


/// image @p nr, or for a missing junction image the standard image of its tile shape
static string get_image_or_standard(tabfileobj_t& obj, const char* type, uint8 nr)
{
	string str = get_image(obj, type, nr);
	if(  str.empty()  &&  nr >= switch_image::FIRST_ROLE  ) {
		str = get_image(obj, type, switch_image::shape_ribi_of_role(nr));
	}
	return str;
}

/**
 * Write a way object (lamps, overheadwires)
 */
void way_obj_writer_t::write_obj(FILE* outfp, obj_node_t& parent, tabfileobj_t& obj)
{
	static const char* const ribi_codes[16] = {
		"-", "n",  "e",  "ne",  "s",  "ns",  "se",  "nse",
		"w", "nw", "ew", "new", "sw", "nsw", "sew", "nsew",
	};
	int ribi, slope;

	obj_node_t node(this, 20, &parent);


	// Version needs high bit set as trigger -> this is required
	// as marker because formerly nodes were unversionend
	uint16 version     = 0x8001;
	uint32 price       = obj.get_int("cost",        100);
	uint32 maintenance = obj.get_int("maintenance", 100);
	sint32 topspeed    = obj.get_int("topspeed",    999);

	uint16 intro  = obj.get_int("intro_year", DEFAULT_INTRO_DATE) * 12;
	intro +=        obj.get_int("intro_month", 1) - 1;

	uint16 retire  = obj.get_int("retire_year", DEFAULT_RETIRE_DATE) * 12;
	retire +=        obj.get_int("retire_month", 1) - 1;

	uint8 wtyp     =  get_waytype(obj.get("waytype"));
	uint8 own_wtyp =  get_waytype(obj.get("own_waytype"));

	node.write_uint16(outfp, version,      0);
	node.write_uint32(outfp, price,        2);
	node.write_uint32(outfp, maintenance,  6);
	node.write_sint32(outfp, topspeed,    10);
	node.write_uint16(outfp, intro,       14);
	node.write_uint16(outfp, retire,      16);
	node.write_uint8 (outfp, wtyp,        18);
	node.write_uint8 (outfp, own_wtyp,    19);

	write_head(outfp, node, obj);

	slist_tpl<string> front_list;
	slist_tpl<string> back_list;

	// standard images and junction images (OTRP, see switch_image.h): up to the highest one in
	// the dat file, at least the 26 images of standard Simutrans, and at least 36 if there are
	// extended junction images. A missing junction image is filled with the standard image of
	// the tile shape it belongs to.
	uint8 highest = 0;
	for(  uint8 nr = switch_image::FIRST_ROLE;  nr < switch_image::COUNT;  nr++  ) {
		if(  !get_image(obj, "front", nr).empty()  ||  !get_image(obj, "back", nr).empty()  ) {
			highest = nr;
		}
	}
	const int count = highest >= switch_image::Y_JUNCTION ? max(highest + 1, switch_image::CROSSOVER + 1) : (int)switch_image::Y_JUNCTION;
	for(  ribi = 0;  ribi < count;  ribi++  ) {
		front_list.append( get_image_or_standard(obj, "front", (uint8)ribi) );
		back_list.append( get_image_or_standard(obj, "back", (uint8)ribi) );
	}
	imagelist_writer_t::instance()->write_obj(outfp, node, front_list);
	imagelist_writer_t::instance()->write_obj(outfp, node, back_list);

	front_list.clear();
	back_list.clear();

	for(  slope = 3;  slope <= 12;  slope += 3  ) {
		char buf[40];
		sprintf( buf, "frontimageup[%d]", slope );
		string str = obj.get(buf);
		front_list.append(str);
		sprintf( buf, "backimageup[%d]", slope );
		string str2 = obj.get(buf);
		back_list.append(str2);
	}
	for(  slope = 3;  slope <= 12;  slope += 3  ) {
		char buf[40];
		sprintf( buf, "frontimageup2[%d]", slope );
		string str = obj.get(buf);
		if(  !str.empty()  ) {
			front_list.append(str);
		}
		sprintf( buf, "backimageup2[%d]", slope );
		string str2 = obj.get(buf);
		if(  !str2.empty()  ) {
			back_list.append(str2);
		}
	}
	imagelist_writer_t::instance()->write_obj(outfp, node, front_list);
	imagelist_writer_t::instance()->write_obj(outfp, node, back_list);

	front_list.clear();
	back_list.clear();

	for (ribi = 3; ribi <= 12; ribi += 3) {
		char buf[40];
		sprintf(buf, "frontdiagonal[%s]", ribi_codes[ribi]);
		string str = obj.get(buf);
		front_list.append(str);
		sprintf(buf, "backdiagonal[%s]", ribi_codes[ribi]);
		string str2 = obj.get(buf);
		back_list.append(str2);
	}
	imagelist_writer_t::instance()->write_obj(outfp, node, front_list);
	imagelist_writer_t::instance()->write_obj(outfp, node, back_list);

	slist_tpl<string> cursorkeys;
	cursorkeys.append(string(obj.get("cursor")));
	cursorkeys.append(string(obj.get("icon")));
	cursorskin_writer_t::instance()->write_obj(outfp, node, obj, cursorkeys);

	// node.write_data(fp, &desc);
	node.write(outfp);
}
