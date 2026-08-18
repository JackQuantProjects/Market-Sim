#include <GLFW/glfw3.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include "Inventory/Inventory.hpp"
#include "MarketQuotes/MarketQuotes.hpp"
#include "Parameters/Parameters.hpp"
#include "TotalPNL/TotalPNL.hpp"
#include "WayBar/WayBar.hpp"

int main() {
  // Initialise GLFW
  if (!glfwInit())
    return 1;

  const char *glsl_version = "#version 130";

  // Create GLFW window
  GLFWwindow *window =
      glfwCreateWindow(1280, 720, "Market Sim", nullptr, nullptr);

  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Create ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  // Set ImGui style
  ImGui::StyleColorsDark();

  ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

  // Initialise ImGui backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 screen = ImGui::GetIO().DisplaySize;

    float width = screen.x;
    float height = screen.y;

    // Layout dimensions
    float leftWidth = width * 0.70f;
    float rightWidth = width * 0.30f;

    float wayBarHeight = height * 0.10f;
    float panelHeight = height * 0.30f;

    // ---------------------------------------------------------
    // Way Bar
    // ---------------------------------------------------------

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

    ImGui::SetNextWindowSize(ImVec2(width, wayBarHeight), ImGuiCond_Always);

    ImGui::Begin("Way Bar");
    ImGui::End();

    // ---------------------------------------------------------
    // Market Quotes
    // ---------------------------------------------------------

    ImGui::SetNextWindowPos(ImVec2(0, wayBarHeight), ImGuiCond_Always);

    ImGui::SetNextWindowSize(ImVec2(leftWidth, panelHeight), ImGuiCond_Always);

    DrawMarketQuotes(0, wayBarHeight, leftWidth, panelHeight);

    // ---------------------------------------------------------
    // Inventory
    // ---------------------------------------------------------

    ImGui::SetNextWindowPos(ImVec2(0, wayBarHeight + panelHeight),
                            ImGuiCond_Always);

    ImGui::SetNextWindowSize(ImVec2(leftWidth, panelHeight), ImGuiCond_Always);

    ImGui::Begin("Inventory");
    ImGui::End();

    // ---------------------------------------------------------
    // Total PNL
    // ---------------------------------------------------------

    ImGui::SetNextWindowPos(ImVec2(0, wayBarHeight + panelHeight * 2),
                            ImGuiCond_Always);

    ImGui::SetNextWindowSize(ImVec2(leftWidth, panelHeight), ImGuiCond_Always);

    ImGui::Begin("Total PNL");
    ImGui::End();

    // ---------------------------------------------------------
    // Parameters
    // ---------------------------------------------------------

    ImGui::SetNextWindowPos(ImVec2(leftWidth, wayBarHeight), ImGuiCond_Always);

    ImGui::SetNextWindowSize(ImVec2(rightWidth, height - wayBarHeight),
                             ImGuiCond_Always);

    ImGui::Begin("PARAMETERS");
    ImGui::End();

    // Render ImGui
    ImGui::Render();

    int display_w;
    int display_h;

    glfwGetFramebufferSize(window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);

    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
