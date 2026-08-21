#pragma once

#include <GLFW/glfw3.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <algorithm>
#include <cfloat>
#include <cstddef>
#include <iostream>
#include <vector>

#include "Parameters.cpp"

class Draw {
private:
    GLFWwindow* window;
    Parameters& parameters;

    bool startPressed = false;
    bool resetPressed = false;

public:

    // =========================================================
    // Constructor
    // =========================================================

    Draw(Parameters& p)
        : window(nullptr),
          parameters(p)
    {
    }

    // =========================================================
    // Initialise
    // =========================================================

    bool Initialise()
    {
        if (!glfwInit())
            return false;

        const char* glsl_version = "#version 130";

        window = glfwCreateWindow(
            1280,
            720,
            "Market Sim",
            nullptr,
            nullptr
        );

        if (!window)
        {
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Light theme, so text stays readable against the white
        // window background below.
        ImGui::StyleColorsLight();

        // White window background
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] =
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        return true;
    }

    // =========================================================
    // Initial UI
    // =========================================================
    //
    // Creates the empty widgets.
    //
    // The actual contents of Market Quotes, Inventory and PNL
    // are drawn separately by Sim.
    //
    // =========================================================

    void init()
    {
        ImVec2 screen = ImGui::GetIO().DisplaySize;

        float width = screen.x;
        float height = screen.y;

        float leftWidth = width * 0.70f;
        float rightWidth = width * 0.30f;

        float wayBarHeight = height * 0.10f;

        // =====================================================
        // Way Bar
        // =====================================================

        ImGui::SetNextWindowPos(
            ImVec2(0, 0),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(width, wayBarHeight),
            ImGuiCond_Always
        );

        ImGui::Begin("Way Bar");

        if (ImGui::Button("Start Simulation"))
        {
            startPressed = true;
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset"))
        {
            resetPressed = true;
        }

        ImGui::End();

        // =====================================================
        // Parameters
        // =====================================================

        ImGui::SetNextWindowPos(
            ImVec2(leftWidth, wayBarHeight),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(rightWidth, height - wayBarHeight),
            ImGuiCond_Always
        );

        ImGui::Begin("Parameters");

        ImGui::Text("Simulation Parameters");

        ImGui::Separator();

        // -----------------------------------------------------
        // Initial Price
        // -----------------------------------------------------

        float initialPrice = parameters.getInitialPrice();

        ImGui::Text("Initial Price");

        if (ImGui::InputFloat(
            "##InitialPrice",
            &initialPrice
        ))
        {
            parameters.setInitialPrice(initialPrice);
        }

        // -----------------------------------------------------
        // Volatility
        // -----------------------------------------------------

        float volatility = parameters.getVol();

        ImGui::Text("Volatility");

        if (ImGui::InputFloat(
            "##Volatility",
            &volatility
        ))
        {
            parameters.setVol(volatility);
        }

        // -----------------------------------------------------
        // Risk Aversion
        // -----------------------------------------------------

        float riskAversion = parameters.getRiskAversion();

        ImGui::Text("Risk Aversion");

        if (ImGui::InputFloat(
            "##RiskAversion",
            &riskAversion,
            0.01f,
            0.1f,
            "%.2f"
        ))
        {
            parameters.setRiskAversion(riskAversion);
        }

        // -----------------------------------------------------
        // Liquidity
        // -----------------------------------------------------

        float liquidity = parameters.getLiquidity();

        ImGui::Text("Liquidity");

        if (ImGui::InputFloat(
            "##Liquidity",
            &liquidity
        ))
        {
            parameters.setLiquidity(liquidity);
        }

        // -----------------------------------------------------
        // Intensity
        // -----------------------------------------------------

        float intensity = parameters.getIntensity();

        ImGui::Text("Intensity");

        if (ImGui::InputFloat(
            "##Intensity",
            &intensity
        ))
        {
            parameters.setIntensity(intensity);
        }

        // -----------------------------------------------------
        // Duration
        // -----------------------------------------------------

        float duration = parameters.getDuration();

        ImGui::Text("Duration");

        if (ImGui::InputFloat(
            "##Duration",
            &duration
        ))
        {
            parameters.setDuration(duration);
        }

        // -----------------------------------------------------
        // Latency
        // -----------------------------------------------------

        float latency = parameters.getLatency();

        ImGui::Text("Latency");

        if (ImGui::InputFloat(
            "##Latency",
            &latency
        ))
        {
            parameters.setLatency(latency);
        }

        // -----------------------------------------------------
        // Maker Fee
        // -----------------------------------------------------

        float makerFee = parameters.getMakerFee();

        ImGui::Text("Maker Fee");

        if (ImGui::InputFloat(
            "##MakerFee",
            &makerFee
        ))
        {
            parameters.setMakerFee(makerFee);
        }

        ImGui::End();

        // Market Quotes, Inventory and Total PNL are opened and sized
        // by Sim via BeginMarketWindow() / BeginInventoryWindow() /
        // BeginPNLWindow(), so they are not begun here as well.
    }

    // =========================================================
    // Frame
    // =========================================================

    void BeginFrame()
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
    }

    void EndFrame()
    {
        ImGui::Render();

        int display_w;
        int display_h;

        glfwGetFramebufferSize(
            window,
            &display_w,
            &display_h
        );

        glViewport(
            0,
            0,
            display_w,
            display_h
        );

        glClearColor(
            0.15f,
            0.15f,
            0.15f,
            1.0f
        );

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(
            ImGui::GetDrawData()
        );

        glfwSwapBuffers(window);
    }

    // =========================================================
    // Window Status
    // =========================================================

    bool IsOpen()
    {
        return !glfwWindowShouldClose(window);
    }

