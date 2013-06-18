/*
 * Copyright (c) 1997 - 2001 Hansj�rg Malthaner
 *
 * This file is part of the Simutrans project under the artistic licence.
 * (see licence.txt)
 */

/*
 * Window with destination information for a stop
 * @author Hj. Malthaner
 */

#ifndef gui_halt_info_h
#define gui_halt_info_h

#include "gui_frame.h"
#include "components/gui_label.h"
#include "components/gui_scrollpane.h"
#include "components/gui_textarea.h"
#include "components/gui_textinput.h"
#include "components/gui_button.h"
#include "components/gui_location_view_t.h"
#include "components/action_listener.h"
#include "components/gui_chart.h"

#include "../utils/cbuffer_t.h"
#include "../simhalt.h"
#include "../simwin.h"


class halt_info_t : public gui_frame_t, private action_listener_t
{
private:
	static karte_t *welt;

	/**
	* Buffer for freight info text string.
	* @author Hj. Malthaner
	*/
	cbuffer_t freight_info;
	cbuffer_t info_buf;

	gui_scrollpane_t scrolly;
	gui_textarea_t text;
	gui_textinput_t input;
	gui_chart_t chart;
	gui_label_t sort_label;
	location_view_t view;
	button_t button;
	button_t sort_button;     // @author hsiegeln
	button_t filterButtons[MAX_HALT_COST];
	button_t toggler;

	halthandle_t halt;
	char edit_name[256];

	void show_hide_statistics( bool show );

	char modified_name[320];

public:
	enum sort_mode_t { by_destination = 0, by_via = 1, by_amount_via = 2, by_amount = 3, by_origin = 4, by_origin_sum = 5, SORT_MODES = 6 };

	halt_info_t(karte_t *welt, halthandle_t halt);

	virtual ~halt_info_t();

	/**
	 * Set the window associated helptext
	 * @return the filename for the helptext, or NULL
	 * @author Hj. Malthaner
	 */
	const char * get_hilfe_datei() const {return "station.txt";}

	/**
	 * Draw new component. The values to be passed refer to the window
	 * i.e. It's the screen coordinates of the window where the
	 * component is displayed.
	 * @author Hj. Malthaner
	 */
	void zeichnen(koord pos, koord gr);

	/**
	 * Set window size and adjust component sizes and/or positions accordingly
	 * @author Hj. Malthaner
	 */
	virtual void set_fenstergroesse(koord groesse);

	virtual koord3d get_weltpos(bool);

	virtual bool is_weltpos();

	bool action_triggered(gui_action_creator_t*, value_t) OVERRIDE;

	void map_rotate90( sint16 );

	// this constructor is only used during loading
	halt_info_t(karte_t *welt);

	void rdwr( loadsave_t *file );

	uint32 get_rdwr_id() { return magic_halt_info; }
};

#endif
