#include <vector>
#include <cmath>

class Stoikov {
    private:
    std::vector<float> bid;
    std::vector<float> ask;

    Inventory inventory;

    float vol;
    float risk_aversion;
    float liquidity;
    float intensity;
    int duration;

public:
    //store constant variable for quicker retrieval
    Stoikov(
        float vol,
        float risk_aversion,
        float liquidity,
        float intensity,
        int duration,
        Inventory inventory
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
        float t = duration - (i/252);

        float r = s - (inventory.getQuantity() * risk_aversion * (vol * vol) * t);

        float spread = (risk_aversion * (vol * vol) * t) + ((2.0f/risk_aversion) * std::log(1.0f + risk_aversion/liquidity));

        float b = r - spread/2;
        float a = r + spread/2;

        bid.push_back(b);
        ask.push_back(a);

    }
    std::vector<float> getBid(){
        return bid;
    }
    std::vector<float> getAsk() {
        return ask;
    }

};