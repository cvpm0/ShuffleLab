#include <iostream>
#include <string>
#include <cstring>   // strcmp
#include <cstdlib>   // std::stoi

#include "Deck.h"
#include "DeckUtils.h"
#include "Report.h"
#include "UI.h"
#include "ExperimentRunner.h"


// Error Helper
static int error(const std::string& msg) {
    std::cerr << "error: " << msg << "\n";
    std::cerr << "use --help for usage\n";
    return 1;
}

int main(int argc, char** argv) {

    // ----- Mode Flags -----
    bool wantHelp = false;
    bool wantDesc = false;
    bool wantRun = false;

    // Track whether any experiment-related flag was seen
    bool sawExperimentFlag = false;

    // ----- Experiment configuration -----
    ExperimentRunner::ExperimentConfig cfg;
    // default run
    cfg.kMax = 5;
    cfg.trials = 50;
    cfg.testUniformity = true;
    cfg.testAdjacency  = true;
    cfg.testMixing     = true;

    // ----- Parse arguments -----
    for (int i = 1; i < argc; ++i) {

        // ---- Modes ----
        if (std::strcmp(argv[i], "--help") == 0) {
            wantHelp = true;
        }
        else if (std::strcmp(argv[i], "--desc") == 0) {
            wantDesc = true;
        }
        else if (std::strcmp(argv[i], "--run") == 0) {
            wantRun = true;
        }

        // ---- Experiment parameters ----
        else if (std::strcmp(argv[i], "--k") == 0) {
            if (i + 1 >= argc)
                return error("--k requires an integer value");

            int k = std::stoi(argv[++i]);

            if (k < ExperimentRunner::K_MIN || k > ExperimentRunner::K_MAX) {
                return error(
                    "k must be between " +
                    std::to_string(ExperimentRunner::K_MIN) +
                    "and" +
                    std::to_string(ExperimentRunner::K_MAX)
                );
            }

            sawExperimentFlag = true;
            cfg.kMax = k;
        }
        else if (std::strcmp(argv[i], "--trials") == 0) {
            if (i + 1 >= argc)
                return error("--trials requires an integer value");
            sawExperimentFlag = true;
    
            int trial = std::stoi(argv[++i]);

            if (trial < 1 || trial > ExperimentRunner::TRIAL_MAX) {
                return error(
                    "trials must be between 0 and" +
                    std::to_string(ExperimentRunner::TRIAL_MAX)
                );
            }

            cfg.trials = trial;
        }

        // ---- Test toggles ----
        else if (std::strcmp(argv[i], "--uniformity") == 0) {
            sawExperimentFlag = true;
            cfg.testUniformity = true;
        }
        else if (std::strcmp(argv[i], "--adjacency") == 0) {
            sawExperimentFlag = true;
            cfg.testAdjacency = true;
        }
        else if (std::strcmp(argv[i], "--mixing") == 0) {
            sawExperimentFlag = true;
            cfg.testMixing = true;
        }

        // ---- Unknown ----
        else {
            return error(std::string("unknown option: ") + argv[i]);
        }
    }

    // ----- Enforce mode exclusivity -----
    if ((wantHelp + wantDesc + wantRun) > 1) {
        return error("choose only one of --run, --help, or --desc");
    }

    if ((wantHelp || wantDesc) && sawExperimentFlag) {
        return error("--help and --desc cannot be combined with experiment flags");
    }

    if (!wantRun && sawExperimentFlag) {
        return error("experiment flags require --run");
    }

    // ----- Dispatch -----
    if (wantHelp) {
        print_help();
        return 0;
    }

    if (wantDesc) {
        print_desc();
        return 0;
    }

    if (!wantRun) {
        print_logo();
        std::cout << "Card shuffle analysis tool\n";
        std::cout << "Use --run to execute a default experiment\n";
        std::cout << "Use --help to see available options\n\n";
        return 0;
    }

    // ----- Run experiment -----
    // print_logo();
    ExperimentRunner runner(cfg);
    runner.run();

    return 0;
}

