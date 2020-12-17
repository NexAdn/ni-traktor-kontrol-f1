#pragma once

#include <string_view>
#include <vector>

#include <lo/lo_cpp.h>

#include "NonSignal.hpp"

/**
 * Representation of a Non peer connected via the NSM OSC API
 *
 * Each Peer within the NSM ecosystem is identified / interacted with using a
 * IDs, names and URLs. Furthermore, each peer is reachable from a certain OSC
 * endpoint and most peers provide inbound and outbound signals via this OSC
 * endpoint.
 *
 * This class concentrates all information regarding a peer as well as means to
 * interact with the peer.
 */
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

	/**
	 * Return, how many signals have been discovered at this peer
	 */
	inline size_t discovered_signals() const noexcept
	{
		return signals.size();
	}

	/**
	 * Fetch the signal list from this peer via OSC and store it internally.
	 *
	 * The received signal list can currently not be accessed.
	 */
	void fetch_signal_list();
	/**
	 * Send the given signal list to the peer via OSC
	 *
	 * FIXME: Do we even need this? Currently, sending the signal list is done
	 * in {@link NonSessionHandler.cpp}
	 */
	void send_signal_list(const NonSignalList& signals);

	/**
	 * Register a signal for this peer.
	 *
	 * As incoming message handling is done exclusively in
	 * {@link NonSessionHandler}, this method provides and interface for
	 * NonSessionHandler to register all received signals.
	 *
	 * This method should not be used from outside NonSessionHandler.
	 */
	void register_signal(const NonSignal& sig);

	/**
	 * Send a raw lo::Bundle to the peer via OSC.
	 */
	void send(lo::Bundle& msg_bundle);

private:
	std::string client_id;
	std::string client_name;
	std::string client_version;
	lo::Address addr;
	lo::ServerThread& osc_server;

	NonSignalList signals;
};
