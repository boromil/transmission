// This file Copyright (C) 2013-2022 Mnemosyne LLC.
// It may be used under GPLv2 (SPDX: GPL-2.0-only), GPLv3 (SPDX: GPL-3.0-only),
// or any future license endorsed by Mnemosyne LLC.
// License text can be found in the licenses/ folder.

#include <gtest/gtest.h>

#include <libtransmission/torrent.h> // tr_torrent, needed by peer-common.h
#include <libtransmission/peer-msgs.h>

#include "test-fixtures.h"

TEST(PeerMsgs, placeholder)
{
}

using PeerMsgsTest = tr::test::SessionTest;

TEST_F(PeerMsgsTest, pieceMessageBounds)
{
    auto* const tor = zeroTorrentInit(ZeroTorrentState::Complete);
    ASSERT_NE(nullptr, tor);

    auto const n_pieces = tor->piece_count();
    ASSERT_GT(n_pieces, 0U);

    EXPECT_TRUE(is_valid_piece_message(*tor, 0, 0, 1U));
    EXPECT_FALSE(is_valid_piece_message(*tor, n_pieces, 0, 1U));
    EXPECT_FALSE(is_valid_piece_message(*tor, 0, tor->piece_size(0), 1U));
    EXPECT_FALSE(is_valid_piece_message(*tor, 0, 0, 0U));
    EXPECT_FALSE(is_valid_piece_message(*tor, 0, 0, tr_block_info::BlockSize + 1U));
}
