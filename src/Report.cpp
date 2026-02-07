#include "Report.h"

UniformityReport report_uniformity(const DeckContext& ctx) {
    UniformityReport report;

    if (ctx.numShuffles == 0) return report;

    double E = static_cast<double>(ctx.numShuffles) / DECK_SIZE; // Expected freq

    double sum = 0;

    for (int card = 0; card < DECK_SIZE; ++card) {
        double chiSq = 0; // per card

        for (int pos = 0; pos < DECK_SIZE; ++pos) {
            double dev = ctx.posFreq[card][pos] - E; // deviation
            chiSq += (dev * dev) / E;
        }

        report.chiSqCard[card] = chiSq;
        report.maxChiSq = std::max(report.maxChiSq, chiSq);
        sum += chiSq;
    }
    report.meanChiSq = sum / DECK_SIZE;
    
    return report;
}

void print_report(const UniformityReport& r) { // overloaded
    std::cout << "[Uniformity — Chi-Squared]\n";
    std::cout << "  Mean χ² : " << r.meanChiSq << "\n";
    // std::cout << "  Max  χ² : " << r.maxChiSq << "\n"; SIMPLIFIED
    std::cout << "  Expected χ² ≈ " << (DECK_SIZE - 1) << "\n\n";
}

// For one card:
// Number of categories = 52
// Degrees of freedom = 51

// Rough intuition:
// χ2 ≈ 51 → looks uniform
// χ2 ≪ 51 → extremely uniform (often small N)
// χ2 ≫5 1 → bias / structure

AdjacencyReport report_adjacency(const DeckContext& ctx) {
    AdjacencyReport report;

    if (ctx.numShuffles == 0) return report;

    double sum = 0;

    for (int card = 0; card < DECK_SIZE; ++card) {

        // total adj observations for this card
        double rowSum = 0;
        for (int flwr = 0; flwr < DECK_SIZE; ++flwr) {
            if (flwr == card) continue;
            rowSum += ctx.adjFreq[card][flwr];
        }
        if (rowSum == 0) continue; // safety for tiny samples

        double E = rowSum / (DECK_SIZE - 1); // expected per follower
        double chiSq = 0;

        for (int flwr = 0; flwr < DECK_SIZE; ++flwr) {
            if (flwr == card) continue;

            double dev = ctx.adjFreq[card][flwr] - E;
            chiSq += (dev * dev) / E;
        }

        report.chiSqCard[card] = chiSq;
        report.maxChiSq = std::max(report.maxChiSq, chiSq);
        sum += chiSq;
    }

    report.meanChiSq = sum / DECK_SIZE;
    
    return report;
}

void print_report(const AdjacencyReport& r) {
    std::cout << "[Adjacency — Chi-Squared]\n";
    std::cout << "  Mean χ² : " << r.meanChiSq << "\n";
    // std::cout << "  Max  χ² : " << r.maxChiSq << "\n"; SIMPLIFIED
    std::cout << "  Expected χ² ≈ " << (DECK_SIZE - 2) << "\n\n";
}

DisplacementReport report_displacement(const DeckContext& ctx) {
    DisplacementReport report;

    if (ctx.numShuffles == 0) return report;
    
    int total = 0;
    long long weightedSum = 0;

    for (int d = 0; d < DECK_SIZE; ++d) {
        int count = ctx.dispHist[d];
        total += count;
        weightedSum += static_cast<long long>(d) * count;

    }

    if (total > 0) {
        report.mean = static_cast<double>(weightedSum) / total;
    }
    
    return report;
}

void print_report(const DisplacementReport& r) {
    std::cout << "[Mixing Speed — Displacement]\n";
    std::cout << "  Mean : " << r.mean << "\n";
    std::cout << "  Expected ≈ 17.33\n\n";
}
