#include "Deck.h"
// Implementation File for Deck.h

// ===== Shuffle Stat Tests =====

// Note: See Report.h for how each result is computed

// domain-specific entropy analysis tool

// ===== Position Frequency (Uniformity) =====

// Question Answered: “Does each card appear equally often in each position?”
// Test Used: Chi-Squared, Data: Position Frequency Matrix
void DeckContext::observe_uniformity() noexcept {
    for (int i = 0; i < DECK_SIZE; ++i) {
        Card card = deck[i];
        posFreq[card][i]++;
    }
}

// ===== Card-to-Card Adjacency (Local Entropy) =====

// Questioned Answered: “Do certain cards tend to stay next to each other?”
// Test Used: Chi-Squared, Data: Adjacency Frequency Matrix
void DeckContext::observe_adjacency() noexcept {
    for (int i = 0; i < DECK_SIZE - 1; ++i) {
        Card card = deck[i];
        Card follower = deck[i+1];
        adjFreq[card][follower]++;
    }
}

// ===== Displacement (Mixing Speed) =====

// Questioned Answered: “How far do cards move from their original positions?”
// Test Used: Mean Displacement, Data: Displacement Histogram
void DeckContext::observe_displacement() noexcept {
    for (int pos = 0; pos < DECK_SIZE; ++pos) {
        Card card = deck[pos];
        int d = std::abs(pos - card);
        ++dispHist[d];
    }
}


// ===== WIP =====

// Runs / Increasing Subsequences (Residual Order)
// Run length counts

// Permutation Parity / Cycle Structure (Deep Structure)
// Parity count + cycle hist 