#pragma once

#include <algorithm>
#include <vector>
#include <imgui.h>

#include "GBM.cpp"

class Parameters {
    private:
        //model parameters
        float initial_price = 100.0f;
        float vol = 0.2f;
        float risk_aversion = 0.1f;
        float liquidity = 0.5f;
        // order arrivals per year, so it scales with duration below
        float intensity = 2000.0f;

        float duration = 0.01f;

        //realism
        float latency = 0.01;
        float maker_fee = 0.001;

        //single time grid shared by the path, the quotes and the fills
        static constexpr int steps = 252;

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

    int getSteps() const {
        return steps;
    }

    const std::vector<float>& getPath() const{
        return path;
    }

    // Setters
    // Clamped: zero or negative values here produce inf/NaN quotes
    // downstream, which silently kills the whole render.
    void setInitialPrice(float value) {
        initial_price = std::max(value, 0.01f);
    }

    void setVol(float value) {
        vol = std::max(value, 0.0f);
    }

    void setRiskAversion(float value) {
        risk_aversion = std::max(value, 1e-4f);
    }

    void setLiquidity(float value) {
        liquidity = std::max(value, 1e-4f);
    }

    void setIntensity(float value) {
        intensity = std::max(value, 0.0f);
    }

    void setDuration(float value) {
        duration = std::max(value, 1e-4f);
    }

    void setLatency(float value) {
        latency = std::max(value, 0.0f);
    }

    void setMakerFee(float value) {
        maker_fee = std::max(value, 0.0f);
    }

    void createMarketPath(){
        path = gbm.BrownianMotion(vol, initial_price, duration, steps);
    }

    void reset() {
        path.clear();
    }
};
