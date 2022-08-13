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

#include "core/kernelMidi.h"
#include "core/const.h"
#include "utils/log.h"
#include <cassert>
#include <memory>

namespace giada::m
{
namespace
{
constexpr auto OUTPUT_NAME = "Giada MIDI output";
constexpr auto INPUT_NAME  = "Giada MIDI input";

/* -------------------------------------------------------------------------- */

std::vector<unsigned char> split_(uint32_t iValue)
{
	return {
	    static_cast<unsigned char>((iValue >> 24) & 0xFF),
	    static_cast<unsigned char>((iValue >> 16) & 0xFF),
	    static_cast<unsigned char>((iValue >> 8) & 0xFF)};
}

/* -------------------------------------------------------------------------- */

uint32_t join_(int b1, int b2, int b3)
{
	return (b1 << 24) | (b2 << 16) | (b3 << 8) | (0x00);
}
} // namespace

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

KernelMidi::KernelMidi()
: onMidiReceived(nullptr)
{
}

/* -------------------------------------------------------------------------- */

bool KernelMidi::openOutDevice(RtMidi::Api api, int port)
{
	if (port == -1)
		return false;

	u::log::print("[KM] Opening output - API=%d, port=%d, device='%s'\n", api, port, OUTPUT_NAME);

	m_midiOut = makeDevice<RtMidiOut>(api, OUTPUT_NAME);
	if (m_midiOut == nullptr)
		return false;

	return openPort(*m_midiOut, port);
}

/* -------------------------------------------------------------------------- */

bool KernelMidi::openInDevice(RtMidi::Api api, int port)
{
	if (port == -1)
		return false;

	u::log::print("[KM] Opening input - API=%d, port=%d, device '%s'\n", api, port, INPUT_NAME);

	m_midiIn = makeDevice<RtMidiIn>(api, INPUT_NAME);
	if (m_midiIn == nullptr)
		return false;

	if (!openPort(*m_midiIn, port))
		return false;

	m_midiIn->setCallback(&s_callback, this);
	m_midiIn->ignoreTypes(true, false, true); // Ignore all system/time msgs, for now

	return true;
}

/* -------------------------------------------------------------------------- */

void KernelMidi::logPorts()
{
	if (m_midiOut != nullptr)
		logPorts(*m_midiOut, OUTPUT_NAME);
	if (m_midiIn != nullptr)
		logPorts(*m_midiIn, INPUT_NAME);
}

/* -------------------------------------------------------------------------- */

bool KernelMidi::hasAPI(RtMidi::Api API) const
{
	std::vector<RtMidi::Api> APIs;
	RtMidi::getCompiledApi(APIs);
	for (unsigned i = 0; i < APIs.size(); i++)
		if (APIs.at(i) == API)
			return true;
	return false;
}

/* -------------------------------------------------------------------------- */

std::string KernelMidi::getOutPortName(unsigned p) const { return getPortName(*m_midiOut, p); }
std::string KernelMidi::getInPortName(unsigned p) const { return getPortName(*m_midiIn, p); }

/* -------------------------------------------------------------------------- */

void KernelMidi::send(uint32_t data)
{
	if (m_midiOut == nullptr)
		return;

	std::vector<unsigned char> msg = split_(data);

	m_midiOut->sendMessage(&msg);

	G_DEBUG("Send MIDI msg=0x{:0X} ({:0X} {:0X} {:0X})", data, msg[0], msg[1], msg[2]);
}

/* -------------------------------------------------------------------------- */

void KernelMidi::send(int b1, int b2, int b3)
{
	if (m_midiOut == nullptr)
		return;

	std::vector<unsigned char> msg(1, b1);

	if (b2 != -1)
		msg.push_back(b2);
	if (b3 != -1)
		msg.push_back(b3);

	m_midiOut->sendMessage(&msg);

	G_DEBUG("Send MIDI msg=({:0X} {:0X} {:0X})", b1, b2, b3);
}

/* -------------------------------------------------------------------------- */

unsigned KernelMidi::countOutPorts() const { return m_midiOut != nullptr ? m_midiOut->getPortCount() : 0; }
unsigned KernelMidi::countInPorts() const { return m_midiIn != nullptr ? m_midiIn->getPortCount() : 0; }

/* -------------------------------------------------------------------------- */

void KernelMidi::s_callback(double /*t*/, std::vector<unsigned char>* msg, void* data)
{
	static_cast<KernelMidi*>(data)->callback(msg);
}

/* -------------------------------------------------------------------------- */

void KernelMidi::callback(std::vector<unsigned char>* msg)
{
	assert(onMidiReceived != nullptr);

	if (msg->size() < 3)
	{
		G_DEBUG("Received unknown MIDI signal - bytes={}", msg->size());
		return;
	}

	onMidiReceived(join_(msg->at(0), msg->at(1), msg->at(2)));
}

/* -------------------------------------------------------------------------- */

template <typename Device>
std::unique_ptr<Device> KernelMidi::makeDevice(RtMidi::Api api, std::string name) const
{
	try
	{
		return std::make_unique<Device>(static_cast<RtMidi::Api>(api), name);
	}
	catch (RtMidiError& error)
	{
		u::log::print("[KM] Error opening device '%s': %s\n", name.c_str(), error.getMessage());
		return nullptr;
	}
}

template std::unique_ptr<RtMidiOut> KernelMidi::makeDevice(RtMidi::Api, std::string) const;
template std::unique_ptr<RtMidiIn>  KernelMidi::makeDevice(RtMidi::Api, std::string) const;

/* -------------------------------------------------------------------------- */

bool KernelMidi::openPort(RtMidi& device, int port)
{
	try
	{
		device.openPort(port, device.getPortName(port));
		return true;
	}
	catch (RtMidiError& error)
	{
		u::log::print("[KM] Error opening port %d: %s\n", port, error.getMessage());
		return false;
	}
}

/* -------------------------------------------------------------------------- */

std::string KernelMidi::getPortName(RtMidi& device, int port) const
{
	try
	{
		return device.getPortName(port);
	}
	catch (RtMidiError& /*error*/)
	{
		return "";
	}
}

/* -------------------------------------------------------------------------- */

void KernelMidi::logPorts(RtMidi& device, std::string name) const
{
	u::log::print("[KM] Device '%s': %d MIDI ports found\n", name.c_str(), device.getPortCount());
	for (unsigned i = 0; i < device.getPortCount(); i++)
		u::log::print("  %d) %s\n", i, device.getPortName(i));
}

/* -------------------------------------------------------------------------- */

void KernelMidi::logCompiledAPIs()
{
	std::vector<RtMidi::Api> apis;
	RtMidi::getCompiledApi(apis);

	u::log::print("[KM] Compiled RtMidi APIs: %d\n", apis.size());

	for (const RtMidi::Api& api : apis)
	{
		switch (api)
		{
		case RtMidi::Api::UNSPECIFIED:
			u::log::print("  UNSPECIFIED\n");
			break;
		case RtMidi::Api::MACOSX_CORE:
			u::log::print("  CoreAudio\n");
			break;
		case RtMidi::Api::LINUX_ALSA:
			u::log::print("  ALSA\n");
			break;
		case RtMidi::Api::UNIX_JACK:
			u::log::print("  JACK\n");
			break;
		case RtMidi::Api::WINDOWS_MM:
			u::log::print("  Microsoft Multimedia MIDI API\n");
			break;
		case RtMidi::Api::RTMIDI_DUMMY:
			u::log::print("  Dummy\n");
			break;
		case RtMidi::Api::WEB_MIDI_API:
			u::log::print("  Web MIDI API\n");
			break;
		default:
			u::log::print("  (unknown)\n");
			break;
		}
	}
}
} // namespace giada::m
