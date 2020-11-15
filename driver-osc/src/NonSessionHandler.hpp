#pragma once

#include <atomic>
#include <cassert>
#include <map>
#include <mutex>
#include <string>
#include <string_view>

#include <iostream>

#include <lo/lo_cpp.h>

#include "NonPeer.hpp"

class F1InputChange;

class NonSessionHandler
{
public:
	constexpr static int NON_API_VERSION_MAJOR{1};
	constexpr static int NON_API_VERSION_MINOR{2};
	constexpr static std::chrono::seconds SESSION_HANDSHAKE_TIMEOUT{5};

	constexpr static const char* NSM_ANNOUNCE{"/nsm/server/announce"};
	constexpr static const char* NSM_SESSION_LOADED{
	  "/nsm/client/session_is_loaded"};
	constexpr static const char* NSM_BROADCAST{"/nsm/server/broadcast"};
	constexpr static const char* NSM_INCOMING_BROADCAST{
	  "/nsm/client/broadcast"};
	constexpr static const char* NSM_HELLO{"/nsm/hello"};
	constexpr static const char* NSM_OPEN{"/nsm/client/open"};
	constexpr static const char* NSM_SAVE{"/nsm/client/save"};
	constexpr static const char* OSC_SIGNAL_LIST{"/signal/list"};

	constexpr static const char* NSM_CLIENT_NAME{"traktor-kontrol-f1-mapper"};
	constexpr static const char* NSM_CLIENT_CAPABILITIES{"switch"};
	constexpr static std::array<const char*, 1>
	  NSM_REQUIRED_SERVER_CAPABILITIES{"broadcast"};

	constexpr static const char* NSM_CONTROL_PREFIX{"/kontrolf1"};
	constexpr static const char* NSM_BUTTON{"/button"};
	constexpr static const char* NSM_BUTTON_MATRIX{"/matrix"};
	constexpr static const char* NSM_BUTTON_SPECIAL{"/special"};
	constexpr static const char* NSM_BUTTON_STOP{"/stop"};

	enum class State : int
	{
		NO_SESSION,
		HANDSHAKE_START,
		HANDSHAKE_AWAIT_REPLY,
		SESSION_AWAIT_OPEN,
		SESSION_AWAIT_LOADED,
		HELLO_START,
		PEER_AWAIT_SIGNAL_LIST,
		RUNNING,
		FAILED
	};

	template <typename string_type>
	inline NonSessionHandler(const string_type& c2s_addr,
	                         std::string_view executable_name,
	                         const NonSignalList& signals = {})
	    : s2c_thread(nullptr)
	    , c2s_addr(c2s_addr)
	    , executable_name(executable_name)
	    , signals(signals)
	{
		assert(s2c_thread.is_valid());
		register_callbacks();
		s2c_thread.start();
	}
	NonSessionHandler(const NonSessionHandler&) = delete;
#ifndef DEBUG
	inline ~NonSessionHandler() = default;
#else
	~NonSessionHandler();
#endif

	operator lo::Address() const
	{
		return {s2c_thread.url()};
	}

	inline bool session_is_ready() const noexcept
	{
		switch (session_state) {
		case State::RUNNING:
		case State::SESSION_AWAIT_OPEN:
		case State::SESSION_AWAIT_LOADED:
		case State::HELLO_START:
			return true;
		default:
			return false;
		}
	}

	inline bool session_has_failed() const noexcept
	{
		return session_state == State::FAILED;
	}

	void start_session();

	inline size_t discovered_peers() const noexcept
	{
		return peers.size();
	}
	inline const NonPeer& peer_at(const std::string& client_id) const
	{
		return peers.at(client_id);
	}

private:
	void step_state_machine();
	void register_callbacks();

	void send_announce();
	void send_hello();
	void send_signal_list();

	template <typename... T>
	void handle_reply(T... args);
	template <typename... T>
	void handle_error(T... args);

	void handle_open(std::string_view project_path,
	                 std::string_view display_name,
	                 std::string_view client_id);
	void handle_save();
	void handle_signal_list(const lo::Address& peer);
	void handle_signal_list_reply(std::string_view signal_path,
	                              float min,
	                              float max,
	                              float default_value);

	void assert_server_announce_matches_required_capabilities(
	  std::string_view capabilities);

	inline bool peer_is_known(const std::string& client_id) const;

	lo::ServerThread s2c_thread;
	const lo::Address c2s_addr;
	const std::string_view executable_name;

	std::atomic<State> session_state{State::NO_SESSION};
	std::mutex state_machine_mtx;

	bool handshake_complete{false};
	bool session_loaded{false};
	bool project_opened{false};

	std::string client_id;
	std::map<std::string, NonPeer> peers;
	NonSignalList signals;

	size_t handshaked_peers{0};
	decltype(peers)::iterator current_handshaking_peer{peers.end()};
};
