/*
 * Copyright (c) 1997 - 2001 Hansj�rg Malthaner
 *
 * This file is part of the Simutrans project under the artistic licence.
 * (see licence.txt)
 */

#include <stdio.h>

#include "../simdebug.h"
#include "../simworld.h"
#include "../simobj.h"
#include "../boden/wege/schiene.h"
#include "../boden/grund.h"
#include "../display/simimg.h"
#include "../dataobj/ribi.h"
#include "../dataobj/loadsave.h"
#include "../dataobj/translator.h"
#include "../dataobj/environment.h"
#include "../utils/cbuffer_t.h"

#include "signal.h"


signal_t::signal_t(loadsave_t *file) :
	roadsign_t(file)
{
	if(desc==NULL) {
		desc = roadsign_t::default_signal;
	}
	state = rot;
}


/**
 * @return Einen Beschreibungsstring f�r das Objekt, der z.B. in einem
 * Beobachtungsfenster angezeigt wird.
 * @author Hj. Malthaner
 */
void signal_t::info(cbuffer_t & buf) const
{
	// well, needs to be done
	obj_t::info(buf);

	buf.printf("%s\n%s%u", translator::translate(desc->get_name()), translator::translate("\ndirection:"), get_dir());
}


void signal_t::calc_image()
{
	foreground_image = IMG_EMPTY;
	image_id image = IMG_EMPTY;
	image2 = IMG_EMPTY;
	foreground_image2 = IMG_EMPTY;

	after_xoffset = 0;
	after_yoffset = 0;
	sint8 xoff = 0, yoff = 0;
	const bool left_swap = welt->get_settings().is_signals_left()  &&  desc->get_offset_left();

	grund_t *gr = welt->lookup(get_pos());
	if(gr) {

		const slope_t::type full_hang = gr->get_weg_hang();
		const sint8 hang_diff = slope_t::max_diff(full_hang);
		const ribi_t::ribi hang_dir = ribi_t::backward( ribi_type(full_hang) );

		set_flag(obj_t::dirty);

		weg_t *sch = gr->get_weg(desc->get_wtyp()!=tram_wt ? desc->get_wtyp() : track_wt);

		if(  desc->does_use_diagonal()  ) {
			// a signal with diagonal and slope images is processed here.
			bool snow = (gr->ist_karten_boden()  ||  !gr->ist_tunnel())  &&  (get_pos().z + gr->get_weg_yoff()/TILE_HEIGHT_STEP >= welt->get_snowline() || welt->get_climate( get_pos().get_2d() ) == arctic_climate  );
			uint8 tmp_state = state;
			if(  sch->is_electrified()  &&   desc->has_electrified_images()  ) {
				tmp_state += desc->is_pre_signal() ? 3 : 2;
			}
			ribi_t::ribi temp_dir = dir;
			hide_ribi_at_tunnel_entrance(gr,temp_dir);
			if(  hang_diff==0  ) {
				// normal? diagonal?
				if(  sch  &&  sch->is_diagonal()  ) {
					// Use diagonal image.
					uint8 diagonal_ribi = (temp_dir<<4)|sch->get_ribi_unmasked();
					solve_image_id_then_set(image_diagonal, diagonal_ribi, snow, tmp_state);
				} else {
					// Use normal image.
					solve_image_id_then_set(image_flat, temp_dir, snow, tmp_state);
				}
			} else {
				// Use slope image.
				uint8 slope_ribi = (hang_dir<<4)|temp_dir;
				solve_image_id_then_set(hang_diff==2?image_slope2:image_slope, slope_ribi, snow, tmp_state);
			}
			return;
		}

		if(sch) {
			uint16 offset=0;
			ribi_t::ribi dir = sch->get_ribi_unmasked() & (~calc_mask());
			if(sch->is_electrified()  &&  (desc->get_count()/8)>1) {
				offset = desc->is_pre_signal() ? 12 : 8;
			}

			// vertical offset of the signal positions
			if(full_hang==slope_t::flat) {
				yoff = -gr->get_weg_yoff();
				after_yoffset = yoff;
			}
			else {
				const ribi_t::ribi test_hang = left_swap ? ribi_t::backward(hang_dir) : hang_dir;
				if(test_hang==ribi_t::east ||  test_hang==ribi_t::north) {
					yoff = -TILE_HEIGHT_STEP*hang_diff;
					after_yoffset = 0;
				}
				else {
					yoff = 0;
					after_yoffset = -TILE_HEIGHT_STEP*hang_diff;
				}
			}

			// and now calculate the images:
			// we need to hide the "second" image on tunnel entries
			ribi_t::ribi temp_dir = dir;
			hide_ribi_at_tunnel_entrance(gr,temp_dir);

			// signs for left side need other offsets and other front/back order
			if(  left_swap  ) {
				const sint16 XOFF = 2*desc->get_offset_left();
				const sint16 YOFF = desc->get_offset_left();

				if(temp_dir&ribi_t::east) {
					image = desc->get_image_id(3+state*4+offset);
					foreground_image = desc->get_image_id(3+state*4+offset,true);
					xoff += XOFF;
					yoff += -YOFF;
				}

				if(temp_dir&ribi_t::north) {
					if(image!=IMG_EMPTY) {
						image2 = desc->does_use_frontImage() ? desc->get_image_id(0+state*4+offset) : IMG_EMPTY;
						foreground_image2 = desc->does_use_frontImage() ? desc->get_image_id(0+state*4+offset,true) : desc->get_image_id(0+state*4+offset);
						after_xoffset += -XOFF;
						after_yoffset += -YOFF;
					}
					else {
						image = desc->get_image_id(0+state*4+offset);
						foreground_image = desc->get_image_id(0+state*4+offset,true);
						xoff += -XOFF;
						yoff += -YOFF;
					}
				}

				if(temp_dir&ribi_t::west) {
					image2 = desc->does_use_frontImage() ? desc->get_image_id(2+state*4+offset) : IMG_EMPTY;
					foreground_image2 = desc->does_use_frontImage() ? desc->get_image_id(2+state*4+offset,true) : desc->get_image_id(2+state*4+offset);
					after_xoffset += -XOFF;
					after_yoffset += YOFF;
				}

				if(temp_dir&ribi_t::south) {
					if(foreground_image2!=IMG_EMPTY) {
						image = desc->get_image_id(1+state*4+offset);
						foreground_image = desc->get_image_id(1+state*4+offset,true);
						xoff += XOFF;
						yoff += YOFF;
					}
					else {
						image2 = desc->does_use_frontImage() ? desc->get_image_id(1+state*4+offset) : IMG_EMPTY;
						foreground_image2 = desc->does_use_frontImage() ? desc->get_image_id(1+state*4+offset,true) : desc->get_image_id(1+state*4+offset);
						after_xoffset += XOFF;
						after_yoffset += YOFF;
					}
				}
			}
			else {
				if(temp_dir&ribi_t::east) {
					image2 = desc->does_use_frontImage() ? desc->get_image_id(3+state*4+offset) : IMG_EMPTY;
					foreground_image2 = desc->does_use_frontImage() ? desc->get_image_id(3+state*4+offset,true) : desc->get_image_id(3+state*4+offset);
				}

				if(temp_dir&ribi_t::north) {
					if(foreground_image2==IMG_EMPTY) {
						image2 = desc->does_use_frontImage() ? desc->get_image_id(0+state*4+offset) : IMG_EMPTY;
						foreground_image2 = desc->does_use_frontImage() ? desc->get_image_id(0+state*4+offset,true) : desc->get_image_id(0+state*4+offset);
					}
					else {
						image = desc->get_image_id(0+state*4+offset);
						foreground_image = desc->get_image_id(0+state*4+offset,true);
					}
				}

				if(temp_dir&ribi_t::west) {
					image = desc->get_image_id(2+state*4+offset);
					foreground_image = desc->get_image_id(2+state*4+offset,true);
				}

				if(temp_dir&ribi_t::south) {
					if(image==IMG_EMPTY) {
						image = desc->get_image_id(1+state*4+offset);
						foreground_image = desc->get_image_id(1+state*4+offset,true);
					}
					else {
						image2 = desc->does_use_frontImage() ? desc->get_image_id(1+state*4+offset) : IMG_EMPTY;
						foreground_image2 = desc->does_use_frontImage() ? desc->get_image_id(1+state*4+offset,true) : desc->get_image_id(1+state*4+offset);
					}
				}
			}
		}
	}
	set_xoff( xoff );
	set_yoff( yoff );
	set_image(image);
}
