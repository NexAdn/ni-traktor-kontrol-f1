#include <queue>
#include <stack>
#include <thread>

#include <lo/lo_cpp.h>

#include "test.hpp"

#include "F1InputState.hpp"
#include "NonSessionHandler.hpp"

#include "config.h"

#define DBG_MODULE_NAME "NSH_TEST"
#include "debug.hpp"

namespace
{
template <size_t msecs = 100>
inline void short_sleep()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
}
} // namespace

TEST(NonSessionHandler, initializies_without_ready_session)
{
	NonSessionHandler nsh("invalid", "invalid");

	ASSERT_FALSE(nsh.session_is_ready());
}

class ServerEmulation
{
public:
	inline ServerEmulation(bool start_immediately = true)
	    : server_thread(nullptr), server_uri(server_thread.url())
	{
		debug(std::string{"Initializing server thread with URL "}
		      + server_thread.url() + '\n');
		server_thread.add_method(
		  NonSessionHandler::NSM_ANNOUNCE, "sssiii",
		  [this](lo::Message msg) {
			  received_messages.push({NonSessionHandler::NSM_ANNOUNCE, msg});
		  },
		  nullptr);
		server_thread.add_method("/reply", "ss", [this](lo::Message msg) {
			debug("ServerEmulation: Received /reply\n");
			received_messages.push({"/reply", msg});
		});
		server_thread.add_method(
		  "/nsm/server/broadcast", "sssss",
		  [this](lo::Message msg) { received_broadcasts.push(msg); });

		if (start_immediately) {
			debug("ServerEmulation: Starting server_thread\n");
			server_thread.start();
			short_sleep<>();
		}
	}
	~ServerEmulation() = default;

	void prepare_session(NonSessionHandler& nsh)
	{
		nsh.start_session();
		short_sleep<>();
		debug("Sending /reply announce to "
		      + static_cast<lo::Address>(nsh).url() + '\n');
		lo::Address{nsh}.send_from(
		  server_thread, "/reply", "ssss", NonSessionHandler::NSM_ANNOUNCE,
		  "hello", "name_of_session_manager",
		  NonSessionHandler::NSM_REQUIRED_SERVER_CAPABILITIES);
		short_sleep<>();
	}

	void load_session(NonSessionHandler& nsh)
	{
		debug("Opening project and sending all ready\n");
		lo::Address{nsh}.send_from(server_thread, NonSessionHandler::NSM_OPEN,
		                           "sss", "path", "display_name", "client_id");
		lo::Address{nsh}.send_from(server_thread,
		                           NonSessionHandler::NSM_SESSION_LOADED, "");
		short_sleep<>();
	}

	std::stack<std::pair<std::string, lo::Message>> received_messages;
	std::stack<lo::Message> received_broadcasts;
	std::queue<std::pair<std::string, lo::Message>> messages_to_send;

	lo::ServerThread server_thread;
	const std::string server_uri;
};

class MixerEmulation : public ServerEmulation
{
public:
	MixerEmulation() : ServerEmulation(false)
	{
		debug("MixerEmulation: Listening at " + server_thread.url() + '\n');
		server_thread.add_method("/reply", "ssiii", [this](lo::Message msg) {
			debug("MixerEmulation: Received reply\n");
			received_messages.push({"/reply", msg});
		});
		server_thread.add_method("/reply", "s", [this](lo::Message msg) {
			debug("MixerEmulation: Received reply\n");
			received_messages.push({"/reply", msg});
		});
		server_thread.add_method(
		  NonSessionHandler::OSC_SIGNAL_LIST, "", [this](lo::Message msg) {
			  debug("MixerEmulation: Received signal list\n");
			  received_messages.push({"/signal/list", msg});
		  });

		server_thread.start();
		short_sleep<>();
	}

	void prepare_session(NonSessionHandler& nsh) = delete;

	void send_hello(NonSessionHandler& nsh)
	{
		debug("MixerEmulation: Sending hello\n");
		lo::Address{nsh}.send_from(
		  server_thread, NonSessionHandler::NSM_INCOMING_BROADCAST, "sssss",
		  NonSessionHandler::NSM_HELLO, server_thread.url().c_str(),
		  client_name.c_str(), "42.42", client_id.c_str());
	}

