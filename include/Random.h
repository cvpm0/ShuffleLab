#pragma once

#include "Deck.h"
#include "DeckConstants.h"
#include <cstdint>
#include <random>
#include <iostream>

// reference: https://github.com/wjakob/pcg32 (unofficial) 

struct PCG32 {
    private:
        static constexpr uint64_t MULTIPLIER = 6364136223846793005ULL;

        uint64_t state;          // RNG internal state
        uint64_t increment;  // stream selector (must be odd)

        inline uint32_t next() noexcept {
            uint64_t old = state;
            state = old * MULTIPLIER + increment;

            uint32_t xorshifted = ((old >> 18u) ^ old) >> 27u;
            uint32_t rot = old >> 59u;
            return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
        } // reject bias sampling for uniform selection
        
    
    public:
        PCG32() {
            increment = 1; // fixed stream
            std::random_device rd;
            state = rd();
        }

        inline uint32_t random_bounded(uint32_t bound) noexcept { // can we guarantee uniform distribution on bound
            return next() % bound; // returns [0, bound)
        }

        // Discrete inverse-CDF sampling (used in hot loops)
        inline uint8_t sample_cdf(const std::array<uint16_t, DECK_SIZE>& cdf) {
            const int n = cdf.size();
            const int W = cdf[n - 1];
            const int R = random_bounded(W);

            for (int k = 0; k < n; ++k) {
                if (R < cdf[k]) return k;
            }
            return n - 1; // prevent UB - unreachable if CDF is valid
        }
        
};



// create distribution (Gaussian-simplified) (Cumulative Distribution Function) (P = [0, 1000]) over [0, DECK_SIZE)
constexpr std::array<uint16_t, DECK_SIZE> create_cdf( // truncation of weights introduces systematic bias in sampling (redistribution)
    int min, // [0, DECK_SIZE)
    int max, // [min, DECK_SIZE)
    int centre, // [min, max]
    double spread, // > 0, # SD
    bool visualiseWeights = false
) {
    constexpr int TOTAL_WEIGHT = 1000;
    std::array<double, DECK_SIZE> weights{};
    std::array<uint16_t, DECK_SIZE> cdf{};

    // compute raw Gaussian weights
    double sum = 0;
    for (int k = min; k < max; ++k) {
        double d = k - centre;
        weights[k] = std::exp(-(d * d)/(2 * spread * spread));
        sum += weights[k];
    }

    // scale to integer weights summing to ~TOTAL_WEIGHT
    double scale = TOTAL_WEIGHT / sum;

    // create CDF
    uint16_t running = 0;
    for (int k = 0; k < DECK_SIZE; ++k) {
        running += static_cast<uint16_t>(weights[k] * scale);
        cdf[k] = running;
    }

    // Optional Visualisation
    if (visualiseWeights) {

        std::cout << "Weights:\n\n";
        for (int i = 0; i < DECK_SIZE; ++i) {
            std::cout << "W(card " << i << ") ";
            if (i < 10) std::cout << " ";
            std::cout <<"|";
            int scaledWeight = weights[i] * 50;
            for (int j = 0; j < scaledWeight; ++j) {
                std::cout << "=";
            }
            std::cout << " (" << weights[i] << ")\n";
        }
    }

    return cdf;
};