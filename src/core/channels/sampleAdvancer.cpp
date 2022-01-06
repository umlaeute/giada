/* -----------------------------------------------------------------------------
 *
 * Giada - Your Hardcore Loopmachine
 *
 * -----------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2020 Giovanni A. Zuliani | Monocasual

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

#include "core/channels/sampleAdvancer.h"
#include "core/channels/channel.h"
#include <cassert>

namespace giada::m
{
void SampleAdvancer::onLastFrame(const Channel& ch, bool seqIsRunning) const
{
	const SamplePlayerMode mode   = ch.samplePlayer->mode;
	const bool             isLoop = ch.samplePlayer->isAnyLoopMode();

	switch (ch.shared->playStatus.load())
	{
	case ChannelStatus::PLAY:
		/* Stop LOOP_* when the sequencer is off, or SINGLE_* except for
		SINGLE_ENDLESS, which runs forever unless it's in ENDING mode. 
		Other loop once modes are put in wait mode. */
		if ((mode == SamplePlayerMode::SINGLE_BASIC ||
		        mode == SamplePlayerMode::SINGLE_BASIC_PAUSE ||
		        mode == SamplePlayerMode::SINGLE_PRESS ||
		        mode == SamplePlayerMode::SINGLE_RETRIG) ||
		    (isLoop && !seqIsRunning))
			stop(ch, 0);
		else if (mode == SamplePlayerMode::LOOP_ONCE || mode == SamplePlayerMode::LOOP_ONCE_BAR)
			wait(ch);
		break;

	case ChannelStatus::ENDING:
		stop(ch, 0);
		break;

	default:
		break;
	}
}

/* -------------------------------------------------------------------------- */

void SampleAdvancer::advance(const Channel& ch, const Sequencer::Event& e) const
{
	switch (e.type)
	{
	case Sequencer::EventType::FIRST_BEAT:
		onFirstBeat(ch, e.delta);
		break;

	case Sequencer::EventType::BAR:
		onBar(ch, e.delta);
		break;

	case Sequencer::EventType::REWIND:
		rewind(ch, e.delta);
		break;

	case Sequencer::EventType::ACTIONS:
		if (ch.shared->readActions.load() == true)
			parseActions(ch, *e.actions, e.delta);
		break;

	default:
		break;
	}
}

/* -------------------------------------------------------------------------- */

void SampleAdvancer::rewind(const Channel& ch, Frame localFrame) const
{
	ch.shared->rewinding = true;
	ch.shared->offset    = localFrame;
}

/* -------------------------------------------------------------------------- */

void SampleAdvancer::stop(const Channel& ch, Frame localFrame) const
{
	ch.shared->playStatus.store(ChannelStatus::OFF);
	ch.shared->tracker.store(ch.samplePlayer->begin);
	ch.shared->generated = 0; // TODO - samplePlayer should be responsible for this

	/*  Clear data in range [localFrame, (buffer.size)) if the event occurs in
    the middle of the buffer. TODO - samplePlayer should be responsible for this*/

	if (localFrame != 0)
		ch.shared->audioBuffer.clear(localFrame);
}

/* -------------------------------------------------------------------------- */

void SampleAdvancer::play(const Channel& ch, Frame localFrame) const
{
	ch.shared->playStatus.store(ChannelStatus::PLAY);
	ch.shared->offset = localFrame;
}

/* -------------------------------------------------------------------------- */

void SampleAdvancer::wait(const Channel& ch) const
{
	ch.shared->playStatus.store(ChannelStatus::WAIT);
	ch.shared->tracker.store(ch.samplePlayer->begin);
}

/* -------------------------------------------------------------------------- */

void SampleAdvancer::onFirstBeat(const Channel& ch, Frame localFrame) const
{
	G_DEBUG("onFirstBeat ch=" << ch.id << ", localFrame=" << localFrame);

	const ChannelStatus playStatus = ch.shared->playStatus.load();
	const ChannelStatus recStatus  = ch.shared->recStatus.load();
	const bool          isLoop     = ch.samplePlayer->isAnyLoopMode();

	switch (playStatus)
	{
	case ChannelStatus::PLAY:
		if (isLoop)
			rewind(ch, localFrame);
		break;

	case ChannelStatus::WAIT:
		play(ch, localFrame);
		break;

	case ChannelStatus::ENDING:
		if (isLoop)
			stop(ch, localFrame);
		break;

	default:
		break;
	}

	switch (recStatus)
	{
	case ChannelStatus::WAIT:
		ch.shared->recStatus.store(ChannelStatus::PLAY);
		ch.shared->readActions.store(true);
		break;

	case ChannelStatus::ENDING:
		ch.shared->recStatus.store(ChannelStatus::OFF);
		ch.shared->readActions.store(false);
		break;

	default:
		break;
	}
}

/* -------------------------------------------------------------------------- */

void SampleAdvancer::onBar(const Channel& ch, Frame localFrame) const
{
	G_DEBUG("onBar ch=" << ch.id << ", localFrame=" << localFrame);

	const ChannelStatus    playStatus = ch.shared->playStatus.load();
	const SamplePlayerMode mode       = ch.samplePlayer->mode;

	if (playStatus == ChannelStatus::PLAY && (mode == SamplePlayerMode::LOOP_REPEAT ||
	                                             mode == SamplePlayerMode::LOOP_ONCE_BAR))
		rewind(ch, localFrame);
	else if (playStatus == ChannelStatus::WAIT && mode == SamplePlayerMode::LOOP_ONCE_BAR)
		play(ch, localFrame);
}

/* -------------------------------------------------------------------------- */

void SampleAdvancer::onNoteOn(const Channel& ch, Frame localFrame) const
{
	switch (ch.shared->playStatus.load())
	{
	case ChannelStatus::OFF:
		play(ch, localFrame);
		break;

	case ChannelStatus::PLAY:
		if (ch.samplePlayer->mode == SamplePlayerMode::SINGLE_RETRIG)
			rewind(ch, localFrame);
		else
			stop(ch, localFrame);
		break;

	default:
		break;
	}
}

/* -------------------------------------------------------------------------- */

void SampleAdvancer::parseActions(const Channel& ch,
    const std::vector<Action>& as, Frame localFrame) const
{
	if (ch.samplePlayer->isAnyLoopMode() || !ch.isReadingActions())
		return;

	for (const Action& a : as)
	{
		if (a.channelId != ch.id)
			continue;

		switch (a.event.getStatus())
		{
		case MidiEvent::NOTE_ON:
			onNoteOn(ch, localFrame);
			break;

		case MidiEvent::NOTE_OFF:
		case MidiEvent::NOTE_KILL:
			stop(ch, localFrame);
			break;

		default:
			break;
		}
	}
}
} // namespace giada::m