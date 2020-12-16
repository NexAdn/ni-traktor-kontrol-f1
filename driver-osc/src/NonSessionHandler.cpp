#include <cassert>

#include <unistd.h>

#include "F1InputState.hpp"
#include "NonSessionHandler.hpp"

#define DBG_MODULE_NAME "NSH"

#include "config.h"
#include "debug.hpp"

#ifdef DEBUG
NonSessionHandler::~NonSessionHandler()
{
	debug("Destroying NSH\n");
	s2c_thread.stop();
}
#endif

template <>
void NonSessionHandler::handle_reply(std::string_view path)
{
	if (path == OSC_SIGNAL_LIST) {
		debug("Received signal list end\n");
		if (session_state != State::PEER_AWAIT_SIGNAL_LIST) {
			debug("Received out of time signal list end. Ignoring it\n");
			return;
		}

		assert(current_handshaking_peer != peers.end());
		debug(std::string{"Completed signal exchange with peer "}
		      + current_handshaking_peer->second.address().url() + '\n');
		debug("State transition: RUNNING\n");
		session_state = State::RUNNING;
		handshaked_peers++;
	}
}

template <>
void NonSessionHandler::handle_reply(std::string_view path,
                                     std::string_view arg1,
                                     std::string_view arg2,
                                     std::string_view arg3)
{
	if (path == NSM_ANNOUNCE) {
		debug("Received announce reply\n");
		if (session_state == State::HANDSHAKE_AWAIT_REPLY) {
			assert_server_announce_matches_required_capabilities(arg3);

			handshake_complete = true;

			debug("handle_reply(): Step state machine\n");
			step_state_machine();
		} else {
			debug("State transition: FAILED\n");
			session_state = State::FAILED;

			debug("handle_reply(): Step state machine\n");
			step_state_machine();
		}
	}
}

template <>
void NonSessionHandler::handle_error(std::string_view path, int32_t error_code,
                                     std::string_view message)
{
	if (path == NSM_ANNOUNCE) {
		debug(std::string{"Received NSM session failure with error "}
		      + std::to_string(error_code) + " and message " + message.data());
		handshake_complete = false;

		debug("State transition: FAILED\n");
		session_state = State::FAILED;

		debug("handle_error(): Step state machine\n");
		step_state_machine();
	}
}

void NonSessionHandler::start_session()
{
	if (handshake_complete) {
		debug("State transition: HELLO_START\n");
		session_state = State::HELLO_START;
	} else {
		debug("State transition: HANDSHAKE_START\n");
		session_state = State::HANDSHAKE_START;
	}

	debug("start_session(): Step state machine\n");
	step_state_machine();
}

void NonSessionHandler::broadcast_input_event(const F1InputChange& changes)
{
	debug("Sending input event\n");
	lo::Bundle bundle;
	std::vector<lo::Message> messages;

	for (const auto& btn : changes.pressed_buttons.matrix) {
		auto& msg = messages.emplace_back();
		std::string path = std::string{OSC_SIG_MTX} + std::to_string(btn);
		msg.add_float(OSC_BTN_PRESSED);
		assert(msg.is_valid());
		bundle.add(path, msg);
	}

	for (const auto& btn : changes.released_buttons.matrix) {
		auto& msg = messages.emplace_back();
		std::string path = std::string{OSC_SIG_MTX} + std::to_string(btn);
		msg.add_float(OSC_BTN_RELEASED);
		assert(msg.is_valid());
		bundle.add(path, msg);
	}

	for (const auto& btn : changes.pressed_buttons.stop) {
		auto& msg = messages.emplace_back();
		std::string path = std::string{OSC_SIG_STOP} + std::to_string(btn);
		msg.add_float(OSC_BTN_PRESSED);
		assert(msg.is_valid());
		bundle.add(path, msg);
	}

	for (const auto& btn : changes.released_buttons.stop) {
		auto& msg = messages.emplace_back();
		std::string path = std::string{OSC_SIG_STOP} + std::to_string(btn);
		msg.add_float(OSC_BTN_RELEASED);
		assert(msg.is_valid());
		bundle.add(path, msg);
	}

	for (const auto& btn : changes.pressed_buttons.special) {
		auto& msg = messages.emplace_back();
		std::string path = std::string{OSC_SIG_SPECIAL} + std::to_string(btn);
		msg.add_float(OSC_BTN_PRESSED);
		assert(msg.is_valid());
		bundle.add(path, msg);
	}

	for (const auto& btn : changes.released_buttons.special) {
		auto& msg = messages.emplace_back();
		std::string path = std::string{OSC_SIG_SPECIAL} + std::to_string(btn);
		msg.add_float(OSC_BTN_RELEASED);
		assert(msg.is_valid());
		bundle.add(path, msg);
	}

	for (const auto& knob : changes.knobs) {
		auto& msg = messages.emplace_back();
		std::string path =
		  std::string{OSC_SIG_KNOB} + std::to_string(knob.first);
		msg.add_float(knob.second / F1Default::KNOB_MAX);
		assert(msg.is_valid());
		bundle.add(path, msg);
	}

	for (const auto& fader : changes.faders) {
		auto& msg = messages.emplace_back();
		std::string path =
		  std::string{OSC_SIG_FADER} + std::to_string(fader.first);
		msg.add_float(fader.second / F1Default::FADER_MAX);
		assert(msg.is_valid());
		bundle.add(path, msg);
	}

	assert(bundle.is_valid());

	for (auto& peer : peers) {
		peer.second.send(bundle);
	}
}