	void fetch_signal_list(NonSessionHandler& nsh)
	{
		lo::Address{nsh}.send_from(server_thread,
		                           NonSessionHandler::OSC_SIGNAL_LIST, "");
	}

	void transmit_signal_list(NonSessionHandler& nsh)
	{
		debug("MixerEmulation: Transmitting signal list\n");
		lo::Address nsh_addr = nsh;
		for (const auto& signal : signals) {
			nsh_addr.send_from(server_thread, "/reply", "ssiii",
			                   NonSessionHandler::OSC_SIGNAL_LIST,
			                   signal.path.c_str(), signal.min, signal.max,
			                   signal.default_value);
		}
		nsh_addr.send_from(server_thread, "/reply", "s",
		                   NonSessionHandler::OSC_SIGNAL_LIST);
	}

	std::string client_id{"MixerEmulationId"};
	std::string client_name{"MixerEmulationName"};

	const NonSignalList signals{{"/signal/one", 0, 100, 50},
	                            {"/signal/two", 10, 20, 10},
	                            {"/signal/three", 0, 1, 0}};
};

TEST(NonSessionHandler, sends_announce_on_session_start)
{
	ServerEmulation srv;

	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");

	nsh.start_session();

	short_sleep<>();

	ASSERT_EQ(1, srv.received_messages.size());

	auto& announce_message = srv.received_messages.top();

	ASSERT_EQ(NonSessionHandler::NSM_ANNOUNCE, announce_message.first);
	ASSERT_EQ("sssiii", announce_message.second.types());

	auto** args = announce_message.second.argv();
	ASSERT_EQ(std::string{NonSessionHandler::NSM_CLIENT_NAME},
	          reinterpret_cast<const char*>(&args[0]->s));
	ASSERT_EQ(std::string{NonSessionHandler::NSM_CLIENT_CAPABILITIES},
	          reinterpret_cast<const char*>(&args[1]->s));
	ASSERT_EQ(std::string{"/invalid/path/exe"},
	          reinterpret_cast<const char*>(&args[2]->s));
	ASSERT_EQ(NonSessionHandler::NON_API_VERSION_MAJOR, args[3]->i);
	ASSERT_EQ(NonSessionHandler::NON_API_VERSION_MINOR, args[4]->i);
}

TEST(NonSessionHandler, is_ready_after_successful_announce_handshake_and_load)
{
	ServerEmulation srv;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	nsh.start_session();
	srv.prepare_session(nsh);
	srv.load_session(nsh);

	short_sleep<>();
	ASSERT_TRUE(nsh.session_is_ready());
}

TEST(NonSessionHandler, is_failed_after_failed_handshake)
{
	ServerEmulation srv;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	nsh.start_session();
	short_sleep<>();

	debug("Sending /error announce to " + static_cast<lo::Address>(nsh).url()
	      + '\n');
	lo::Address{nsh}.send_from(srv.server_thread, "/error", "sis",
	                           NonSessionHandler::NSM_ANNOUNCE, -1,
	                           "my error message");

	short_sleep<>();
	ASSERT_FALSE(nsh.session_is_ready());
	ASSERT_TRUE(nsh.session_has_failed());
}

TEST(NonSessionHandler, fails_on_missing_required_server_capabilities)
{
	ServerEmulation srv;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	// TODO: Need to assert an exception gets thrown
	// nsh.start_session();
	short_sleep<>();
	lo::Address{nsh}.send_from(srv.server_thread, "/reply", "ssss",
	                           NonSessionHandler::NSM_ANNOUNCE, "hello",
	                           "dummy-manager"
	                           "cap");
	short_sleep<>();
	ASSERT_TRUE(nsh.session_has_failed());
}

