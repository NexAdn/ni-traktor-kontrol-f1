#include "test.hpp"

#include <array>

#include "NonPeer.hpp"
#include "NonSignalConnector.hpp"

class NonSignalConnectorF : public ::testing::Test
{
protected:
	NonSignalConnectorF()
	{
		p1.register_signal(peer_signals[0]);
		p1.register_signal(peer_signals[1]);
		p1.register_signal(peer_signals[2]);
		p2.register_signal(peer_signals[3]);

		connector.add_peer(p1);
		connector.add_peer(p2);
		connector.add_peer(p3);
	}

	virtual ~NonSignalConnectorF() = default;

	lo::ServerThread dummy_srv{0};
	NonPeer p1{"client_id1", "client_name", "1.1", "url", dummy_srv};
	NonPeer p2{"client_id2", "client_name", "1.1", "url", dummy_srv};
	NonPeer p3{"client_id3", "client_name", "1.1", "url", dummy_srv};

	NonSignalList peer_signals{{"/c/1"}, {"/c/2"}, {"/c/3"}, {"/c/4"}};

	NonSignalConnector connector;
};

class NonSignalConnector_WithLocalSignalsF : public NonSignalConnectorF
{
protected:
	NonSignalConnector_WithLocalSignalsF()
	{
		for (const auto& sig : signals)
			connector.add_signal(sig);
	}

	std::vector<NonSignal> signals{{"/s/1"}, {"/s/2"}, {"/s/3"}};
};

TEST_F(NonSignalConnectorF, registers_peers)
{
	ASSERT_TRUE(connector.has_peer(p1));
	ASSERT_TRUE(connector.has_peer(p2));
	ASSERT_TRUE(connector.has_peer(p3));
}

TEST_F(NonSignalConnectorF, register_peer_adds_peer_signals)
{
	ASSERT_EQ(3, connector.registered_signals(p1));
	ASSERT_EQ(1, connector.registered_signals(p2));
	ASSERT_EQ(0, connector.registered_signals(p3));

	ASSERT_TRUE(connector.has_signal({"/c/1"}));
	ASSERT_TRUE(connector.has_signal({"/c/2"}));
	ASSERT_TRUE(connector.has_signal({"/c/3"}));
}

TEST_F(NonSignalConnectorF, counts_peer_signals)
{
	ASSERT_EQ(4, connector.registered_signals());
}

TEST_F(NonSignalConnector_WithLocalSignalsF,
       registers_local_signals_to_local_signal_list)
{
	for (const auto& sig : signals) {
		ASSERT_TRUE(connector.has_signal(sig));
		ASSERT_TRUE(connector.signal_is_local(sig));
	}
}

TEST_F(NonSignalConnector_WithLocalSignalsF, counts_local_signals)
{
	ASSERT_EQ(7, connector.registered_signals());
}

TEST_F(NonSignalConnector_WithLocalSignalsF, creates_connection_on_connect)
{
	NonSignalConnector::Connection c =
	  connector.connect(signals.at(0), peer_signals.at(0));
	ASSERT_EQ(signals.at(0), c.from);
	ASSERT_EQ(peer_signals.at(0), c.to);
	ASSERT_EQ(NonSignalConnector::Connection::Direction::OUTBOUND, c.direction);
}

class NonSignalConnector_WithLocalSignals_OnSignalF
    : public NonSignalConnector_WithLocalSignalsF
{
protected:
	NonSignalConnector_WithLocalSignals_OnSignalF()
	{
		connector.connect(signals.at(0), peer_signals.at(0));
		connector.connect(signals.at(1), peer_signals.at(1));
		connector.connect(signals.at(1), peer_signals.at(2));
	}

	~NonSignalConnector_WithLocalSignals_OnSignalF() = default;
};

TEST_F(NonSignalConnector_WithLocalSignals_OnSignalF,
       propagates_signal_change_to_connected_signals)
{
}
