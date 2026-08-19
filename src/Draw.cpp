#include <GLFW/glfw3.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

class Draw {
private:
    GLFWwindow* window;

    Sim& sim;
    MarketQuotes marketQuotes;

public:
    Draw(Sim& simulation)
        : sim(simulation),
          marketQuotes()
    {
        window = nullptr;
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

        ImGuiIO& io = ImGui::GetIO();
        (void)io;

        ImGui::StyleColorsDark();

        ImGui::GetStyle().Colors[ImGuiCol_WindowBg] =
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        return true;
    }

    void Run()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            DrawInterface();

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
    }

    void DrawInterface()
    {
        ImVec2 screen = ImGui::GetIO().DisplaySize;

        float width = screen.x;
        float height = screen.y;

        float leftWidth = width * 0.70f;
        float rightWidth = width * 0.30f;

        float wayBarHeight = height * 0.10f;
        float panelHeight = height * 0.30f;

        // ---------------------------------------------------------
        // Parameters
        // ---------------------------------------------------------

        sim.getParameters().Draw(
            leftWidth,
            wayBarHeight,
            rightWidth,
            height - wayBarHeight
        );

        // ---------------------------------------------------------
        // Way Bar
        // ---------------------------------------------------------

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
            sim.startSim();
        }

        ImGui::End();

        // ---------------------------------------------------------
        // Market Quotes
        // ---------------------------------------------------------

        ImGui::SetNextWindowPos(
            ImVec2(0, wayBarHeight),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(leftWidth, panelHeight),
            ImGuiCond_Always
        );

        ImGui::Begin("Market Quotes");

        marketQuotes.DrawMarketPath(
            sim.getParameters().getPath()
        );

        ImGui::End();

        // ---------------------------------------------------------
        // Inventory
        // ---------------------------------------------------------

        ImGui::SetNextWindowPos(
            ImVec2(0, wayBarHeight + panelHeight),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(leftWidth, panelHeight),
            ImGuiCond_Always
        );

        ImGui::Begin("Inventory");

        // Inventory drawing will go here

        ImGui::End();

        // ---------------------------------------------------------
        // Total PNL
        // ---------------------------------------------------------

        ImGui::SetNextWindowPos(
            ImVec2(0, wayBarHeight + panelHeight * 2),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(leftWidth, panelHeight),
            ImGuiCond_Always
        );

        ImGui::Begin("Total PNL");

        // PNL drawing will go here

        ImGui::End();
    }

    void Shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();

        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }
};