    // =========================================================
    // Market Window
    // =========================================================

    void BeginMarketWindow()
    {
        ImGui::SetNextWindowPos(
            ImVec2(
                0,
                ImGui::GetIO().DisplaySize.y * 0.10f
            ),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(
                ImGui::GetIO().DisplaySize.x * 0.70f,
                ImGui::GetIO().DisplaySize.y * 0.30f
            ),
            ImGuiCond_Always
        );

        ImGui::Begin("Market Quotes");
    }

    void EndMarketWindow()
    {
        ImGui::End();
    }

    // =========================================================
    // Draw Market
    // =========================================================

    void DrawMarket(
        const std::vector<float>& path,
        const std::vector<float>& bid,
        const std::vector<float>& ask
    )
    {
        if (path.size() < 2)
            return;

        ImDrawList* draw_list =
            ImGui::GetWindowDrawList();

        ImVec2 canvas_pos =
            ImGui::GetCursorScreenPos();

        ImVec2 canvas_size =
            ImGui::GetContentRegionAvail();

        // -----------------------------------------------------
        // Find price range
        // -----------------------------------------------------

        float min_price = path[0];
        float max_price = path[0];

        auto update_range =
            [&](const std::vector<float>& values)
        {
            for (float value : values)
            {
                min_price =
                    std::min(min_price, value);

                max_price =
                    std::max(max_price, value);
            }
        };

        update_range(path);
        update_range(bid);
        update_range(ask);

        // Prevent division by zero
        if (max_price == min_price)
        {
            max_price += 1.0f;
            min_price -= 1.0f;
        }

        // -----------------------------------------------------
        // Price -> screen Y
        // -----------------------------------------------------

        auto price_to_screen_y =
            [&](float price)
        {
            float normalized =
                (price - min_price) /
                (max_price - min_price);

            return canvas_pos.y +
                   canvas_size.y *
                   (1.0f - normalized);
        };

        // -----------------------------------------------------
        // Draw line
        // -----------------------------------------------------

        // Every series shares the path's x axis, so a shorter bid or
        // ask series truncates instead of stretching across the canvas.
        const float x_steps =
            static_cast<float>(path.size() - 1);

        auto draw_line =
            [&](const std::vector<float>& values,
                ImU32 colour)
        {
            if (values.size() < 2)
                return;

            for (size_t i = 1;
                 i < values.size();
                 ++i)
            {
                float x1 =
                    canvas_pos.x +
                    (
                        static_cast<float>(i - 1) /
                        x_steps
                    ) *
                    canvas_size.x;

                float x2 =
                    canvas_pos.x +
                    (
                        static_cast<float>(i) /
                        x_steps
                    ) *
                    canvas_size.x;

                float y1 =
                    price_to_screen_y(
                        values[i - 1]
                    );

                float y2 =
                    price_to_screen_y(
                        values[i]
                    );

                draw_list->AddLine(
                    ImVec2(x1, y1),
                    ImVec2(x2, y2),
                    colour,
                    2.0f
                );
            }
        };

        // -----------------------------------------------------
        // Market Price
        // Black
        // -----------------------------------------------------

        draw_line(
            path,
            IM_COL32(0, 0, 0, 255)
        );

        // -----------------------------------------------------
        // Bid
        // Red
        // -----------------------------------------------------

        draw_line(
            bid,
            IM_COL32(255, 0, 0, 255)
        );

        // -----------------------------------------------------
        // Ask
        // Green
        // -----------------------------------------------------

        draw_line(
            ask,
            IM_COL32(0, 180, 0, 255)
        );
    }

    // =========================================================
    // Inventory Window
    // =========================================================

    void BeginInventoryWindow()
    {
        ImVec2 screen =
            ImGui::GetIO().DisplaySize;

        float wayBarHeight =
            screen.y * 0.10f;

        float panelHeight =
            screen.y * 0.30f;

        float leftWidth =
            screen.x * 0.70f;

        ImGui::SetNextWindowPos(
            ImVec2(
                0,
                wayBarHeight + panelHeight
            ),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(
                leftWidth,
                panelHeight
            ),
            ImGuiCond_Always
        );

        ImGui::Begin("Inventory");
    }

    void EndInventoryWindow()
    {
        ImGui::End();
    }

    // =========================================================
    // Draw Inventory
    // =========================================================

    void DrawInventory(int inventory)
    {
        ImGui::Text(
            "Inventory: %d",
            inventory
        );
    }

    // =========================================================
    // PNL Window
    // =========================================================

    void BeginPNLWindow()
    {
        ImVec2 screen =
            ImGui::GetIO().DisplaySize;

        float wayBarHeight =
            screen.y * 0.10f;

        float panelHeight =
            screen.y * 0.30f;

        float leftWidth =
            screen.x * 0.70f;

        ImGui::SetNextWindowPos(
            ImVec2(
                0,
                wayBarHeight + panelHeight * 2
            ),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(
                leftWidth,
                panelHeight
            ),
            ImGuiCond_Always
        );

        ImGui::Begin("Total PNL");
    }

    void EndPNLWindow()
    {
        ImGui::End();
    }

    // =========================================================
    // Draw PNL
    // =========================================================

    void DrawPNL(float pnl)
    {
        ImGui::Text(
            "PNL: %.2f",
            pnl
        );
    }

    // =========================================================
    // Buttons
    // =========================================================

    bool StartPressed()
    {
        if (startPressed)
        {
            startPressed = false;
            return true;
        }

        return false;
    }

    bool ResetPressed()
    {
        if (resetPressed)
        {
            resetPressed = false;
            return true;
        }

        return false;
    }

    // =========================================================
    // Shutdown
    // =========================================================

    void Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }
};