#pragma once

#include <atomic>
#include <cassert>
#include <map>
#include <mutex>
#include <string>
#include <string_view>

#include <iostream>

#include <lo/lo_cpp.h>

#include "JackClient.hpp"
#include "NonPeer.hpp"

struct F1InputChange;
class JackClient;

/**
 * Main interface to the Non Session Manager and implementation of NSM related
 * protocols
 *
 * This class aims to implement the whole Non Session Management protocol using
 * a state machine as well as handle traffic received from the Non Session
 * Manager and included Non Peers.
 *
 * This class must be the only class responsible for receiving and processing
 * OSC messages. It furthermore is responsible for creating objects representing
 * any OSC related stuff or preparing the data for creating such objects (e.g.
 * {@link NonPeer}s, partially {@link NonSignal}s).
 *
 * The class contains its own OSC server and state machine to handle the
 * communication to the Non Session Manager and its connected peers. It
 * provides a listener thread for handling incoming OSC messages as well as
 * separate handler methods for separate message types.
 *
 * The state machine is responsible for emitting any non-trivial events which
 * should happen in response to incoming events.
 * Furthermore, the state of input variables is partially tracked inside the
 * class while the checking of these input variables and the corresponding state
 * changes happen inside the state machine.
 */
class NonSessionHandler
{
public:
	constexpr static int NON_API_VERSION_MAJOR{1};
	constexpr static int NON_API_VERSION_MINOR{2};
	constexpr static std::chrono::seconds SESSION_HANDSHAKE_TIMEOUT{5};

	constexpr static const char* NSM_ANNOUNCE{"/nsm/server/announce"};
	constexpr static const char* NSM_BROADCAST{"/nsm/server/broadcast"};
	constexpr static const char* NSM_INCOMING_BROADCAST{
	  "/nsm/client/broadcast"};
	constexpr static const char* NSM_HELLO{"/non/hello"};
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

	constexpr static const char* OSC_SIG_MTX{"/kontrolf1/mtx/"};
	constexpr static const char* OSC_SIG_STOP{"/kontrolf1/stop/"};
	constexpr static const char* OSC_SIG_SPECIAL{"/kontrolf1/special/"};
	constexpr static const char* OSC_SIG_KNOB{"/kontrolf1/knob/"};
	constexpr static const char* OSC_SIG_FADER{"/kontrolf1/fader/"};

	constexpr static float OSC_BTN_PRESSED{1.f};
	constexpr static float OSC_BTN_RELEASED{0.f};

	/**
	 * States of the internal state machine
	 */
	enum class State : int
	{
		NO_SESSION,
		HANDSHAKE_START,
		HANDSHAKE_AWAIT_REPLY,
		SESSION_AWAIT_OPEN,
		HELLO_START,
		PEER_AWAIT_SIGNAL_LIST,
		RUNNING,
		FAILED
	};

	/**
	 * Create a new instance and OSC server thread
	 */
	template <typename string_type>
	inline NonSessionHandler(const string_type& c2s_addr,
	                         std::string_view executable_name,
	                         const NonSignalList& signals = {},
	                         bool jack_midi = false)
	    : s2c_thread(nullptr)
	    , c2s_addr(c2s_addr)
	    , executable_name(executable_name)
	    , signals(signals)
	    , jack_midi(jack_midi)
	{
		assert(s2c_thread.is_valid());
		register_callbacks();
		s2c_thread.start();
	}
	NonSessionHandler(const NonSessionHandler&) = delete;
	// FIXME: Why do we have these two destructors?
#ifndef DEBUG
	inline ~NonSessionHandler() = default;
#else
	~NonSessionHandler();
#endif

	/**
	 * Return an address to send messages to or from the internal OSC server
	 */
	operator lo::Address() const
	{
		return {s2c_thread.url()};
	}

	/**
	 * Check, if the state machine is in a state where it is ready to process
	 * input events
	 */
	inline bool session_is_ready() const noexcept
	{
		switch (session_state) {
		case State::RUNNING:
		case State::SESSION_AWAIT_OPEN:
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

	/**
	 * Return the amount of peers discovered via NSM
	 */
	inline size_t discovered_peers() const noexcept
	{
		return peers.size();
	}
	/**
	 * Get the {@link NonPeer} with the corresponding client id
	 *
	 * @throws std::out_of_range if the peer with the client_id does not exist
	 */
	inline const NonPeer& peer_at(const std::string& client_id) const
	{
		return peers.at(client_id);
	}

	/**
	 * Broadcast an input event to all Non Peers
	 *
	 * This is the default way to send input events to Non Peers, but maybe we
	 * can improve this by keeping track of which input is associated with with
	 * signals.
	 */
	void broadcast_input_event(const F1InputChange& changes);

private:
	/**
	 * A single step in the state machine
	 *
	 * It first acquires a lock to ensure only one state machine step is
	 * executed at the same time. It then checks for the current state and acts
	 * depending on the state. Some states cause the state machine to be rerun
	 * immediately.
	 */
	void step_state_machine();
	void register_callbacks();

	/** @name Senders
	 *
	 * @brief Methods to send message to the Non Session Manager
	 *
	 * These methods only send messages to the Non Session Manager, but do not
	 * change the state of the state machine.
	 */
	//@{
	void send_announce();
	void send_hello();
	void send_signal_list();
	//@}

	/** @name Incoming message handlers
	 *
	 * @brief handlers for incoming messages
	 *
	 * These methods are handlers for incoming messages from NSM and Non Peers.
	 */
	//@{
	template <typename... T>
	void handle_reply(T... args);
	template <typename... T>
	void handle_error(T... args);

	void handle_open(std::string_view project_path,
	                 std::string_view display_name,
	                 std::string_view client_id);
	void handle_save();
	/**
	 * Handle incoming /signal/list
	 *
	 * This method responds by sending all signals we have
	 *
	 * Not to be confused with {@link handle_signal_list_reply()} which handles
	 * incoming replies to a /signal/list which we have sent to the peer using
	 * {@link send_signal_list()}.
	 */
	void handle_signal_list(const lo::Address& peer);
	/**
	 * Handle incoming replies to /signal/list
	 *
	 * This method adds all received signals to the {@link NonPeer}'s signal
	 * list.
	 *
	 * Not to be confused with {@link handle_signal_list()} which handles
	 * incoming /signal/list requests rather than replies to requests we have
	 * sent out using {@link send_signal_list()}.
	 */
	void handle_signal_list_reply(std::string_view signal_path,
	                              float min,
	                              float max,
	                              float default_value);
	//@}

	/// @name Misc methods
	//@{
	void assert_server_announce_matches_required_capabilities(
	  std::string_view capabilities);

	inline bool peer_is_known(const std::string& client_id) const;
	//@}

	/// @name OSC/Non Stuff
	//@{
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
	//@}

	/// @name Jack Stuff
	//@{
	bool jack_midi;
	std::unique_ptr<JackClient> jack;
	//@}
};
