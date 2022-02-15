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

#ifndef G_V_UI_H
#define G_V_UI_H

#include "core/conf.h"
#include "core/patch.h"
#include "gui/dialogs/mainWindow.h"
#include "gui/dispatcher.h"
#include "gui/updater.h"
#include <memory>
#include <string>

namespace giada::m
{
class Recorder;
class Engine;
} // namespace giada::m

namespace giada::v
{
class Ui final
{
public:
	Ui(m::Recorder&, const m::Conf::Data&);

	/* shouldBlink
	Return whether is time to blink something or not. This is used to make 
	widgets blink. */

	bool shouldBlink() const;

	/* load
	Reads UI information from a Patch when a new project has been loaded. */

	void load(const m::Patch::Data&);

	/* store
	Writes UI information to a patch when a project needs to be saved. */

	void store(const std::string patchName, m::Patch::Data& patch);

	void init(int argc, char** argv, m::Engine&);
	void reset();
	void shutdown();

	/* refresh
	Repaints dynamic GUI elements. */

	void refresh();

	/* rebuild
	Rebuilds the UI from scratch. Used when the model has changed. */

	void rebuild();

	/* [rebuild|refresh]SubWindow 
	Rebuilds or refreshes subwindow with ID 'wid' if it exists, i.e. if it's open. */

	void rebuildSubWindow(int wid);
	void refreshSubWindow(int wid);

	/* getSubwindow
	Returns a pointer to an open subwindow, otherwise nullptr. */

	v::gdWindow* getSubwindow(v::gdWindow& parent, int wid);

	/* openSubWindow
	Opens a new sub-window as a child of parent and assigns 'wid' to child. */

	void openSubWindow(v::gdWindow& parent, v::gdWindow* child, int wid);

	/* closeSubWindow
	Closes a sun-window currently attached to the main one. */

	void closeSubWindow(int wid);

	/* closeAllSubwindows
	Closes all subwindows attached to the main one. */

	void closeAllSubwindows();

	/* setMainWindowTitle
    Updates the title of the main window, usually visible on top of it. */

	void setMainWindowTitle(const std::string&);

#ifdef WITH_VST

	/* [start|stop]JuceDispatchLoop
	Starts and stops the JUCE dispatch loop from its MessageManager component.
	This is needed for plugin-ins to wake up their UI editor and let it react
	to UI events. */

	void startJuceDispatchLoop();
	void stopJuceDispatchLoop();

#endif

	std::unique_ptr<gdMainWindow> mainWindow;
	Dispatcher                    dispatcher;

private:
	static constexpr int BLINK_RATE = G_GUI_FPS / 2;

#ifdef WITH_VST
	static void juceDispatchLoop(void*);
#endif

	/* rebuildStaticWidgets
    Updates attributes of static widgets, i.e. those elements that don't get
    automatically refreshed during the UI update loop. Useful when loading a new 
    patch. */

	void rebuildStaticWidgets();

	Updater m_updater;
	int     m_blinker;
};
} // namespace giada::v

#endif