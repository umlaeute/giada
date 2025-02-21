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

#ifndef GE_CHOICE_H
#define GE_CHOICE_H

#include "core/types.h"
#include <FL/Fl_Choice.H>
#include <functional>
#include <string>
#include <vector>

namespace giada::v
{
class geChoice : public Fl_Choice
{
public:
	geChoice(int x, int y, int w, int h, const char* l = 0, bool angle = true);
	void draw() override;

	ID getSelectedId() const;

	/* addItem
	Adds a new item with a certain ID. Pass id = -1 to auto-increment it. */

	void addItem(const std::string& label, ID id = -1);

	void showItem(const std::string& label);
	void showItem(ID id);
	void clear();

	std::function<void(ID)> onChange = nullptr;

private:
	static void cb_onChange(Fl_Widget* w, void* p);
	void        cb_onChange();

	bool            m_angle;
	std::vector<ID> m_ids;
};
} // namespace giada::v

#endif
