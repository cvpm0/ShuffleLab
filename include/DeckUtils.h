#pragma once

#include "Deck.h"
#include <string>
#include <iostream>


// ===== Utilities =====

enum Suit : uint8_t {
    Club = 0,
    Diamond = 1,
    Heart = 2,
    Spade = 3
};

inline uint8_t get_rank(Card c) {
    return c % 13;
}

inline uint8_t get_suit(Card c) {
    return c / 13;
}

inline std::string card_to_string(Card c) {
    static constexpr const char* RANKS[] =
        {"A","2","3","4","5","6","7","8","9","T","J","Q","K"};
    static constexpr const char* SUITS[] =
        {"♣", "♦", "♥", "♠"};

    const char* colour =
        (get_suit(c) == 0) ? "\033[38;2;0;200;83m"   :   // ♣ bright green
        (get_suit(c) == 1) ? "\033[38;2;41;121;255m" :   // ♦ bright blue
        (get_suit(c) == 2) ? "\033[38;2;213;0;0m"    :   // ♥ bright red
                        "\033[38;2;170;0;255m";      // ♠ purple

    return std::string(colour)
        + RANKS[get_rank(c)]
        + SUITS[get_suit(c)]
        + "\033[0m"; // reset colour
}

inline void print_deck_rows(const DeckContext& ctx, size_t per_row = 13) { // suit or rank first?
    const Deck& d = ctx.deck;
    for (size_t i = 0; i < d.size(); ++i) {
        std::cout << card_to_string(d[i]) << ' ';
        if ((i + 1) % per_row == 0) std::cout << '\n';
    }
    if (d.size() % per_row != 0) std::cout << '\n';
}


enum class Shuffle : int {
    RandomTest,
    Cut,
    Riffle,
    Hindu,
    Overhand
};

constexpr std::string_view to_string(Shuffle s) {
    switch (s) {
        case Shuffle::Cut:        return "Cut";
        case Shuffle::Riffle:     return "Riffle";
        case Shuffle::Hindu:      return "Hindu";
        case Shuffle::Overhand:   return "Overhand";
        case Shuffle::RandomTest: return "RandomTest";
    }
    return "UNKNOWN";
}