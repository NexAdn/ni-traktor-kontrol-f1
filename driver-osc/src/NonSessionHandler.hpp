#pragma once

#include <atomic>
#include <mutex>
#include <set>
#include <string>
#include <string_view>
#include <thread>

#include <lo/lo_cpp.h>

class NonSessionHandler
{
public:
	constexpr static int NON_API_VERSION_MAJOR{1};
	constexpr static int NON_API_VERSION_MINOR{2};
	constexpr static std::chrono::seconds SESSION_HANDSHAKE_TIMEOUT{5};

	constexpr static const char* NSM_ANNOUNCE{"/nsm/server/announce"};
	constexpr static const char* NSM_OPEN{"/nsm/client/open"};
	constexpr static const char* NSM_SAVE{"/nsm/client/save"};

	enum class State : int
	{
		NO_SESSION = 0,
		HANDSHAKE_START = 1,
		HANDSHAKE_AWAIT_REPLY = 2,
		RUNNING = 3,
		TASKING = 4,
		FAILED = 5
	};

	template <typename string_type>
	inline NonSessionHandler(const string_type& c2s_addr,
	                         std::string_view executable_name)
	    : s2c_thread(nullptr)
	    , c2s_addr(c2s_addr)
	    , executable_name(executable_name)
	{
		register_callbacks();
		s2c_thread.start();
	}
	NonSessionHandler(const NonSessionHandler&) = delete;
	inline ~NonSessionHandler() = default;

	operator lo::Address() const
	{
		return {s2c_thread.url()};
	}

	inline bool session_is_running() const noexcept
	{
		return session_state == State::RUNNING;
	}

	inline bool session_has_failed() const noexcept
	{
		return session_state == State::FAILED;
	}

	void start_session();
	State state() const noexcept;

	void broadcast_controller_state_change();

private:
	void step_state_machine();
	void register_callbacks();

	void send_announce();

	template <typename... T>
	void handle_reply(T... args);
	template <typename... T>
	void handle_error(T... args);

	void handle_open(std::string_view project_path, std::string_view display_name, std::string_view client_id);
	void handle_save();

	lo::ServerThread s2c_thread;
	const lo::Address c2s_addr;
	const std::string_view executable_name;

	std::atomic<State> session_state{State::NO_SESSION};
	std::mutex state_machine_mtx;

	bool handshake_complete{false};

	std::set<std::string> server_capabilities;
};
