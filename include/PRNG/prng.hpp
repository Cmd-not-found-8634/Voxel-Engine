#pragma once

#include <cstdint>
#include <limits>
#include <stdexcept>


class ReversiblePRNG64
{
public:
    using u64 = std::uint64_t;
    using i64 = std::int64_t;

private:
    // Must be odd so multiplication has an inverse modulo 2^64.
    static constexpr u64 GAMMA = 0x9e3779b97f4a7c15ULL;

    // Multipliers used by the mixing function.
    static constexpr u64 MIX1 = 0xbf58476d1ce4e5b9ULL;

    static constexpr u64 MIX2 = 0x94d049bb133111ebULL;

    // Modular inverse of GAMMA mod 2^64.
    static constexpr u64 INV_GAMMA = 0xf1de83e19937733dULL;

    // Modular inverses of MIX1 and MIX2 mod 2^64.
    static constexpr u64 INV_MIX1 = 0x96de1b173f119089ULL;

    static constexpr u64 INV_MIX2 = 0x319642b2d24d8ec3ULL;

    u64 seed_;
    u64 index_;
    u64 state_;

    // ------------------------------------------------------------
    // Inverse of:
    //     x ^= x >> shift
    //
    // For xorshift-right, the inverse can be obtained by repeatedly
    // applying larger powers of the shift.
    // ------------------------------------------------------------
    static constexpr u64 unxorshift_right(u64 x, unsigned shift)
    {
        x ^= x >> shift;
        x ^= x >> (2 * shift);
        x ^= x >> (4 * shift);

        // These may shift by >= 64 for some generic shifts.
        // Our actual shifts are 27, 30, 31, so only the above
        // operations are needed. The extra operation is guarded.
        if (8 * shift < 64)
            x ^= x >> (8 * shift);

        return x;
    }

public:
    explicit constexpr ReversiblePRNG64(u64 seed = 0)
        : seed_(seed), index_(0), state_(seed)
    {
    }

    // ============================================================
    // Forward permutation
    // ============================================================

    static constexpr u64 mix(u64 x)
    {
        x ^= x >> 30;
        x *= MIX1;

        x ^= x >> 27;
        x *= MIX2;

        x ^= x >> 31;

        return x;
    }

    // ============================================================
    // Reverse permutation
    //
    // This exactly undoes mix().
    // ============================================================

    static constexpr u64 unmix(u64 x)
    {
        // Undo: x ^= x >> 31
        x ^= x >> 31;
        x ^= x >> 62;

        // Undo: x *= MIX2
        x *= INV_MIX2;

        // Undo: x ^= x >> 27
        x ^= x >> 27;
        x ^= x >> 54;

        // Undo: x *= MIX1
        x *= INV_MIX1;

        // Undo: x ^= x >> 30
        x ^= x >> 30;
        x ^= x >> 60;

        return x;
    }

    // ============================================================
    // State corresponding to sequence index i
    //
    //     state(i) = seed + i * GAMMA mod 2^64
    // ============================================================

    constexpr u64 state_at(u64 index) const
    {
        return seed_ + index * GAMMA;
    }

    // ============================================================
    // Random value at an arbitrary index.
    //
    // O(1), does not modify the generator.
    // ============================================================

    constexpr u64 at(u64 index) const
    {
        return mix(state_at(index));
    }

    // ============================================================
    // Current index
    // ============================================================

    constexpr u64 index() const
    {
        return index_;
    }

    // ============================================================
    // Current raw state.
    //
    // This is the state used for the current index.
    // ============================================================

    constexpr u64 state() const
    {
        return state_at(index_);
    }

    // ============================================================
    // next()
    //
    // Returns value at current index and advances by one.
    // ============================================================

    constexpr u64 next()
    {
        u64 value = mix(state_);
        state_ += GAMMA;
        ++index_;
        return value;
    }

    // ============================================================
    // previous()
    //
    // Moves backward one step and returns that value.
    // ============================================================

    constexpr u64 previous()
    {
        --index_;
        state_ -= GAMMA;
        return mix(state_);
    }

public:
    // ============================================================
    // Jump forward/backward by n positions.
    //
    // O(1), regardless of n.
    //
    // Because arithmetic is unsigned, overflow is intentional and
    // equivalent to modulo 2^64.
    // ============================================================

    constexpr void jump(i64 distance)
    {
        if (distance >= 0)
        {
            const u64 d = static_cast<u64>(distance);
            index_ += d;
            state_ += d * GAMMA;
        }
        else
        {
            // Avoid undefined behavior from negating INT64_MIN.
            const u64 d = static_cast<u64>(-(distance + 1)) + 1ULL;

            index_ -= d;
            state_ -= d * GAMMA;
        }
    }

    constexpr void jump_forward(u64 distance)
    {
        index_ += distance;
        state_ += distance * GAMMA;
    }

    constexpr void jump_backward(u64 distance)
    {
        index_ -= distance;
        state_ -= distance * GAMMA;
    }

    // ============================================================
    // Reset
    // ============================================================

    constexpr void reset()
    {
        index_ = 0;
        state_ = seed_;
    }

    constexpr void reset(u64 seed)
    {
        seed_ = seed;
        index_ = 0;
        state_ = seed;
    }

    // ============================================================
    // Seed
    // ============================================================

    constexpr u64 seed() const
    {
        return seed_;
    }

    // ============================================================
    // Recover raw state from a random output.
    //
    // Since mix() is a permutation:
    //
    //     state = unmix(output)
    //
    // O(1).
    // ============================================================

    static constexpr u64 state_from_value(u64 value)
    {
        return unmix(value);
    }

    // ============================================================
    // Recover sequence index from a random output.
    //
    // Given:
    //
    //     value = mix(seed + index * GAMMA)
    //
    // we first undo mix(), then solve for index.
    //
    //     index = (state - seed) * inverse(GAMMA) mod 2^64
    //
    // O(1).
    // ============================================================

    constexpr u64 index_from_value(u64 value) const
    {
        const u64 state = unmix(value);
        return (state - seed_) * INV_GAMMA;
    }

    // ============================================================
    // Recover sequence index from a raw state.
    // ============================================================

    constexpr u64 index_from_state(u64 state) const
    {
        return (state - seed_) * INV_GAMMA;
    }

    // ============================================================
    // Given a random output, find the previous output.
    //
    // O(1).
    // ============================================================

    static constexpr u64 previous_value(u64 value)
    {
        u64 state = unmix(value);
        state -= GAMMA;
        return mix(state);
    }

    // ============================================================
    // Given a random output, find the next output.
    //
    // O(1).
    // ============================================================

    static constexpr u64 next_value(u64 value)
    {
        u64 state = unmix(value);
        state += GAMMA;
        return mix(state);
    }

    // ============================================================
    // Get value from a raw state.
    // ============================================================

    static constexpr u64 value_from_state(u64 state)
    {
        return mix(state);
    }

    // ============================================================
    // Get raw state from index.
    // ============================================================

    constexpr u64 state_from_index(u64 index) const
    {
        return state_at(index);
    }
};
