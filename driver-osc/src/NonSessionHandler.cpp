#include "NonSessionHandler.hpp"

#include "debug.hpp"

template <>
void NonSessionHandler::handle_reply(std::string_view path,
                                     std::string_view arg1,
                                     std::string_view arg2)
{
	if (path == "/nsm/server/announce") {
		debug("Received /nsm/server/announce\n");
		if (session_state == State::HANDSHAKE_AWAIT_REPLY) {
			handshake_complete = true;

			debug("handle_repley(): Step state machine\n");
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
	if (path == "/nsm/server/announce") {
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
	s2c_thread.add_method("/nsm/client/open", "sss", [this](lo::Message msg) {
		debug("Received /nsm/client/open\n");
		auto** argv = msg.argv();
		std::string_view project_path =
		  reinterpret_cast<const char*>(&argv[0]->s);
		std::string_view display_name =
		  reinterpret_cast<const char*>(&argv[1]->s);
		std::string_view client_id = reinterpret_cast<const char*>(&argv[2]->s);
		handle_open(project_path, display_name, client_id);
	});
	s2c_thread.add_method("/nsm/client/save", "sss", [this](lo::Message msg) {
		debug("Received /nsm/client/save\n");
		handle_save();
	});
}

void NonSessionHandler::send_announce()
{
	debug("send_announce(): Sending /nsm/client/announce\n");
	c2s_addr.send_from(s2c_thread, "/nsm/client/announce", "ssii",
	                   executable_name.data(), "FIXME", NON_API_VERSION_MAJOR,
	                   NON_API_VERSION_MINOR);
}

void NonSessionHandler::handle_open(std::string_view project_path,
                                    std::string_view display_name,
                                    std::string_view client_id)
{
	// We have nothing so save, so we have nothing to open
	debug("NonSessionHandler::handle_open(): Sending /reply\n");
	c2s_addr.send_from(s2c_thread, "/reply", "ss", "/nsm/client/open", "NOP");
}

void NonSessionHandler::handle_save()
{
	// We have nothing to save
	debug("NonSessionHandler::handle_save(): Sending /reply\n");
	c2s_addr.send_from(s2c_thread, "/reply", "ss", "/nsm/client/save", "NOP");
}
