#pragma once

#include <map>
#include <string_view>
#include <vector>

#include <lo/lo_cpp.h>

#include "NonSignal.hpp"

class NonSignalConnector;

class NonPeer
{
public:
	using signal_value_type = float;

	inline NonPeer(std::string_view client_id, std::string_view client_name,
	               std::string_view client_version, std::string_view url,
	               lo::ServerThread& osc_server)
	    : client_id(client_id)
	    , client_name(client_name)
	    , client_version(client_version)
	    , addr(url.data())
	    , osc_server(osc_server)
	{}
	inline NonPeer(const NonPeer& src)
	    : client_id(src.client_id)
	    , client_name(src.client_name)
	    , client_version(src.client_version)
	    , addr(src.addr.url())
	    , osc_server(src.osc_server)
	{}
	inline ~NonPeer() = default;

	inline const std::string& id() const noexcept
	{
		return client_id;
	}
	inline const std::string& name() const noexcept
	{
		return client_name;
	}
	inline const lo::Address& address() const noexcept
	{
		return addr;
	}

	inline size_t discovered_signals() const noexcept
	{
		return signals.size();
	}

	inline const NonRemoteSignal& signal_at(const std::string& path) const
	{
		return signals.at(path);
	}

	void fetch_signal_list();

	inline void register_signal(const NonSignal& sig)
	{
		register_signal(sig.path, sig.min, sig.max, sig.default_value,
		                sig.direction);
	}
	void register_signal(std::string_view path, signal_value_type min,
	                     signal_value_type max, signal_value_type default_value,
	                     NonSignal::Direction dir = NonSignal::Direction::IN);

	void send(lo::Bundle& msg_bundle);

	void register_signals_at(NonSignalConnector& connector) const;

private:
	std::string client_id;
	std::string client_name;
	std::string client_version;
	lo::Address addr;
	lo::ServerThread& osc_server;

	// NonRemoteSignalList signals;
	std::map<std::string, NonRemoteSignal> signals;
};
