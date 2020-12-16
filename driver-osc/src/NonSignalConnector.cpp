#include <algorithm>

#include "NonPeer.hpp"
#include "NonSignal.hpp"
#include "NonSignalConnector.hpp"

#define DBG_MODULE_NAME "NSC"
#include "debug.hpp"

namespace
{
template <typename IteratorT, typename T>
bool contains(IteratorT begin, IteratorT end, const T& item)
{
	return std::any_of(begin, end, [&item](auto& list_item) -> bool {
		return item == list_item;
	});
}
} // namespace

size_t NonSignalConnector::registered_signals() const noexcept
{
	size_t count{0};
	for (const auto& list : peer_signals)
		count += list.second.size();

	count += local_signals.size();

	return count;
}

bool NonSignalConnector::has_signal(const NonSignal& signal) const noexcept
{
	return signal_is_local(signal)
	       || std::any_of(peer_signals.begin(), peer_signals.end(),
	                      [&signal](auto& sig_set) -> bool {
		                      return contains(sig_set.second.begin(),
		                                      sig_set.second.end(), signal);
	                      });
}

bool NonSignalConnector::signal_is_local(const NonSignal& signal) const noexcept
{
	return std::any_of(local_signals.begin(), local_signals.end(),
	                   [&signal](auto& sig) -> bool { return signal == sig; });
}

void NonSignalConnector::add_peer(const NonPeer& peer)
{
	debug(std::string{"Adding peer "} + peer.id() + '\n');
	peer_signals.emplace(&peer, remote_signal_set{});
	peer.register_signals_at(*this);
}

void NonSignalConnector::add_signal(const NonSignal& signal)
{
	const NonRemoteSignal* rsignal =
	  dynamic_cast<const NonRemoteSignal*>(&signal);
	if (rsignal) {
		remote_signal_set& signals = peer_signals[&rsignal->owner];
		signals.push_back(*rsignal);
	} else {
		local_signals.emplace_back(signal);
	}
}

NonSignalConnector::Connection
NonSignalConnector::connect(const NonSignal& from, const NonSignal& to)
{
	assert(&from);
	assert(&to);

	Connection c{from, to,
	             signal_is_local(from) ? Connection::Direction::OUTBOUND :
                                         Connection::Direction::INBOUND};
	connections[&from].emplace_back(c);

	return c;
}
