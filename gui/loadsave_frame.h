/*
 * Copyright (c) 1997 - 2001 Hansj�rg Malthaner
 *
 * This file is part of the Simutrans project under the artistic licence.
 * (see licence.txt)
 */

#ifndef gui_loadsave_frame_h
#define gui_loadsave_frame_h


#include "savegame_frame.h"
#include "../tpl/stringhashtable_tpl.h"
#include <string>

class karte_t;
class loadsave_t;

class gui_file_table_pak_column_t : public gui_file_table_label_column_t
{
	char pak[1024];
protected:
	virtual const char *get_text(const gui_table_row_t &row) const;
public:
	gui_file_table_pak_column_t();
	virtual int compare_rows(const gui_table_row_t &row1, const gui_table_row_t &row2) const;
	virtual void paint_cell(const koord &offset, coordinate_t x, coordinate_t y, const gui_table_row_t &row);
};

class gui_file_table_int_column_t : public gui_file_table_label_column_t
{
protected:
	virtual sint32 get_int(const gui_table_row_t &row) const = 0;
public:
	gui_file_table_int_column_t(coordinate_t size_) : gui_file_table_label_column_t(size_) {}
	virtual int compare_rows(const gui_table_row_t &row1, const gui_table_row_t &row2) const { return get_int(row1) - get_int(row2); }
};

class gui_file_table_std_column_t : public gui_file_table_int_column_t
{
protected:
	virtual sint32 get_int(const gui_table_row_t &row) const;
public:
	gui_file_table_std_column_t() : gui_file_table_int_column_t(65) {}
	virtual void paint_cell(const koord &offset, coordinate_t x, coordinate_t y, const gui_table_row_t &row);
};

class gui_file_table_exp_column_t : public gui_file_table_int_column_t
{
protected:
	virtual sint32 get_int(const gui_table_row_t &row) const;
public:
	gui_file_table_exp_column_t() : gui_file_table_int_column_t(25) {}
	virtual void paint_cell(const koord &offset, coordinate_t x, coordinate_t y, const gui_table_row_t &row);
};

class sve_info_t {
public:
	std::string pak;
	sint64 mod_time;
	sint32 file_size;
	bool file_exists;
	sve_info_t() : pak(""), mod_time(0), file_size(0), file_exists(false) {}
	sve_info_t(const char *pak_, time_t mod_, long fs);
	bool operator== (const sve_info_t &) const;
	void rdwr(loadsave_t *file);
};

class loadsave_frame_t : public savegame_frame_t
{
private:
	karte_t *welt;
	gui_file_table_delete_column_t delete_column;
	gui_file_table_action_column_t action_column;
	gui_file_table_time_column_t date_column;
	gui_file_table_pak_column_t pak_column;
	gui_file_table_std_column_t std_column;
	gui_file_table_exp_column_t exp_column;
	bool do_load;

	static stringhashtable_tpl<sve_info_t *> cached_info;
protected:
	virtual void init(const char *suffix, const char *path);
	virtual void set_file_table_default_sort_order();

	/**
	 * Aktion, die nach Knopfdruck gestartet wird.
	 * @author Hansj�rg Malthaner
	 */
	virtual void action(const char *filename);

	// returns extra file info
	virtual const char *get_info(const char *fname);
	virtual void add_file(const char *fullpath, const char *filename, const bool not_cutting_suffix);

public:
	/**
	* Manche Fenster haben einen Hilfetext assoziiert.
	* @return den Dateinamen f�r die Hilfe, oder NULL
	* @author Hj. Malthaner
	*/
	virtual const char *get_hilfe_datei() const;

	loadsave_frame_t(karte_t *welt, bool do_load);

	/**
	 * save hashtable to xml file
	 */
	virtual ~loadsave_frame_t();
};

#endif
