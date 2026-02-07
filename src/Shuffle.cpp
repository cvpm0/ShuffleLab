#include "Deck.h"
// Implementation File for Deck.h

// ===== Human Shuffles =====

// Simple Cut (Custom)
void DeckContext::cut() noexcept {
    static auto cut_cdf = []{ // std::exp not constexpr
        return create_cdf(5, 47, 26, 5); 
    }();

    uint8_t cutPoint = rng.sample_cdf(cut_cdf);

    perfect_cut(cutPoint);

}

// GSR Riffle Model
void DeckContext::riffle() noexcept {
    static auto riffle_cdf = []{ // used for getting two packets
        return create_cdf(12, 40, 26, 3.6); // Binomial approximation for now
    }();

    uint8_t cutPoint = rng.sample_cdf(riffle_cdf); // split deck into two packets

    // packet 1 (L) Deck [0, cutPoint), packet 2 (R) Deck [cutPoint, DECK_SIZE)
    int L = cutPoint, R = DECK_SIZE - cutPoint; // num card left in each packet

    // P(left)  = L / (L + R), P(right) = R / (L + R)

    int leftIdx = L - 1, rightIdx = L + R - 1;

    // interleave packets onto buffer
    for (int i = DECK_SIZE - 1; i >= 0; --i) {

        if (L == 0) {
            buffer[i] = deck[rightIdx--];
            --R;
        } else if (R == 0) {
            buffer[i] = deck[leftIdx--];
            --L;
        } else {
            // int method used to avoid float arithmetic
            int rand = rng.random_bounded(L + R);
            if (rand < L) {
                buffer[i] = deck[leftIdx--];
                --L;
            } else {
                buffer[i] = deck[rightIdx--];
                --R;
            }
        }
    }
    deck = buffer;

}

// Hindu Shuffle (Custom)
void DeckContext::hindu() noexcept {
    // determines number of packet pickups per shuffle
    static auto hindu_num_ops = []{ // more variance shuffle-to-shuffle -> more trials
        return create_cdf(1, 5, 2, 1.2);
    }();

    // determines number of cards taken off of top for every removal //larrger more random cut
    static auto hindu_cut_cdf = []{
        return create_cdf(20, 50, 35, 9); // to be observed
    }();

    // determines num cards dropped from top of packet at a time
    static auto hindu_drop_cdf = []{
        return create_cdf(2, 10, 5, 2.5); // to be observed - as defined gives count not idx
    }();


    auto numOps = rng.sample_cdf(hindu_num_ops);

    buffer = deck; // subsequent operations guarantee this condition afterwards
    for (int i = 0; i < numOps; ++i) {
        
        int cutPoint = rng.sample_cdf(hindu_cut_cdf); // idx of bottom of packet

        int n = cutPoint; // buffer ptr

        int dropped = 0; // running total of amount dropped

        buffer = deck;

        while (n >= 0) {
            
            int dropCount = rng.sample_cdf(hindu_drop_cdf); // always > 0
            int dropPoint = std::min(cutPoint, dropped + dropCount - 1); // bottom card of sub-packet idx


            
            // drop from top of packet on to top of deck
            for (int d = dropPoint; d >= dropped; --d) {
                buffer[n--] = deck[d];
            }

            dropped = dropPoint + 1; // can't use dropCount directly in case n used
            
        }

        deck = buffer;

    }

    // NOTE: when using timing contraints each indiviual hindu cut and place/drop should be considered its own shuffle as timings vary greatly
}

// Overhand Shuffle (Custom)
inline void DeckContext::overhand() noexcept {
    // determines number of cards taken off of top for every removal //larrger more random cut
    static auto overhand_cut_cdf = []{
        return create_cdf(20, 26, 31, 4); // to be observed
    }();

    // determines num cards dropped from top of packet at a time
    static auto overhand_drop_cdf = []{
        return create_cdf(2, 10, 5, 2.5); // to be observed - as defined gives count not idx
    }();

    // take packet from bottom [0, cutPoint)
    int cutPoint = rng.sample_cdf(overhand_cut_cdf);

    buffer = deck; // could maybe be optimised by only copying necessary cards

    int n = cutPoint; // write cursor
    int dropped = 0; // cards consumed from packet

    while (n >= 0) {
            
            int dropCount = rng.sample_cdf(overhand_drop_cdf); // > 0
            int dropPoint = std::min(cutPoint, dropped + dropCount - 1); // bottom card of sub-packet idx


            
            // drop from top of packet on to top of deck
            for (int d = dropPoint; d >= dropped; --d) {
                buffer[n--] = deck[d];
            }

            dropped = dropPoint + 1;
            
        }

        deck = buffer;

} 

// center cut shuffle ()
// numOps and two cut points second roughly dependant on first - experiment

// Fisher Yates Baseline - For Testing
void DeckContext::random_test_shuffle() noexcept {
    for (int i = DECK_SIZE - 1; i > 0; --i) {
        int j = rng.random_bounded(DECK_SIZE);  // inclusive
        std::swap(deck[i], deck[j]);
    }
}

// ===== Perfect Shuffles =====


// Perfect Cut (half by default)
inline void DeckContext::perfect_cut(uint8_t cutPoint) noexcept {

    int n = 0;

    for (int i = cutPoint; i < DECK_SIZE; ++i) { // cut point defined as index bottom of removed packet
        buffer[n++] = deck[i];
    }

    for (int i = 0; i < cutPoint; ++i) {
        buffer[n++] = deck[i];
    }

    deck = buffer;
}

// Perfect Riffle (Faro)
inline void DeckContext::perfect_riffle() noexcept {

    perfect_cut(); // buffer now holds cut;
    int n = 0; // buffer incrementor
    
    // odd positions
    for (int i = 1; i < DECK_SIZE; i += 2) {
        deck[i] = buffer[n++];
    }

    // even positions
    for (int i = 0; i < DECK_SIZE; i += 2) {
        deck[i] = buffer[n++];
    }
}