void NonSessionHandler::step_state_machine()
{
	std::unique_lock<std::mutex> lock(state_machine_mtx);
	State current_session_state = session_state;

	bool rerun_state_machine_immediately{false};

	switch (current_session_state) {
	case State::FAILED:
		debug("State: FAILED\n");
		break;
	case State::NO_SESSION: {
		debug("State: NO_SESSION\n");
		break;
	}

	case State::HANDSHAKE_START: {
		debug("State: HANDSHAKE_START\n");

		session_state = State::HANDSHAKE_AWAIT_REPLY;
		send_announce();
		rerun_state_machine_immediately = true;
		break;
	}

	case State::HANDSHAKE_AWAIT_REPLY: {
		debug("State: HANDSHAKE_AWAIT_REPLY\n");

		if (handshake_complete) {
			if (session_loaded) {
				session_state = State::SESSION_AWAIT_OPEN;
			} else {
				session_state = State::SESSION_AWAIT_LOADED;
			}
			rerun_state_machine_immediately = true;
		}
		break;
	}

	case State::SESSION_AWAIT_OPEN: {
		debug("State: SESSION_AWAIT_OPEN\n");

		if (!project_opened)
			break;

		if (session_loaded) {
			session_state = State::HELLO_START;
		} else {
			session_state = State::SESSION_AWAIT_LOADED;
		}
		rerun_state_machine_immediately = true;
		break;
	}

	case State::SESSION_AWAIT_LOADED: {
		debug("State: SESSION_AWAIT_LOADED\n");

		if (!session_loaded)
			break;

		if (project_opened) {
			session_state = State::HELLO_START;
		} else {
			session_state = State::SESSION_AWAIT_LOADED;
		}
		rerun_state_machine_immediately = true;
		break;
	}

	case State::HELLO_START: {
		debug("State: HELLO_START\n");

		send_hello();

		debug("State transition: RUNNING\n");
		session_state = State::RUNNING;
		rerun_state_machine_immediately = true;
		break;
	}

	case State::PEER_AWAIT_SIGNAL_LIST: {
		debug("State: PEER_AWAIT_SIGNAL_LIST\n");
		send_signal_list();
		break;
	}

	case State::RUNNING: {
		debug("State: RUNNING\n");
		if (handshaked_peers < peers.size()) {
			if (current_handshaking_peer == peers.end()) {
				current_handshaking_peer = peers.begin();
			} else {
				++current_handshaking_peer;
			}

			debug("State transition: PEER_AWAIT_SIGNAL_LIST\n");
			session_state = State::PEER_AWAIT_SIGNAL_LIST;

			rerun_state_machine_immediately = true;
		}
		break;
	}

	default:
		debug("Invalid state!\n");
		throw std::runtime_error(
		  "NonSessionHandler::step_state_machine(): State not implemented!");
	}

	if (rerun_state_machine_immediately) {
		lock.unlock();
		debug("step_state_machine(): Rerun\n");
		step_state_machine();
	}
}