TEST(NonSessionHandler, starts_hello_after_open_and_session_loaded)
{
	ServerEmulation srv;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	srv.prepare_session(nsh);
	short_sleep<>();

	ASSERT_EQ(0, srv.received_broadcasts.size());

	lo::Address{nsh}.send_from(srv.server_thread, NonSessionHandler::NSM_OPEN,
	                           "sss", "projectpath", "display_name",
	                           "client_id");
	lo::Address{nsh}.send_from(srv.server_thread,
	                           NonSessionHandler::NSM_SESSION_LOADED, "");
	short_sleep<>();

	ASSERT_EQ(1, srv.received_broadcasts.size());

	auto& hello_msg = srv.received_broadcasts.top();
	std::string_view arg1 =
	  reinterpret_cast<const char*>(&hello_msg.argv()[0]->s);
	std::string_view arg2 =
	  reinterpret_cast<const char*>(&hello_msg.argv()[1]->s);
	std::string_view arg3 =
	  reinterpret_cast<const char*>(&hello_msg.argv()[2]->s);
	std::string_view arg4 =
	  reinterpret_cast<const char*>(&hello_msg.argv()[3]->s);
	std::string_view arg5 =
	  reinterpret_cast<const char*>(&hello_msg.argv()[4]->s);
	ASSERT_EQ("/nsm/hello", arg1);
	ASSERT_EQ(static_cast<lo::Address>(nsh).url(), arg2);
	ASSERT_EQ(NonSessionHandler::NSM_CLIENT_NAME, arg3);
	ASSERT_EQ(KONTROLF1_VERSION, arg4);
	ASSERT_EQ("client_id", arg5);
}

TEST(NonSessionHandler, returns_signals_on_signal_list)
{
	ServerEmulation srv;
	MixerEmulation mix;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe",
	                      {{"/c/1", 0, 10, 5}, {"/c/2", 10, 30, 20}});
	srv.prepare_session(nsh);
	short_sleep<>();

	mix.fetch_signal_list(nsh);
	short_sleep<>();

	ASSERT_GT(mix.received_messages.size(), 0);

	lo::Message finish_msg = mix.received_messages.top().second;
	mix.received_messages.pop();
	ASSERT_EQ("s", finish_msg.types());
	ASSERT_EQ(std::string{NonSessionHandler::OSC_SIGNAL_LIST},
	          reinterpret_cast<const char*>(&finish_msg.argv()[0]->s));

	lo::Message control_2_msg = mix.received_messages.top().second;
	mix.received_messages.pop();
	ASSERT_EQ("ssiii", control_2_msg.types());
	lo_arg** control_2_argv = control_2_msg.argv();
	ASSERT_EQ(std::string{NonSessionHandler::OSC_SIGNAL_LIST},
	          reinterpret_cast<const char*>(&control_2_argv[0]->s));
	ASSERT_EQ(std::string{"/c/2"},
	          reinterpret_cast<const char*>(&control_2_argv[1]->s));
	ASSERT_EQ(10, control_2_argv[2]->i);
	ASSERT_EQ(30, control_2_argv[3]->i);
	ASSERT_EQ(20, control_2_argv[4]->i);

	lo::Message control_1_msg = mix.received_messages.top().second;
	mix.received_messages.pop();
	ASSERT_EQ("ssiii", control_1_msg.types());
	lo_arg** control_1_argv = control_1_msg.argv();
	ASSERT_EQ(std::string{NonSessionHandler::OSC_SIGNAL_LIST},
	          reinterpret_cast<const char*>(&control_1_argv[0]->s));
	ASSERT_EQ(std::string{"/c/1"},
	          reinterpret_cast<const char*>(&control_1_argv[1]->s));
	ASSERT_EQ(0, control_1_argv[2]->i);
	ASSERT_EQ(10, control_1_argv[3]->i);
	ASSERT_EQ(5, control_1_argv[4]->i);
}

TEST(NonSessionHandler, requests_signals_on_hello)
{
	ServerEmulation srv;
	MixerEmulation mix;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	srv.prepare_session(nsh);
	srv.load_session(nsh);
	mix.send_hello(nsh);
	short_sleep<>();

	ASSERT_EQ(1, mix.received_messages.size());
	std::string cmd = mix.received_messages.top().first;
	lo::Message msg = mix.received_messages.top().second;

	ASSERT_EQ(NonSessionHandler::OSC_SIGNAL_LIST, cmd);
	ASSERT_EQ("", msg.types());
}

