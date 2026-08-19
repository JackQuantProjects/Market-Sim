#include <vector>
#include <imgui.h>

#include "GBM.cpp"

class Parameters {
    private:
        //model parameters
        float initial_price = 0;
        float vol = 0;
        float risk_aversion = 0;
        float liquidity = 0;
        float intensity = 0;

        int duration = 0;

        //realism
        float latency = 0;
        float maker_fee = 0;

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

    int getDuration() const {
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
    void setInitialPrice(int value) {
        initial_price = value;
    }

    void setVol(float value) {
        vol = value;
    }

    void setRiskAversion(int value) {
        risk_aversion = value;
    }

    void setLiquidity(float value) {
        liquidity = value;
    }

    void setIntensity(float value) {
        intensity = value;
    }

    void setDuration(int value) {
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


    void Draw(float x, float y, float width, float height) {
    ImGui::SetNextWindowPos(
        ImVec2(x, y),
        ImGuiCond_Always
    );

    ImGui::SetNextWindowSize(
        ImVec2(width, height),
        ImGuiCond_Always
    );

    ImGui::Begin("PARAMETERS");

    float initialPriceValue = initial_price;
    float volValue = vol;
    float riskAversionValue = risk_aversion;
    float liquidityValue = liquidity;
    float intensityValue = intensity;
    int durationValue = duration;
    float latencyValue = latency;
    float makerFeeValue = maker_fee;

    ImGui::PushStyleColor(
        ImGuiCol_Text,
        ImVec4(0, 0, 0, 1)
    );

    ImGui::Text("Initial Price:");
    ImGui::InputFloat(
        "##InitialPrice",
        &initialPriceValue
    );

    ImGui::Text("Volatility:");
    ImGui::InputFloat(
        "##Volatility",
        &volValue
    );

    ImGui::Text("Risk Aversion:");
    ImGui::InputFloat(
        "##RiskAversion",
        &riskAversionValue
    );

    ImGui::Text("Liquidity:");
    ImGui::InputFloat(
        "##Liquidity",
        &liquidityValue
    );

    ImGui::Text("Intensity:");
    ImGui::InputFloat(
        "##Intensity",
        &intensityValue
    );

    ImGui::Text("Duration:");
    ImGui::InputInt(
        "##Duration",
        &durationValue
    );

    ImGui::Text("Latency:");
    ImGui::InputFloat(
        "##Latency",
        &latencyValue
    );

    ImGui::Text("Maker Fee:");
    ImGui::InputFloat(
        "##MakerFee",
        &makerFeeValue
    );

    ImGui::PopStyleColor();

    // Save changes back to the class
    initial_price = initialPriceValue;
    vol = volValue;
    risk_aversion = riskAversionValue;
    liquidity = liquidityValue;
    intensity = intensityValue;
    duration = durationValue;
    latency = latencyValue;
    maker_fee = makerFeeValue;

    ImGui::End();
    }
};