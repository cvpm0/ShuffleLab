#pragma once

#include "Deck.h"
#include "DeckUtils.h"
#include "ExperimentRunner.h"
#include "Report.h"

#include <iostream>
#include <vector>

// ===== Output =====

void print_logo();

void print_experiment_overview(const ExperimentRunner::ExperimentConfig& cfg, int numShufflesAllowed);

void print_experiment_results(const ExperimentRunner::ExperimentConfig& cfg, const DeckContext& ctx, const std::vector<int>& bestShuffleSeqIdx, int numShufflesAllowed);

void print_help();

void print_desc();

std::vector<std::string> shuffleIdx_to_string(const std::vector<int>& idx);
