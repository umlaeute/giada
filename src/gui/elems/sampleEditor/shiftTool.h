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

#ifndef GE_SHIFT_TOOL_H
#define GE_SHIFT_TOOL_H

#include "core/types.h"
#include "gui/elems/basics/box.h"
#include "gui/elems/basics/button.h"
#include "gui/elems/basics/input.h"
#include "gui/elems/basics/pack.h"

namespace giada::c::sampleEditor
{
struct Data;
}
namespace giada::v
{
class geShiftTool : public gePack
{
public:
	geShiftTool(const c::sampleEditor::Data& d, int x, int y);

	void rebuild(const c::sampleEditor::Data& d);
	void update(Frame shift);

  private:
	static void cb_setShift(Fl_Widget* /*w*/, void* p);
	static void cb_reset(Fl_Widget* /*w*/, void* p);
	void        cb_setShift();
	void        cb_reset();

	void shift(int f);

	const c::sampleEditor::Data* m_data;

	geBox    m_label;
	geInput  m_shift;
	geButton m_reset;
};
} // namespace giada::v

#endif
