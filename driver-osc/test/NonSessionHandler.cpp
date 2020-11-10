#include <queue>
#include <stack>
#include <thread>

#include <lo/lo_cpp.h>

#include "test.hpp"

#include "NonSessionHandler.hpp"

#include "debug.hpp"

namespace
{
template <size_t msecs = 10>
inline void short_sleep()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(msecs));
}
} // namespace

TEST(NonSessionHandler, initializies_without_running_session)
{
	NonSessionHandler nsh("invalid", "invalid");

	ASSERT_FALSE(nsh.session_is_running());
}

class ServerEmulation
{
public:
	inline ServerEmulation()
	    : server_thread(nullptr), server_uri(server_thread.url())
	{
		server_thread.add_method(
		  NonSessionHandler::NSM_ANNOUNCE, "sssiii",
		  [this](lo::Message msg) {
			  received_messages.push({NonSessionHandler::NSM_ANNOUNCE, msg});
		  },
		  nullptr);
		server_thread.add_method("/reply", "ss", [this](lo::Message msg) {
			received_messages.push({"/reply", msg});
		});

		server_thread.start();
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

	std::stack<std::pair<std::string, lo::Message>> received_messages;
	std::queue<std::pair<std::string, lo::Message>> messages_to_send;

	lo::ServerThread server_thread;
	const std::string server_uri;
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

TEST(NonSessionHandler, is_running_after_successful_announce_handshake)
{
	ServerEmulation srv;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	nsh.start_session();
	short_sleep<>();

	debug("Sending /reply announce to " + static_cast<lo::Address>(nsh).url()
	      + '\n');
	lo::Address{nsh}.send_from(
	  srv.server_thread, "/reply", "ssss", NonSessionHandler::NSM_ANNOUNCE,
	  "hello", "name_of_session_manager",
	  NonSessionHandler::NSM_REQUIRED_SERVER_CAPABILITIES);

	short_sleep<>();
	ASSERT_TRUE(nsh.session_is_running());
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
	ASSERT_FALSE(nsh.session_is_running());
	ASSERT_TRUE(nsh.session_has_failed());
}

TEST(NonSessionHandler, fails_on_missing_required_server_capabilities)
{
	ServerEmulation srv;
	NonSessionHandler nsh(srv.server_uri, "/invalid/path/exe");
	short_sleep<>();
	lo::Address{nsh}.send_from(srv.server_thread, "/reply", "ssss",
	                           NonSessionHandler::NSM_ANNOUNCE, "hello",
	                           "dummy-manager"
	                           "cap");
	short_sleep<>();
	ASSERT_TRUE(nsh.session_has_failed());
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
