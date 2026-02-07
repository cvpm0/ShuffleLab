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
            ctx.hindu(); break;
        case Shuffle::Cut:
            ctx.hindu(); break;
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

    constexpr double W_UNIFORMITY = 0.65;
    constexpr double W_ADJACENCY = 0.2;    
    constexpr double W_DISPLACEMENT = 0.15;

    double score = 0.0; // lower = less deviation / closer to expected value
    double weightSum = 0.0;

    // NOTE: INfuture could consider more complex scoring function - add a filter before optimisation
    // Ex. if (meanDisp < 0.8 * DISPLACEMENT_TARGET) displacementPenalty += 5.0;


    // Absoloute deviation from ideal
    if (seqMeanUniformity != -1) {
        double p = std::abs(seqMeanUniformity - MEAN_UNIFORMITY_TARGET);
        score += W_UNIFORMITY * p;
        weightSum += W_UNIFORMITY;
    }

    if (seqMeanAdjacency != -1) {
        double p = std::abs(seqMeanAdjacency - MEAN_ADJACENCY_TARGET);
        score += W_ADJACENCY * p;
        weightSum += W_ADJACENCY;
    }

    if (seqMeanDisplacement != -1) {
        double p = std::abs(seqMeanDisplacement - MEAN_DISPLACEMENT_TARGET);
        score += W_DISPLACEMENT * p;
        weightSum += W_DISPLACEMENT;
    }

    // Normalise so socre comparable if tests are disabled
    return (weightSum > 0.0) ? score / weightSum : 0.0;

}

void ExperimentRunner::run() {
    

    print_experiment_overview(cfg, allowed.size());

    const int base = static_cast<int>(allowed.size());

    std::vector<int> bestSeqIdx(cfg.kMax);  
    // radix enumerator needs to be altered to be compatible when allowed != Shuffle Enum
    DeckContext bestShuffledDeck;
    double bestScore = std::numeric_limits<double>::infinity();

    // Compute t trials for n^k sequences of size k (n = # unique shuffle types)
    for (int k = 1; k <= cfg.kMax; ++k) {
        
        std::vector<int> idx(k, 0);
        bool hasNext = true;

        while (hasNext) {

            // Aggregate Stat Initialisation
            double seqMeanUniformity = cfg.testUniformity ? 0 : -1;
            double seqMeanAdjacency = cfg.testAdjacency ? 0 : -1;
            double seqMeanDisplacement = cfg.testMixing ? 0 : -1;

            DeckContext ctx; // better to use reset function?

            for (int t = 0; t < cfg.trials; ++t) {
                
                ctx.reset();
                
                for (int step = 0; step < k; ++step) {
                    apply_shuffle(ctx, allowed[idx[step]]);

                    // Update relevant stats
                    if (cfg.testUniformity) ctx.observe_uniformity();
                    if (cfg.testAdjacency)  ctx.observe_adjacency();
                    if (cfg.testMixing)     ctx.observe_displacement();
                }

                // Extracting only key metrics for now
                if (cfg.testUniformity) {
                    auto report = report_uniformity(ctx);
                    seqMeanUniformity += (report.meanChiSq - seqMeanUniformity) / (t + 1);
                } 
                if (cfg.testAdjacency) {
                    auto report = report_adjacency(ctx);
                    seqMeanAdjacency += (report.meanChiSq - seqMeanAdjacency) / (t + 1);
                } 
                if (cfg.testMixing) {
                    auto report = report_displacement(ctx);
                    seqMeanDisplacement += (report.mean - seqMeanDisplacement) / (t + 1);
                } 


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
    }

    print_experiment_results(cfg, bestShuffledDeck, bestSeqIdx, allowed.size());
}


