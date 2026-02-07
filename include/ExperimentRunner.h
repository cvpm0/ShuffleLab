#pragma once

#include <vector>

#include "Deck.h"
#include "Report.h"
#include "DeckUtils.h"

class ExperimentRunner {
public:
    static constexpr int K_MIN = 1;
    static constexpr int K_MAX = 8;
    static constexpr int TRIAL_MAX = 100;
    struct ExperimentConfig {
        // configure in main to allow user specs
        int kMax;    // max shuffles per trial
        int trials; // trials per k

        bool testUniformity;
        bool testAdjacency;
        bool testMixing;

    };

    explicit ExperimentRunner(const ExperimentConfig& cfg);
    void run();

private:

    

    ExperimentConfig cfg;
    std::vector<Shuffle> allowed; // not yet configurable

    void apply_shuffle(DeckContext& ctx, Shuffle s);
    bool next_sequence(std::vector<int>& idx, int base);
    double score(double seqMeanUniformity, double seqMeanAdjacency, double seqMeanDisplacement);
};



