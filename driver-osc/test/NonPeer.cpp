#include "test.hpp"

#include "NonPeer.hpp"
#include "NonSignal.hpp"

class NonPeerF : public ::testing::Test
{
protected:
	inline NonPeerF()
	{
		peer.register_signal("/c/1", 0.f, 1.f, 0.f);
		peer.register_signal("/c/2", 0.f, 1.f, 0.f);
		peer.register_signal("/c/3", 0.f, 1.f, 0.f);
		peer.register_signal("/c/4", 0.f, 1.f, 0.f);
		peer.register_signal("/c/5", 0.f, 1.f, 0.f);
	}

	lo::ServerThread srv{0};
	NonPeer peer{"client_id", "client_name", "client_version", "url", srv};
};

TEST_F(NonPeerF, registers_signals)
{
	ASSERT_EQ(5, peer.discovered_signals());
}

TEST_F(NonPeerF, finds_signals_by_path)
{
	auto& sig = peer.signal_at("/c/1");

	ASSERT_EQ("/c/1", sig.path);
	ASSERT_EQ(0.f, sig.min);
	ASSERT_EQ(1.f, sig.max);
	ASSERT_EQ(0.f, sig.default_value);
}

TEST_F(NonPeerF, throws_on_signal_not_found)
{
	ASSERT_THROW(peer.signal_at("/c/invalid"), std::out_of_range);
}
