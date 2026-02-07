#pragma once

#include <algorithm> // std::max
#include "Deck.h"
#include "DeckConstants.h"

// Could be migrated into Deck.h

struct UniformityReport {
    double maxChiSq = 0;
    double meanChiSq = 0;
    std::array<double, DECK_SIZE> chiSqCard{};
};
UniformityReport report_uniformity(const DeckContext& ctx) ;
void print_report(const UniformityReport& r);


struct AdjacencyReport {
    double maxChiSq = 0;
    double meanChiSq = 0;
    std::array<double, DECK_SIZE> chiSqCard{};
};
AdjacencyReport report_adjacency(const DeckContext& ctx);
void print_report(const AdjacencyReport& r);


struct DisplacementReport {
    double mean = 0;
};
DisplacementReport report_displacement(const DeckContext& ctx);

void print_report(const DisplacementReport& r);