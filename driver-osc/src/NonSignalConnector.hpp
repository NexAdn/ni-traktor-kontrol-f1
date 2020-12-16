#pragma once

#include <unordered_map>
#include <vector>

class NonSignal;
class NonRemoteSignal;
class NonPeer;

class NonSignalConnector
{
public:
	struct Connection
	{
		enum class Direction
		{
			INBOUND,
			OUTBOUND
		};

		const NonSignal& from;
		const NonSignal& to;
		const Direction direction;
	};

	using signal_value_type = float;
	using remote_signal_set = std::vector<NonRemoteSignal>;
	using signal_set = std::vector<NonSignal>;

	inline NonSignalConnector() = default;
	inline NonSignalConnector(const NonSignalConnector&) = default;
	inline ~NonSignalConnector() = default;

	inline bool has_peer(const NonPeer& peer) const noexcept
	{
		return peer_signals.find(&peer) != peer_signals.end();
	}

	size_t registered_signals() const noexcept;
	inline size_t registered_signals(const NonPeer& peer) const noexcept
	{
		return peer_signals.at(&peer).size();
	}

	bool has_signal(const NonSignal& signal) const noexcept;
	bool signal_is_local(const NonSignal& signal) const noexcept;

	void add_peer(const NonPeer& peer);
	void add_signal(const NonSignal& signal);

	Connection safe_connect(const NonSignal& from, const NonSignal& to);
	Connection connect(const NonSignal& from, const NonSignal& to);

	void send_signal(const NonSignal& signal, signal_value_type value);

private:
	std::unordered_map<const NonPeer*, remote_signal_set> peer_signals;
	std::vector<NonSignal> local_signals;

	std::unordered_map<const NonSignal*, std::vector<Connection>> connections;
};

