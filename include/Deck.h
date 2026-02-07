#pragma once

#include <array>
#include <cstdint>
#include "DeckConstants.h"

#include "Random.h" // PCG32


// ===== Definitions =====

using Card = uint8_t;
using Deck = std::array<Card, DECK_SIZE>; // convention: deck[0] is top
static constexpr Deck CANONICAL_DECK = []{
    Deck d{};
    for (int i = 0; i < DECK_SIZE; ++i) {
        d[i] = i;
    }
    return d;
}();


// ===== Deck and Functions =====

struct DeckContext { // is DeckState more accurate?
    Deck deck = CANONICAL_DECK;
    Deck buffer{};
    PCG32 rng; 
    int numShuffles = 0;


    inline void reset() { deck = CANONICAL_DECK; }   
   
    // Human Shuffles (Using RNG)
    void cut() noexcept;
    void riffle() noexcept;
    void hindu() noexcept;
    void overhand() noexcept;
    void random_test_shuffle() noexcept; // for testing stats

    // Perfect Shuffles (Deterministic)
    void perfect_cut(uint8_t cutPoint = DECK_SIZE / 2) noexcept;
    void perfect_riffle() noexcept;

    // Shuffle Statistics (Updates - hot)
    void observe_uniformity() noexcept;
    std::array<std::array<int, DECK_SIZE>, DECK_SIZE> posFreq{}; // (pos, card ID)

    void observe_adjacency() noexcept;
    std::array<std::array<int, DECK_SIZE>, DECK_SIZE> adjFreq{}; // (cardID, followerID)
    
    void observe_displacement() noexcept;
    std::array<int, DECK_SIZE> dispHist{};
};

// ===== Implementations =====

// Shuffle.cpp
// Stats.cpp
