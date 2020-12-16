#include "NonPeer.hpp"
#include "NonSessionHandler.hpp"
#include "NonSignalConnector.hpp"

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

void NonPeer::register_signal(std::string_view path, signal_value_type min,
                              signal_value_type max,
                              signal_value_type default_value,
                              NonSignal::Direction dir)
{
	debug(std::string{"Registering signal "} + path.data() + '\n');
	signals.emplace(path,
	                NonRemoteSignal{*this, path, min, max, default_value, dir});
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

void NonPeer::register_signals_at(NonSignalConnector& connector) const
{
	for (auto& signal : signals) {
		connector.add_signal(signal.second);
	}
}
