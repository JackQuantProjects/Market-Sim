#include <vector>
#include <cmath>

class Stoikov {
    private:
    std::vector<float> bid;
    std::vector<float> ask;

    Inventory& inventory;

    float vol;
    float risk_aversion;
    float liquidity;
    float intensity;
    int duration;

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
        float intensity,
        float duration,
        Inventory& inventory
    )
        : inventory(inventory),
          vol(vol),
          risk_aversion(risk_aversion),
          liquidity(liquidity),
          intensity(intensity),
          duration(duration)
    {
    }
    void quotes(int i, float s) {
        // Time remaining in years
        float t = std::max(
            0.0f,
            duration - static_cast<float>(i) / 252.0f
        );

        // Reservation price
        float r =
            s -
            inventory.getQuantity()
            * risk_aversion
            * vol * vol
            * t;

        // Optimal total spread
        float spread =
            risk_aversion
            * vol * vol
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
    std::vector<float> getBid(){
        return bid;
    }
    std::vector<float> getAsk() {
        return ask;
    }
    void setParams(float vol_, float risk_aversion_, float liquidity_, float intensity_, float duration_) {
        vol = vol_;
        risk_aversion = risk_aversion_;
        liquidity = liquidity_;
        intensity = intensity_;
        duration = duration_;
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