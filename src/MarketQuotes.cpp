#include "imgui.h"

#include <cfloat>
#include <cmath>
#include <random>
#include <vector>

class MarketQuotes {
private:

public:
  MarketQuotes() {}

  void DrawMarketPath(std::vector<float> path, std::vector<float> bid, std::vector<float> ask) {
    ImGui::PlotLines(
        "GBM",
        path.data(),
        static_cast<int>(path.size()),
        0,
        nullptr,
        FLT_MAX,
        FLT_MAX,
        ImVec2(-1, -1)
    );
  }
};
