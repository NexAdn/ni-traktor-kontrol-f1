#include "NonPeer.hpp"
#include "NonSessionHandler.hpp"

#define DBG_MODULE_NAME "NSH"
#include "debug.hpp"

void NonPeer::fetch_signal_list()
{
	assert(addr.is_valid());
	assert(osc_server.is_valid());

	debug(std::string{"Fetching /signal/list from "} + addr.url() + '\n');
	int res =
	  addr.send_from(osc_server, NonSessionHandler::OSC_SIGNAL_LIST, "");
	assert(res >= 0);
}

void NonPeer::register_signal(const NonSignal& sig)
{
	debug(std::string{"Registering signal "} + sig.path + ":"
	      + std::to_string(sig.min) + "/" + std::to_string(sig.max) + "/"
	      + std::to_string(sig.default_value) + '\n');
	signals.push_back(sig);
}

void NonPeer::send(lo::Bundle& bundle)
{
	debug(std::string{"Sending bundle to peer "} + addr.url() + '\n');
#ifdef DEBUG
	bundle.print();
#endif
	assert(bundle.is_valid());
	addr.send_from(osc_server, bundle);
}