void NonSessionHandler::register_callbacks()
{
#ifdef DEBUG
	s2c_thread.set_callbacks(
	  []() { debug("lo::ServerThread s2c initialized\n"); }, []() {});
#endif

	s2c_thread.add_method("/reply", "s", [this](lo::Message msg) {
		debug("Received /reply\n");
		auto* argv = msg.argv();
		std::string_view path = reinterpret_cast<const char*>(&argv[0]->s);
		handle_reply(path);
	});
	s2c_thread.add_method("/reply", "ssss", [this](lo::Message msg) {
		debug("Received /reply\n");
		auto** argv = msg.argv();
		std::string_view path = reinterpret_cast<const char*>(&argv[0]->s);
		std::string_view arg1 = reinterpret_cast<const char*>(&argv[1]->s);
		std::string_view arg2 = reinterpret_cast<const char*>(&argv[2]->s);
		std::string_view arg3 = reinterpret_cast<const char*>(&argv[3]->s);
		handle_reply(path, arg1, arg2, arg3);
	});
	s2c_thread.add_method("/reply", "ssfff", [this](lo::Message msg) {
		debug("Received /reply\n");
		auto** argv = msg.argv();
		std::string_view path = reinterpret_cast<const char*>(&argv[0]->s);
		std::string_view arg1 = reinterpret_cast<const char*>(&argv[1]->s);
		float arg2 = argv[2]->f;
		float arg3 = argv[3]->f;
		float arg4 = argv[4]->f;
		if (path == OSC_SIGNAL_LIST) {
			handle_signal_list_reply(arg1, arg2, arg3, arg4);
		} else {
			debug("Not implemented / Invalid signal\n");
			assert(false);
		}
	});
	s2c_thread.add_method("/error", "sis", [this](lo::Message msg) {
		debug("Received /error\n");
		auto** argv = msg.argv();
		std::string_view path = reinterpret_cast<const char*>(&argv[0]->s);
		int32_t error_code = argv[1]->i;
		std::string_view message = reinterpret_cast<const char*>(&argv[2]->s);
		handle_error(path, error_code, message);
	});
	s2c_thread.add_method(NSM_OPEN, "sss", [this](lo::Message msg) {
		debug(std::string{"Received "} + NSM_OPEN + '\n');
		auto** argv = msg.argv();
		std::string_view project_path =
		  reinterpret_cast<const char*>(&argv[0]->s);
		std::string_view display_name =
		  reinterpret_cast<const char*>(&argv[1]->s);
		std::string_view client_id = reinterpret_cast<const char*>(&argv[2]->s);
		handle_open(project_path, display_name, client_id);
	});
	s2c_thread.add_method(NSM_SAVE, "", [this](lo::Message msg) {
		debug(std::string{"Received "} + NSM_SAVE + '\n');
		handle_save();
	});
	s2c_thread.add_method(NSM_SESSION_LOADED, "", [this](lo::Message msg) {
		debug(std::string{"Received "} + NSM_SESSION_LOADED + '\n');
		session_loaded = true;
		step_state_machine();
	});
	s2c_thread.add_method(OSC_SIGNAL_LIST, "", [this](lo::Message msg) {
		debug(std::string{"Received "} + OSC_SIGNAL_LIST + '\n');
		handle_signal_list(msg.source());
	});
	s2c_thread.add_method(NSM_HELLO, "ssss", [this](lo::Message msg) {
		debug(std::string{"Received hello\n"});
		std::string_view peer_url =
		  reinterpret_cast<const char*>(&msg.argv()[0]->s);
		std::string_view peer_name =
		  reinterpret_cast<const char*>(&msg.argv()[1]->s);
		std::string_view peer_version =
		  reinterpret_cast<const char*>(&msg.argv()[2]->s);
		std::string peer_client_id =
		  reinterpret_cast<const char*>(&msg.argv()[3]->s);

		if (peer_is_known(peer_client_id)) {
			// TODO Update URL
			debug("Feature not implemented\n");
			assert(false);
		} else {
			peers.emplace(peer_client_id,
			              NonPeer(peer_client_id, peer_name, peer_version,
			                      peer_url, s2c_thread));
		}
		step_state_machine();
	});
}

