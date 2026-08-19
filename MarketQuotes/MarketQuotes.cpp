#include "MarketQuotes.hpp"
#include "imgui.h"

#include <cmath>
#include <random>
#include <vector>

std::vector<float> BrownianMotion(int time_horizon, float T) {
  std::vector<float> path(time_horizon + 1);

  float dt = T / time_horizon; // delta_t
  float drift = 0.05f;
  float vol = 0.40f;

  std::random_device rd;
  std::mt19937 generator(rd());
  std::normal_distribution<float> normal(0.0f, 1.0f);

  path[0] = 100.0f;

  for (int i = 1; i <= time_horizon; i++) {
    float Z = normal(generator);

    path[i] = path[i - 1] * std::exp((drift - 0.5 * vol * vol) *
                                     ((dt) + (vol * std::sqrt(dt) * Z)));
  }

  return path;
}

void DrawMarketQuotes(float x, float y, float width, float height) {
  ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
  ImGui::Begin("Market Quotes");

  // BM
  static std::vector<float> path = BrownianMotion(252, 1.0f);

  ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1, 1, 1, 1));
  ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0, 0, 0, 1));

  ImGui::PlotLines("Brownian Motion", path.data(),
                   static_cast<int>(path.size()), 0, nullptr, FLT_MAX, FLT_MAX,
                   ImVec2(-1, height - 60));

  ImGui::PopStyleColor(2);

  ImGui::End();
}
