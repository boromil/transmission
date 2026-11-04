// This file Copyright © Transmission authors and contributors.
// It may be used under the MIT (SPDX: MIT) license.
// License text can be found in the licenses/ folder.

#pragma once

#ifndef __TRANSMISSION__
#error only libtransmission should #include this header.
#endif

#include <algorithm>
#include <cstddef> // size_t
#include <ctime> // time_t
#include <map>
#include <string>
#include <string_view>

// Tracks RPC authentication failures per peer, so that one client's failures do not lock out
// everyone else and a lockout ends on its own instead of lasting until the process restarts.
class tr_rpc_login_attempts
{
public:
    // how long a peer stays locked out after reaching the failure threshold
    static constexpr auto Window = time_t{ 15 * 60 };

    // how many peers to remember before evicting the oldest entry
    static constexpr auto MaxPeers = size_t{ 256U };

    [[nodiscard]] bool is_locked_out(std::string_view peer, time_t now, size_t limit) const
    {
        if (limit == 0U)
        {
            return false;
        }

        auto const iter = attempts_.find(peer);
        return iter != std::end(attempts_) && iter->second.count >= limit && now - iter->second.first_failure < Window;
    }

    void note_failure(std::string_view peer, time_t now)
    {
        auto const iter = attempts_.find(peer);
        if (iter == std::end(attempts_))
        {
            if (std::size(attempts_) >= MaxPeers)
            {
                evict_one(now);
            }

            attempts_.try_emplace(std::string{ peer }, Attempt{ 1U, now });
            return;
        }

        // a failure after the window closes starts a new window
        if (now - iter->second.first_failure >= Window)
        {
            iter->second = Attempt{ 1U, now };
        }
        else
        {
            ++iter->second.count;
        }
    }

    void note_success(std::string_view peer)
    {
        if (auto const iter = attempts_.find(peer); iter != std::end(attempts_))
        {
            attempts_.erase(iter);
        }
    }

    void clear() noexcept
    {
        attempts_.clear();
    }

    [[nodiscard]] size_t size() const noexcept
    {
        return std::size(attempts_);
    }

private:
    struct Attempt
    {
        size_t count = 0U;
        time_t first_failure = 0;
    };

    void evict_one(time_t now)
    {
        // drop peers whose window has closed before evicting anyone still active
        for (auto iter = std::begin(attempts_); iter != std::end(attempts_);)
        {
            if (now - iter->second.first_failure >= Window)
            {
                iter = attempts_.erase(iter);
            }
            else
            {
                ++iter;
            }
        }

        if (std::size(attempts_) < MaxPeers)
        {
            return;
        }

        auto const oldest = std::ranges::min_element(
            attempts_,
            [](auto const& lhs, auto const& rhs) { return lhs.second.first_failure < rhs.second.first_failure; });
        attempts_.erase(oldest);
    }

    std::map<std::string, Attempt, std::less<>> attempts_;
};