void NonSessionHandler::send_announce()
{
	debug(std::string{"send_announce(): Sending "} + NSM_ANNOUNCE + '\n');
	c2s_addr.send_from(s2c_thread, NSM_ANNOUNCE, "sssiii", NSM_CLIENT_NAME,
	                   NSM_CLIENT_CAPABILITIES, executable_name.data(),
	                   NON_API_VERSION_MAJOR, NON_API_VERSION_MINOR,
	                   ::getpid());
}

void NonSessionHandler::send_hello()
{
	debug("send_hello(): Sending hello\n");
	c2s_addr.send_from(s2c_thread, NSM_BROADCAST, "sssss", NSM_HELLO,
	                   static_cast<lo::Address>(*this).url().c_str(),
	                   NSM_CLIENT_NAME, KONTROLF1_VERSION, client_id.c_str());
}

void NonSessionHandler::send_signal_list()
{
	current_handshaking_peer->second.fetch_signal_list();
}

void NonSessionHandler::handle_open(std::string_view project_path,
                                    std::string_view display_name,
                                    std::string_view client_id)
{
	// We have nothing so save, yet, so we have nothing to open
	this->client_id = client_id;

	debug("NonSessionHandler::handle_open(): Sending /reply\n");
	c2s_addr.send_from(s2c_thread, "/reply", "ss", NSM_OPEN, "NOP");

	project_opened = true;
	step_state_machine();
}

void NonSessionHandler::handle_save()
{
	// We have nothing to save, yet
	debug("NonSessionHandler::handle_save(): Sending /reply\n");
	c2s_addr.send_from(s2c_thread, "/reply", "ss", NSM_SAVE, "NOP");
}

void NonSessionHandler::handle_signal_list(const lo::Address& peer)
{
	assert(peer.is_valid());
	assert(s2c_thread.is_valid());

	lo::Bundle bundle;
	std::vector<lo::Message> msgs(signals.size());

	for (auto& signal : signals) {
		lo::Message& msg = msgs.emplace_back();
		msg.add_string(OSC_SIGNAL_LIST);
		msg.add_string(signal.path);
		msg.add_string(signal.direction_str());
		msg.add_float(signal.min);
		msg.add_float(signal.max);
		msg.add_float(signal.default_value);
		assert(msg.is_valid());
		bundle.add("/reply", msg);
	}

	lo::Message& finish_msg = msgs.emplace_back();
	finish_msg.add_string(OSC_SIGNAL_LIST);
	assert(finish_msg.is_valid());
	bundle.add("/reply", finish_msg);

	assert(bundle.is_valid());

	debug(std::string{"Sending signal bundle to "} + peer.url() + '\n');
#ifdef DEBUG
	bundle.print();
#endif

	peer.send_from(s2c_thread, bundle);
}

void NonSessionHandler::handle_signal_list_reply(std::string_view signal_path,
                                                 float min, float max,
                                                 float default_value)
{
	assert(current_handshaking_peer != peers.end());
	auto& peer = current_handshaking_peer->second;
	// FIXME Read direction from reply
	peer.register_signal(signal_path, min, max, default_value, NonSignal::Direction::IN);
}

void NonSessionHandler::assert_server_announce_matches_required_capabilities(
  std::string_view capabilities)
{
	// We assume no capability is prefix of any other capability
	for (const auto& required_cap :
	     NonSessionHandler::NSM_REQUIRED_SERVER_CAPABILITIES) {
		size_t found_pos = capabilities.find(required_cap);
		if (found_pos == std::string_view::npos) {
			debug(
			  std::string{"NonSessionHandler::assert_server_announce_matches_"
			              "required_capabilities(): Missing capability "}
			  + required_cap + '\n');
			debug("State transition: FAILED\n");
			session_state = State::FAILED;
		}
	}
}

bool NonSessionHandler::peer_is_known(const std::string& client_id) const
{
	return peers.find(client_id) != peers.end();
}
