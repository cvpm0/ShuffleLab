#include "ExperimentRunner.h"
#include "UI.h"


ExperimentRunner::ExperimentRunner(const ExperimentConfig& cfg) : cfg(cfg), allowed{Shuffle::Cut, Shuffle::Riffle, Shuffle::Hindu, Shuffle::Overhand} {}

void ExperimentRunner::apply_shuffle(DeckContext& ctx, Shuffle s) {
    ++ctx.numShuffles;
    switch (s) {
        case Shuffle::RandomTest:
            ctx.random_test_shuffle(); break;
        case Shuffle::Riffle:
            ctx.riffle(); break;
        case Shuffle::Hindu:
            ctx.hindu(); break;
        case Shuffle::Overhand:
            ctx.overhand(); break;
        case Shuffle::Cut:
            ctx.cut(); break;
    }
}

// Enumerate all shuffle sequences
bool ExperimentRunner::next_sequence(std::vector<int>& idx, int base) {
    for (int i = static_cast<int>(idx.size()) - 1; i >= 0; --i) {
        if (++idx[i] < base) return true;
        idx[i] = 0;
    }
    return false;
}

double ExperimentRunner::score(double seqMeanUniformity,
             double seqMeanAdjacency,
             double seqMeanDisplacement)
{
    // Expected values (theoretical / emprirical baseline) 
    constexpr double MEAN_UNIFORMITY_TARGET = 51.0;
    constexpr double MEAN_ADJACENCY_TARGET = 50.0;
    constexpr double MEAN_DISPLACEMENT_TARGET = 17.33;

    // Inverse standard deviation used to convert deviation into z-score.
    // For chi-square distribution: StdDev = sqrt(2·df), so InvStdDev = 1 / sqrt(2·df).

    constexpr double UNIFORMITY_INV_STDDEV = 0.099015; // df = 51 → StdDev = sqrt(102) ≈ 10.0995 → Inv ≈ 0.0990147543

    constexpr double ADJACENCY_INV_STDDEV = 0.1;                  // df = 50 → StdDev = sqrt(100) = 10 → Inv = 0.1

    // Displacement is not chi-square distributed. For uniform permutation of 52 cards:
    // Expected mean ≈ 17.3269, empirical StdDev ≈ 3 → InvStdDev ≈ 1/3.
    constexpr double DISPLACEMENT_INV_STDDEV = 0.333333;

    constexpr double W_UNIFORMITY = 0.25;
    constexpr double W_ADJACENCY = 0.7;    
    constexpr double W_DISPLACEMENT = 0.05;

    

    double score = 0.0; // lower = less deviation / closer to expected value
    double weightSum = 0.0;

    // NOTE: INfuture could consider more complex scoring function - add a filter before optimisation
    // Ex. if (meanDisp < 0.8 * DISPLACEMENT_TARGET) displacementPenalty += 5.0;


    // Absoloute deviation from ideal
    if (seqMeanUniformity != -1) {
        double z = (seqMeanUniformity - MEAN_UNIFORMITY_TARGET) * UNIFORMITY_INV_STDDEV;
        score += W_UNIFORMITY * z * z;
        weightSum += W_UNIFORMITY;
    }

    if (seqMeanAdjacency != -1) {
        double z = (seqMeanAdjacency - MEAN_ADJACENCY_TARGET) * ADJACENCY_INV_STDDEV;
        score += W_ADJACENCY * z * z;
        weightSum += W_ADJACENCY;
    }

    if (seqMeanDisplacement != -1) {
        double z = (seqMeanDisplacement - MEAN_DISPLACEMENT_TARGET) * DISPLACEMENT_INV_STDDEV;
        score += W_DISPLACEMENT * z * z;
        weightSum += W_DISPLACEMENT;
    }

    // Normalise so socre comparable if tests are disabled
    return (weightSum > 0.0) ? score / weightSum : 0.0;

}

void ExperimentRunner::run() {
    

    print_experiment_overview(cfg, allowed.size());

    const int base = static_cast<int>(allowed.size());

    std::vector<int> bestSeqIdx;  
    // radix enumerator needs to be altered to be compatible when allowed != Shuffle Enum
    DeckContext bestShuffledDeck;
    double bestScore = std::numeric_limits<double>::infinity();

    // Compute t trials for n^k sequences of size k (n = # unique shuffle types)
    //for (int k = 1; k <= cfg.kMax; ++k) {
    int k = cfg.kMax;
        
        std::vector<int> idx(k, 0);
        bool hasNext = true;

        while (hasNext) {

            // Aggregate Stat Initialisation
            double seqMeanUniformity = cfg.testUniformity ? 0 : -1;
            double seqMeanAdjacency = cfg.testAdjacency ? 0 : -1;
            double seqMeanDisplacement = cfg.testMixing ? 0 : -1;

            DeckContext ctx; // better to use reset function?

            for (int t = 0; t < cfg.trials; ++t) {
                
                ctx.reset(); // sorts deck
                
                for (int step = 0; step < k; ++step) {
                    apply_shuffle(ctx, allowed[idx[step]]);
                }

                // Update relevant stats
                if (cfg.testAdjacency)  ctx.observe_adjacency();
                if (cfg.testUniformity) ctx.observe_uniformity();
                if (cfg.testMixing)     ctx.observe_displacement();
            }

            // Aggregate statistical data across trials
            if (cfg.testUniformity) {
                seqMeanUniformity = report_uniformity(ctx).meanChiSq;
            }

            if (cfg.testAdjacency) {
                seqMeanAdjacency = report_adjacency(ctx).meanChiSq;
            }

            if (cfg.testMixing) {
                seqMeanDisplacement = report_displacement(ctx).mean;
            }


            // Update best sequence
            double seqScore = score(seqMeanUniformity, seqMeanAdjacency, seqMeanDisplacement); // NEED TO NORMALISE
            if (seqScore < bestScore) {
                bestShuffledDeck = ctx;
                bestSeqIdx = idx;
                bestScore = seqScore;
            }

            hasNext = next_sequence(idx, base);
        }
    //}

    print_experiment_results(cfg, bestShuffledDeck, bestSeqIdx, allowed.size());
}


