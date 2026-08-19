#include <vector>
#include <imgui.h>

#include "GBM.cpp"

class Parameters {
    private:
        //model parameters
        float initial_price = 100.0f;
        float vol = 0.2f;
        float risk_aversion = 0.1f;
        float liquidity = 1.5f;
        float intensity = 1.0f;

        float duration = 1.00f;

        //realism
        float latency = 0.01;
        float maker_fee = 0.001;

        //GBM
        GBM gbm = GBM();
        std::vector<float> path;

    public:
    Parameters(){}

    // Getters
    float getInitialPrice() const {
        return initial_price;
    }

    float getVol() const {
        return vol;
    }

    float getRiskAversion() const {
        return risk_aversion;
    }

    float getLiquidity() const {
        return liquidity;
    }

    float getIntensity() const {
        return intensity;
    }

    float getDuration() const {
        return duration;
    }

    float getLatency() const {
        return latency;
    }

    float getMakerFee() const {
        return maker_fee;
    }

    std::vector<float> getPath() const{
        return path;
    }

    // Setters
    void setInitialPrice(float value) {
        initial_price = value;
    }

    void setVol(float value) {
        vol = value;
    }

    void setRiskAversion(float value) {
        risk_aversion = value;
    }

    void setLiquidity(float value) {
        liquidity = value;
    }

    void setIntensity(float value) {
        intensity = value;
    }

    void setDuration(float value) {
        duration = value;
    }

    void setLatency(float value) {
        latency = value;
    }

    void setMakerFee(float value) {
        maker_fee = value;
    }

    void createMarketPath(){
        path = gbm.BrownianMotion(vol, initial_price);
    }
};