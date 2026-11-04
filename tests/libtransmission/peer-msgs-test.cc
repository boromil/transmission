// This file Copyright (C) 2013-2022 Mnemosyne LLC.
// It may be used under GPLv2 (SPDX: GPL-2.0-only), GPLv3 (SPDX: GPL-3.0-only),
// or any future license endorsed by Mnemosyne LLC.
// License text can be found in the licenses/ folder.

#include <gtest/gtest.h>

#include <libtransmission/torrent.h> // tr_torrent, needed by peer-common.h
#include <libtransmission/peer-msgs.h>

TEST(PeerMsgs, placeholder)
{
}

TEST(PeerMsgs, maxMessageLen)
{
    // before the piece count is known, a magnet transfer still allows a bitfield of 8M pieces
    auto constexpr MagnetFloor = size_t{ 1024U * 1024U };
    EXPECT_EQ(MagnetFloor, max_peer_message_len(0));
    EXPECT_EQ(MagnetFloor, max_peer_message_len(1000));

    // with a known piece count the largest legitimate message is its bitfield
    EXPECT_EQ(1U + (9'000'000U / 8U), max_peer_message_len(9'000'000));
    EXPECT_EQ(1U + (100'000'000U / 8U), max_peer_message_len(100'000'000));
}
