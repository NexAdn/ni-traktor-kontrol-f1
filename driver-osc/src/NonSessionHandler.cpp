#include <unistd.h>

#include "NonSessionHandler.hpp"

#include "debug.hpp"

template <>
void NonSessionHandler::handle_reply(std::string_view path,
                                     std::string_view arg1,
                                     std::string_view arg2)
{
	if (path == NSM_ANNOUNCE) {
		debug("Received announce reply\n");
		if (session_state == State::HANDSHAKE_AWAIT_REPLY) {
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
		debug("State transition: RUNNING\n");
		session_state = State::RUNNING;
	} else {
		debug("State transition: HANDSHAKE_START\n");
		session_state = State::HANDSHAKE_START;
	}

	debug("start_session(): Step state machine\n");
	step_state_machine();
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
			session_state = State::RUNNING;
			rerun_state_machine_immediately = true;
		}
		break;
	}

	case State::RUNNING: {
		debug("State: RUNNING\n");
		// TODO: Check for new task
		break;
	}

	case State::TASKING: {
		debug("State: TASKING\n");
		// TODO
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
	s2c_thread.set_callbacks([]() { debug("lo::ServerThread initialized\n"); },
	                         []() {});
#endif

	s2c_thread.add_method("/reply", "sss", [this](lo::Message msg) {
		debug("Received /reply\n");
		auto** argv = msg.argv();
		std::string_view path = reinterpret_cast<const char*>(&argv[0]->s);
		std::string_view arg1 = reinterpret_cast<const char*>(&argv[1]->s);
		std::string_view arg2 = reinterpret_cast<const char*>(&argv[2]->s);
		handle_reply(path, arg1, arg2);
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
}

void NonSessionHandler::send_announce()
{
	debug(std::string{"send_announce(): Sending "} + NSM_ANNOUNCE + '\n');
	c2s_addr.send_from(s2c_thread, NSM_ANNOUNCE, "sssiii",
	                   "application-name-FIXME", "", executable_name.data(),
	                   NON_API_VERSION_MAJOR, NON_API_VERSION_MINOR,
	                   ::getpid());
}

void NonSessionHandler::handle_open(std::string_view project_path,
                                    std::string_view display_name,
                                    std::string_view client_id)
{
	// We have nothing so save, so we have nothing to open
	debug("NonSessionHandler::handle_open(): Sending /reply\n");
	c2s_addr.send_from(s2c_thread, "/reply", "ss", NSM_OPEN, "NOP");
}

void NonSessionHandler::handle_save()
{
	// We have nothing to save
	debug("NonSessionHandler::handle_save(): Sending /reply\n");
	c2s_addr.send_from(s2c_thread, "/reply", "ss", NSM_SAVE, "NOP");
}
