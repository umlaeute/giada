/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2022 Giovanni A. Zuliani | Monocasual Laboratories
 *
 * This file is part of Giada - Your Hardcore Loopmachine.
 *
 * Giada - Your Hardcore Loopmachine is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * Giada - Your Hardcore Loopmachine is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Giada - Your Hardcore Loopmachine. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------- */

#ifndef G_UTILS_GUI_H
#define G_UTILS_GUI_H

#include "core/types.h"
#include "deps/geompp/src/rect.hpp"
#include <FL/Fl_Menu_Item.H>
#include <string>

namespace giada::v
{
class gdWindow;
}

namespace giada::u::gui
{
/* ScopedLock
ScopedLock for locking the main FLTK thread when the UI must be updated from a
secondary thread. */

class ScopedLock
{
public:
	ScopedLock();
	~ScopedLock();
};

void setFavicon(v::gdWindow* w);

/* removeFltkChars
Strips special chars used by FLTK to split menus into sub-menus. */

std::string removeFltkChars(const std::string& s);

/* getStringRect
Returns the bounding box in pixels of a string 's'. */

geompp::Rect<int> getStringRect(const std::string& s);

/* getCenterWinBounds
Returns the bounding box to be used for a centered window. */

geompp::Rect<int> getCenterWinBounds(int w, int h);

/* truncate
Adds ellipsis to a string 's' if it longer than 'width' pixels. */

std::string truncate(const std::string& s, Pixel width);

int centerWindowX(int w);
int centerWindowY(int h);

/* makeMenuItem
Makes a new Fl_Menu_Item at compile time. Used to initialize pop-up menus. */

constexpr Fl_Menu_Item makeMenuItem(const char* text, Fl_Callback* callback = nullptr,
    void* data = nullptr, int flags = 0)
{
	return Fl_Menu_Item{text, 0, callback, data, flags, 0, 0, 0, 0};
}
} // namespace giada::u::gui

#endif
