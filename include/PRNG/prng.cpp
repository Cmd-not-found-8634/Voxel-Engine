#include <iostream>
#include <iomanip>

#include <PRNG/prng.hpp>S

int main()
{
    ReversiblePRNG64 rng(123456789ULL);

    std::cout << rng.next() << '\n';
    std::cout << rng.next() << '\n';
    std::cout << rng.next() << '\n';

    // Jump forward by one million values.
    rng.jump_forward(1'000'000);

    std::cout << rng.next() << '\n';

    // Go backwards.
    std::cout << rng.previous() << '\n';

    // Random access without changing RNG state.
    std::cout << rng.at(123456789ULL) << '\n';
}