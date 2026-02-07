#include "UI.h"

void print_logo() {
    std::cout <<
R"(

              ♠  S H U F F L E L A B  ♠

                       — Created by Cooper Marshall


)";
}

void print_experiment_overview(const ExperimentRunner::ExperimentConfig& cfg, int numShufflesAllowed) {
    std::size_t numSequences = 1;
  for (int i = 0; i < cfg.kMax; ++i) {
    numSequences *= numShufflesAllowed;
  }
    std::cout << "\n";
    std::cout << "ShuffleLab — Randomness Analysis\n";
    std::cout << "--------------------------------\n";
    std::cout << "Evaluating " << numSequences << " sequences\n";
    std::cout << "Shuffles per sequence : " << cfg.kMax << "\n";
    std::cout << "Trials                : " << cfg.trials << "\n";
    std::cout << "Tests                 : ";

    bool first = true;
    if (cfg.testUniformity) {
        std::cout << (first ? "" : ", ") << "Uniformity";
        first = false;
    }
    if (cfg.testAdjacency) {
        std::cout << (first ? "" : ", ") << "Adjacency";
        first = false;
    }
    if (cfg.testMixing) {
        std::cout << (first ? "" : ", ") << "Mixing";
        first = false;
    }

    if (first) {
        std::cout << "None";
    }

}

void print_experiment_results(const ExperimentRunner::ExperimentConfig& cfg, const DeckContext& ctx, const std::vector<int>& bestShuffleSeqIdx, int numShufflesAllowed) {
  auto shuffleSeq = shuffleIdx_to_string(bestShuffleSeqIdx);

  
    
    std::cout << "\n\n";

    std::cout << "Best-performing shuffle sequence:\n  ";

    for (std::size_t i = 0; i < shuffleSeq.size(); ++i) {
        if (i > 0)
            std::cout << " \u2192 "; // Unicode arrow →
        std::cout << shuffleSeq[i];
    }

    std::cout << "\n\n";

    print_report(report_uniformity(ctx));
    print_report(report_adjacency(ctx));
    print_report(report_displacement(ctx));

    std::cout << "Example Before and After of Shuffle Sequence on Sorted Deck:\n\n";
    DeckContext example;
    print_deck_rows(example);
    std::cout << "\n";
    print_deck_rows(ctx);
    std::cout << "\n";
    

    std::cout << "Done.\n";
}



void print_help() {
    std::cout <<
R"(
                  ShuffleLab — Help
               -----------------------

USAGE:
  shufflelab [mode] [options]

MODES (choose one):
  --run        Run a shuffle experiment
  --help       Show this help message
  --desc       Describe ShuffleLab and its goals

RUN OPTIONS:
  --k <int>        Maximum shuffle sequence length
  --trials <int>   Trials per shuffle sequence

TEST SELECTION:
  --uniformity     Enable position uniformity test (chi-squared)
  --adjacency      Enable card adjacency test (chi-squared)
  --mixing         Enable displacement / mixing test

DEFAULT BEHAVIOUR:
  Running with --run and no additional options uses a recommended
  configuration suitable for exploration and comparison.

EXAMPLES:
  shufflelab
  shufflelab --run
  shufflelab --run --k 6 --trials 20000
  shufflelab --desc

)";
}

void print_desc() {
    std::cout <<
R"(
                ShuffleLab — Description
              ----------------------------

ShuffleLab is an experimental tool for analysing the randomness of
card shuffling strategies.

It models common human shuffles (such as cuts, riffles, and overhand
shuffles) using custom probability distributions rather than idealised
uniform permutations. This allows ShuffleLab to study how real-world
shuffling behaviour differs from theoretical randomness.

Shuffle sequences are evaluated using statistical tests that measure:
  • Position uniformity
  • Local card adjacency
  • Mixing behaviour over repeated shuffles

Rather than simulating a single shuffle, ShuffleLab compares entire
shuffle sequences of bounded length and ranks them by statistical quality.
The maximum sequence length is capped to reflect realistic time and
attention constraints, and to enable practical comparison within a
finite search space.

The tool is designed to explore how small numbers of human-performed
shuffles affect randomness in settings where decks are reused and
re-shuffled repeatedly, such as self-dealt tournament poker.

ShuffleLab prioritises quantitative rigor, repeatability, and clear
comparisons over visual simulation.

)";
}


// Helper to convert from seq idx list to list of shuffle names
std::vector<std::string> shuffleIdx_to_string(const std::vector<int>& idx) { // move to UI.h?
    const std::vector<std::string> names = {
        "Cut",
        "Riffle",
        "Hindu",
        "Overhand"
    };

    std::vector<std::string> result;
    result.reserve(idx.size());

    for (int i : idx) {
        if (i < 0 || i >= static_cast<int>(names.size())) {
            result.emplace_back("UNKNOWN");
        } else {
            result.emplace_back(names[i]);
        }
    }

    return result;
};