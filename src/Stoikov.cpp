#pragma once

#include <algorithm>
#include <vector>
#include <cmath>

#include "Inventory.cpp"

class Stoikov {
    private:
    std::vector<float> bid;
    std::vector<float> ask;

    Inventory& inventory;

    float vol;
    float risk_aversion;
    float liquidity;
    float duration;
    int steps;

    float current_bid;
    float current_ask;

    float current_bid_dist;
    float current_ask_dist;

public:
    //store constant variable for quicker retrieval
    Stoikov(
        float vol,
        float risk_aversion,
        float liquidity,
        float duration,
        int steps,
        Inventory& inventory
    )
        : inventory(inventory),
          vol(vol),
          risk_aversion(risk_aversion),
          liquidity(liquidity),
          duration(duration),
          steps(steps),
          current_bid(0.0f),
          current_ask(0.0f),
          current_bid_dist(0.0f),
          current_ask_dist(0.0f)
    {
    }
    void quotes(size_t i, float s) {
        // Time remaining, on the same grid the path was generated on
        float elapsed =
            duration
            * (static_cast<float>(i) / static_cast<float>(steps));

        float t = std::max(0.0f, duration - elapsed);

        // Avellaneda-Stoikov assumes arithmetic price moves, so sigma
        // has to be in price units. The path is GBM, where vol is a
        // relative rate, so scale it by the current price.
        float sigma = vol * s;
        float variance = sigma * sigma;

        // Reservation price
        float r =
            s -
            inventory.getQuantity()
            * risk_aversion
            * variance
            * t;

        // Optimal total spread
        float spread =
            risk_aversion
            * variance
            * t
            +
            (2.0f / risk_aversion)
            * std::log(
                1.0f + risk_aversion / liquidity
            );

        float halfSpread = spread / 2.0f;

        // Quotes centred around reservation price
        float b = r - halfSpread;
        float a = r + halfSpread;

        current_bid = b;
        current_ask = a;

        current_bid_dist = s-b;
        current_ask_dist = a-s;

        bid.push_back(b);
        ask.push_back(a);

    }
    const std::vector<float>& getBid(){
        return bid;
    }
    const std::vector<float>& getAsk() {
        return ask;
    }
    void setParams(float vol_, float risk_aversion_, float liquidity_, float duration_, int steps_) {
        vol = vol_;
        risk_aversion = risk_aversion_;
        liquidity = liquidity_;
        duration = duration_;
        steps = steps_;
    }

    void reset() {
        bid.clear();
        ask.clear();

        current_bid = 0.0f;
        current_ask = 0.0f;
        current_bid_dist = 0.0f;
        current_ask_dist = 0.0f;
    }

    float getBidDist() {
        return current_bid_dist;
    }
    float getAskDist() {
        return current_ask_dist;
    }
    float currentBid() {
        return current_bid;
    }
    float currentAsk() {
        return current_ask;
    }
};
