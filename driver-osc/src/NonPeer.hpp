#pragma once

#include <string_view>
#include <vector>

#include <lo/lo_cpp.h>

#include "NonSignalList.hpp"

class NonPeer
{
public:
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

	inline std::string_view id() const noexcept
	{
		return client_id;
	}
	inline std::string_view name() const noexcept
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

	void fetch_signal_list();
	// FIXME: Do we even need this? Currently, sending the signal list is done
	// in NonSessionHandler.cpp
	void send_signal_list(const NonSignalList& signals);

	void register_signal(const NonSignal& sig);

private:
	std::string client_id;
	std::string client_name;
	std::string client_version;
	lo::Address addr;
	lo::ServerThread& osc_server;

	NonSignalList signals;
};
