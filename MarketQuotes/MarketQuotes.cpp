#include "MarketQuotes.hpp"
#include "imgui.h"

#include <cfloat>
#include <cstdlib>
#include <vector>

void DrawMarketQuotes(float x, float y, float width, float height) {
  ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
  ImGui::Begin("Market Quotes");

  // GBM

  ImGui::End();
}
