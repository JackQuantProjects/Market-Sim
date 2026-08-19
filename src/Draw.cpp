#include <GLFW/glfw3.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <cfloat>
#include <vector>

class Draw {
private:
    GLFWwindow* window;
    Parameters& parameters;

public:
    Draw(Parameters& p)
        : window(nullptr),
          parameters(p)
    {
    }

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

        ImGui::StyleColorsDark();

        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] =
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        return true;
    }

    // ---------------------------------------------------------
    // UI
    // ---------------------------------------------------------

    void init()
    {
        ImVec2 screen = ImGui::GetIO().DisplaySize;

        float width = screen.x;
        float height = screen.y;

        float leftWidth = width * 0.70f;
        float rightWidth = width * 0.30f;

        float wayBarHeight = height * 0.10f;
        float panelHeight = height * 0.30f;

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

        ImGui::PushStyleColor(
            ImGuiCol_Text,
            ImVec4(0, 0, 0, 1)
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
            &riskAversion
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

        int duration = parameters.getDuration();

        ImGui::Text("Duration");

        if (ImGui::InputInt(
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

        ImGui::PopStyleColor();

        // =====================================================
        // Market Quotes
        // =====================================================

        ImGui::SetNextWindowPos(
            ImVec2(0, wayBarHeight),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(leftWidth, panelHeight),
            ImGuiCond_Always
        );

        ImGui::Begin("Market Quotes");

        ImGui::Text("Market");

        ImGui::End();

        // =====================================================
        // Inventory
        // =====================================================

        ImGui::SetNextWindowPos(
            ImVec2(0, wayBarHeight + panelHeight),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(leftWidth, panelHeight),
            ImGuiCond_Always
        );

        ImGui::Begin("Inventory");

        ImGui::Text("Inventory");

        ImGui::End();

        // =====================================================
        // Total PNL
        // =====================================================

        ImGui::SetNextWindowPos(
            ImVec2(0, wayBarHeight + panelHeight * 2),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(leftWidth, panelHeight),
            ImGuiCond_Always
        );

        ImGui::Begin("Total PNL");

        ImGui::Text("Total PNL");

        ImGui::End();
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

    bool IsOpen()
    {
        return !glfwWindowShouldClose(window);
    }

    // =========================================================
    // Market
    // =========================================================

    void DrawMarket(
        const std::vector<float>& path,
        const std::vector<float>& bid,
        const std::vector<float>& ask
    )
    {
        if (!path.empty())
        {
            ImGui::PlotLines(
                "Market Price",
                path.data(),
                path.size(),
                0,
                nullptr,
                FLT_MAX,
                FLT_MAX,
                ImVec2(-1, -1)
            );
        }
    }

    // =========================================================
    // Inventory
    // =========================================================

    void DrawInventory(int inventory)
    {
        ImGui::Text(
            "Inventory: %d",
            inventory
        );
    }

    // =========================================================
    // PNL
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

private:

    bool startPressed = false;
    bool resetPressed = false;
};