TEST(NonSessionHandler, registers_peers_after_hello)
{
	ServerEmulation srv;
	MixerEmulation mix;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	srv.prepare_session(nsh);
	srv.load_session(nsh);
	mix.send_hello(nsh);
	short_sleep<>();

	ASSERT_EQ(1, nsh.discovered_peers());

	const NonPeer& peer = nsh.peer_at(mix.client_id);
	ASSERT_EQ(mix.client_id, peer.id());
	ASSERT_EQ(mix.client_name, peer.name());
	ASSERT_EQ(mix.server_thread.url(), peer.address().url());
}

TEST(NonSessionHandler, registers_peer_signals_upon_receiving_them)
{
	ServerEmulation srv;
	MixerEmulation mix;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	srv.prepare_session(nsh);
	srv.load_session(nsh);
	mix.send_hello(nsh);
	short_sleep<>();
	mix.received_messages = {};

	mix.transmit_signal_list(nsh);
	short_sleep<>();

	const NonPeer& peer = nsh.peer_at(mix.client_id);
	ASSERT_EQ(mix.signals.size(), peer.discovered_signals());
}

TEST(NonSessionHandler, is_ready_after_completing_hello_handshake)
{
	ServerEmulation srv;
	MixerEmulation mix;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	srv.prepare_session(nsh);
	srv.load_session(nsh);
	mix.send_hello(nsh);
	short_sleep<>();

	mix.transmit_signal_list(nsh);
	short_sleep<>();

	ASSERT_TRUE(nsh.session_is_ready());
}

TEST(NonSessionHandler, replies_to_open_command)
{
	ServerEmulation srv;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	srv.prepare_session(nsh);

	debug("Sending open\n");
	lo::Address{nsh}.send_from(srv.server_thread, NonSessionHandler::NSM_OPEN,
	                           "sss", "projectpath", "display_name",
	                           "client_id");
	short_sleep<>();
	ASSERT_EQ(2, srv.received_messages.size());
	auto& open_reply = srv.received_messages.top();
	ASSERT_EQ(std::string{NonSessionHandler::NSM_OPEN},
	          reinterpret_cast<const char*>(&open_reply.second.argv()[0]->s));
}

TEST(NonSessionHandler, replies_to_save_command)
{
	ServerEmulation srv;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	srv.prepare_session(nsh);

	debug("Sending save");
	lo::Address{nsh}.send_from(srv.server_thread, NonSessionHandler::NSM_SAVE,
	                           "");
	short_sleep<>();

	ASSERT_EQ(2, srv.received_messages.size());
	auto& save_reply = srv.received_messages.top();
	ASSERT_EQ(std::string{NonSessionHandler::NSM_SAVE},
	          reinterpret_cast<const char*>(&save_reply.second.argv()[0]->s));
}

class NonSessionHandlerOnInput : public ::testing::Test
{
protected:
	NonSessionHandlerOnInput() : srv(), nsh(srv.server_uri, "/invalid/path/exe")
	{
		srv.prepare_session(nsh);
		short_sleep<>();
		srv.received_messages = {};
	}

	ServerEmulation srv;
	NonSessionHandler nsh;
	F1InputChange diff;

	decltype(srv.received_messages)& recv = srv.received_messages;
};

TEST_F(NonSessionHandlerOnInput, broadcasts_matrix_input_changes)
{
	diff.pressed_buttons.matrix.push_back(3);
	diff.pressed_buttons.matrix.push_back(4);

	diff.released_buttons.matrix.push_back(1);
	diff.released_buttons.matrix.push_back(2);

	// nsh.broadcast_controller_state_change(diff);
	// short_sleep<>();

	// ASSERT_EQ(4, recv.size());

	// auto button2 = recv.top();
	// recv.pop();
	// auto button1 = recv.top();
	// recv.pop();
	// auto button4 = recv.top();
	// recv.pop();
	// auto button3 = recv.top();

	// ASSERT_EQ("/kontrolf1/button/matrix/2", button2.first);
	// ASSERT_EQ("/kontrolf1/button/matrix/1", button1.first);
	// ASSERT_EQ("/kontrolf1/button/matrix/4", button4.first);
	// ASSERT_EQ("/kontrolf1/button/matrix/3", button3.first);
}
