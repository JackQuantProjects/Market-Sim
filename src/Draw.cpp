#pragma once

#include <GLFW/glfw3.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "Parameters.cpp"
#include "Theme.cpp"

class Draw {
private:
    GLFWwindow* window;
    Parameters& parameters;

    bool startPressed = false;
    bool resetPressed = false;

    ImFont* fontUI = nullptr;
    ImFont* fontMedium = nullptr;
    ImFont* fontMono = nullptr;

    // =========================================================
    // Layout
    // =========================================================
    //
    // Every panel position is derived here so the dashboard
    // stays aligned at any window size.
    //
    // =========================================================

    struct Layout {
        ImVec2 headerPos, headerSize;
        ImVec2 chartPos, chartSize;
        ImVec2 invPos, invSize;
        ImVec2 pnlPos, pnlSize;
        ImVec2 sidePos, sideSize;
    };

    Layout layout() const
    {
        const ImVec2 screen = ImGui::GetIO().DisplaySize;

        const float g = Theme::Gap;
        const float W = screen.x;
        const float H = screen.y;

        Layout L;

        L.headerPos  = ImVec2(0.0f, 0.0f);
        L.headerSize = ImVec2(W, Theme::HeaderHeight);

        const float top = Theme::HeaderHeight + g;

        float sideW = std::min(Theme::SidebarWidth, W * 0.42f);
        float leftW = std::max(240.0f, W - sideW - 3.0f * g);

        L.sidePos  = ImVec2(g * 2.0f + leftW, top);
        L.sideSize = ImVec2(sideW, std::max(120.0f, H - top - g));

        float statH = std::min(Theme::StatHeight, (H - top - 2.0f * g) * 0.34f);
        float chartH = std::max(140.0f, H - top - statH - 2.0f * g);

        L.chartPos  = ImVec2(g, top);
        L.chartSize = ImVec2(leftW, chartH);

        const float statY = top + chartH + g;
        const float invW = (leftW - g) * 0.5f;

        L.invPos  = ImVec2(g, statY);
        L.invSize = ImVec2(invW, statH);

        L.pnlPos  = ImVec2(g + invW + g, statY);
        L.pnlSize = ImVec2(leftW - invW - g, statH);

        return L;
    }

    static ImGuiWindowFlags panelFlags(bool scroll)
    {
        ImGuiWindowFlags f =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoSavedSettings;

        if (!scroll)
            f |= ImGuiWindowFlags_NoScrollbar;

        return f;
    }

    void beginPanel(
        const char* name,
        ImVec2 pos,
        ImVec2 size,
        bool scroll = false
    )
    {
        ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, Theme::Panel());

        ImGui::Begin(name, nullptr, panelFlags(scroll));

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }

    // =========================================================
    // Small typographic helpers
    // =========================================================

    void label(const char* text)
    {
        ImGui::PushFont(fontMedium, 11.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextFaint());
        ImGui::TextUnformatted(text);
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    void hint(const char* text)
    {
        ImGui::SameLine(0.0f, 6.0f);

        ImGui::PushFont(fontUI, 11.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextFaint());
        ImGui::TextUnformatted("?");
        ImGui::PopStyleColor();
        ImGui::PopFont();

        if (ImGui::IsItemHovered())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 10));
            ImGui::SetTooltip("%s", text);
            ImGui::PopStyleVar();
        }
    }

    void pill(ImVec2 pos, const char* text, ImVec4 colour)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();

        ImGui::PushFont(fontMedium, 11.0f);

        const ImVec2 ts = ImGui::CalcTextSize(text);

        const float padX = 10.0f;
        const float padY = 5.0f;
        const float dot  = 6.0f;

        const ImVec2 size(
            ts.x + padX * 2.0f + dot + 7.0f,
            ts.y + padY * 2.0f
        );

        dl->AddRectFilled(
            pos,
            ImVec2(pos.x + size.x, pos.y + size.y),
            Theme::U32(Theme::Fade(colour, 0.16f)),
            size.y * 0.5f
        );

        dl->AddCircleFilled(
            ImVec2(pos.x + padX + dot * 0.5f, pos.y + size.y * 0.5f),
            dot * 0.5f,
            Theme::U32(colour)
        );

        dl->AddText(
            ImVec2(pos.x + padX + dot + 7.0f, pos.y + padY),
            Theme::U32(colour),
            text
        );

        ImGui::PopFont();
    }

    void legendItem(ImVec2& cursor, ImVec4 colour, const char* text)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();

        const float sw = 12.0f;
        const float sh = 3.0f;

        dl->AddRectFilled(
            ImVec2(cursor.x, cursor.y + 5.0f),
            ImVec2(cursor.x + sw, cursor.y + 5.0f + sh),
            Theme::U32(colour),
            1.5f
        );

        dl->AddText(
            ImVec2(cursor.x + sw + 6.0f, cursor.y),
            Theme::U32(Theme::TextDim()),
            text
        );

        cursor.x += sw + 6.0f + ImGui::CalcTextSize(text).x + 18.0f;
    }

    // A compact series chart filled back to a zero baseline, split
    // at every sign change so gains and losses read at a glance.
    void miniSeries(
        ImVec2 a,
        ImVec2 b,
        const std::vector<float>& values,
        ImVec4 posColour,
        ImVec4 negColour
    )
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();

        dl->AddRectFilled(a, b, Theme::U32(Theme::Panel()), 6.0f);

        const float w = b.x - a.x;
        const float h = b.y - a.y;

        if (w < 8.0f || h < 8.0f)
            return;

        if (values.size() < 2)
        {
            ImGui::PushFont(fontUI, 11.0f);

            const char* msg = "no data yet";
            const ImVec2 ts = ImGui::CalcTextSize(msg);

            dl->AddText(
                ImVec2(a.x + (w - ts.x) * 0.5f, a.y + (h - ts.y) * 0.5f),
                Theme::U32(Theme::TextFaint()),
                msg
            );

            ImGui::PopFont();
            return;
        }

        // The baseline is always in frame, so the sign of the
        // series is never ambiguous.
        float lo = 0.0f;
        float hi = 0.0f;

        for (float v : values)
        {
            if (!std::isfinite(v))
                continue;

            lo = std::min(lo, v);
            hi = std::max(hi, v);
        }

        float pad = (hi - lo) * 0.15f;

        if (!(pad > 0.0f))
            pad = 1.0f;

        lo -= pad;
        hi += pad;

        const float span = static_cast<float>(values.size() - 1);

        auto toX = [&](size_t i)
        {
            return a.x + (static_cast<float>(i) / span) * w;
        };

        auto toY = [&](float v)
        {
            return b.y - ((v - lo) / (hi - lo)) * h;
        };

        const float zeroY = toY(0.0f);

        dl->PushClipRect(a, b, true);

        dl->AddLine(
            ImVec2(a.x, zeroY),
            ImVec2(b.x, zeroY),
            Theme::U32(Theme::Fade(Theme::Line(), 0.35f)),
            1.0f
        );

        for (size_t i = 1; i < values.size(); ++i)
        {
            dl->AddLine(
                ImVec2(toX(i - 1), toY(values[i - 1])),
                ImVec2(toX(i),     toY(values[i])),
                Theme::U32(values[i - 1] + values[i] >= 0.0f
                           ? posColour : negColour),
                1.6f
            );
        }

        const size_t last = values.size() - 1;

        dl->AddCircleFilled(
            ImVec2(toX(last), toY(values[last])),
            2.8f,
            Theme::U32(values[last] >= 0.0f ? posColour : negColour)
        );

        dl->PopClipRect();
    }

    // Gridline spacing that lands on round numbers.
    static float niceStep(float range, int target)
    {
        if (range <= 0.0f || target <= 0)
            return 1.0f;

        const float raw = range / static_cast<float>(target);
        const float mag = std::pow(10.0f, std::floor(std::log10(raw)));
        const float norm = raw / mag;

        float step = 10.0f;

        if (norm < 1.5f)      step = 1.0f;
        else if (norm < 3.0f) step = 2.0f;
        else if (norm < 7.0f) step = 5.0f;

        return step * mag;
    }

    static ImFont* loadFirstFont(
        const std::string* paths,
        int count,
        float size
    )
    {
        ImGuiIO& io = ImGui::GetIO();

        for (int i = 0; i < count; ++i)
        {
            if (paths[i].empty())
                continue;

            FILE* probe = std::fopen(paths[i].c_str(), "rb");

            if (!probe)
                continue;

            std::fclose(probe);

            if (ImFont* font =
                    io.Fonts->AddFontFromFileTTF(paths[i].c_str(), size))
                return font;
        }

        return nullptr;
    }

    void loadFonts()
    {
        const char* home = std::getenv("HOME");
        const std::string h = home ? std::string(home) : std::string();

        const std::string regular[] = {
            h.empty() ? std::string() : h + "/.fonts/Roboto-Regular.ttf",
            "/usr/share/fonts/TTF/Roboto-Regular.ttf",
            "/usr/share/fonts/Adwaita/AdwaitaSans-Regular.ttf",
            "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
            "/usr/share/fonts/gsfonts/NimbusSans-Regular.otf"
        };

        const std::string medium[] = {
            h.empty() ? std::string() : h + "/.fonts/Roboto-Medium.ttf",
            "/usr/share/fonts/TTF/Roboto-Medium.ttf",
            "/usr/share/fonts/Adwaita/AdwaitaSans-Bold.ttf",
            "/usr/share/fonts/liberation/LiberationSans-Bold.ttf",
            "/usr/share/fonts/gsfonts/NimbusSans-Bold.otf"
        };

        const std::string mono[] = {
            "/usr/share/fonts/Adwaita/AdwaitaMono-Regular.ttf",
            "/usr/share/fonts/liberation/LiberationMono-Regular.ttf",
            "/usr/share/fonts/gsfonts/NimbusMonoPS-Regular.otf",
            "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
            std::string()
        };

        fontUI     = loadFirstFont(regular, 5, 16.0f);
        fontMedium = loadFirstFont(medium, 5, 16.0f);
        fontMono   = loadFirstFont(mono, 5, 16.0f);

        // Falling back to the built-in font is fine: every PushFont
        // below tolerates a null ImFont* by keeping the current face.
        if (fontUI)
            ImGui::GetIO().FontDefault = fontUI;
    }

    void applyStyle()
    {
        ImGuiStyle& s = ImGui::GetStyle();

        s.WindowRounding    = 10.0f;
        s.ChildRounding     = 8.0f;
        s.FrameRounding     = 7.0f;
        s.GrabRounding      = 7.0f;
        s.PopupRounding     = 8.0f;
        s.ScrollbarRounding = 8.0f;
        s.TabRounding       = 7.0f;

        s.WindowBorderSize = 1.0f;
        s.FrameBorderSize  = 1.0f;
        s.PopupBorderSize  = 1.0f;

        s.WindowPadding    = ImVec2(18.0f, 16.0f);
        s.FramePadding     = ImVec2(11.0f, 7.0f);
        s.ItemSpacing      = ImVec2(10.0f, 9.0f);
        s.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
        s.ScrollbarSize    = 10.0f;
        s.GrabMinSize      = 10.0f;

        s.AntiAliasedLines = true;
        s.AntiAliasedFill  = true;

        ImVec4* c = s.Colors;

        c[ImGuiCol_Text]           = Theme::Text();
        c[ImGuiCol_TextDisabled]   = Theme::TextFaint();
        c[ImGuiCol_WindowBg]       = Theme::Panel();
        c[ImGuiCol_ChildBg]        = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_PopupBg]        = Theme::Raised();
        c[ImGuiCol_Border]         = Theme::Border();
        c[ImGuiCol_BorderShadow]   = ImVec4(0, 0, 0, 0);

        c[ImGuiCol_FrameBg]        = Theme::Raised();
        c[ImGuiCol_FrameBgHovered] = Theme::Fade(Theme::Text(), 0.10f);
        c[ImGuiCol_FrameBgActive]  = Theme::Fade(Theme::Text(), 0.16f);

        c[ImGuiCol_Button]         = Theme::Fade(Theme::Text(), 0.08f);
        c[ImGuiCol_ButtonHovered]  = Theme::Fade(Theme::Text(), 0.16f);
        c[ImGuiCol_ButtonActive]   = Theme::Fade(Theme::Text(), 0.24f);

        c[ImGuiCol_SliderGrab]       = Theme::Text();
        c[ImGuiCol_SliderGrabActive] = Theme::Text();
        c[ImGuiCol_CheckMark]        = Theme::Text();

        c[ImGuiCol_Header]         = Theme::Fade(Theme::Text(), 0.14f);
        c[ImGuiCol_HeaderHovered]  = Theme::Fade(Theme::Text(), 0.20f);
        c[ImGuiCol_HeaderActive]   = Theme::Fade(Theme::Text(), 0.28f);

        c[ImGuiCol_Separator]      = Theme::Border();
        c[ImGuiCol_ScrollbarBg]    = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_ScrollbarGrab]  = Theme::Border();
        c[ImGuiCol_ScrollbarGrabHovered] = Theme::TextFaint();
        c[ImGuiCol_ScrollbarGrabActive]  = Theme::TextDim();

        c[ImGuiCol_ResizeGrip]     = ImVec4(0, 0, 0, 0);
        c[ImGuiCol_NavCursor]      = Theme::Text();
    }

    // A labelled, full-width numeric field.
    bool field(
        const char* text,
        const char* id,
        float* value,
        float speed,
        float lo,
        float hi,
        const char* fmt,
        const char* tip
    )
    {
        label(text);

        if (tip)
            hint(tip);

        ImGui::SetNextItemWidth(-FLT_MIN);

        const bool changed =
            ImGui::DragFloat(id, value, speed, lo, hi, fmt);

        ImGui::Spacing();

        return changed;
    }

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

        glfwWindowHint(GLFW_SAMPLES, 4);

        window = glfwCreateWindow(
            1440,
            860,
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

        // Layout is computed from the display size every frame, so
        // there is nothing worth persisting to imgui.ini.
        ImGui::GetIO().IniFilename = nullptr;

        loadFonts();
        applyStyle();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        return true;
    }

    // =========================================================
    // Header bar + parameter sidebar
    // =========================================================

    void init()
    {
        const Layout L = layout();

        DrawHeader(L);
        DrawParameters(L);
    }

    void DrawHeader(const Layout& L)
    {
        ImGui::SetNextWindowPos(L.headerPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(L.headerSize, ImGuiCond_Always);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, Theme::Raised());

        ImGui::Begin("##header", nullptr, panelFlags(false));

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(3);

        ImDrawList* dl = ImGui::GetWindowDrawList();

        // Hairline under the bar
        dl->AddLine(
            ImVec2(L.headerPos.x, L.headerPos.y + L.headerSize.y - 1.0f),
            ImVec2(L.headerPos.x + L.headerSize.x, L.headerPos.y + L.headerSize.y - 1.0f),
            Theme::U32(Theme::Border()),
            1.0f
        );

        // Accent mark
        const ImVec2 origin = ImGui::GetCursorScreenPos();

        dl->AddRectFilled(
            ImVec2(origin.x, origin.y + 15.0f),
            ImVec2(origin.x + 4.0f, origin.y + 41.0f),
            Theme::U32(Theme::Text()),
            2.0f
        );

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 16.0f);
        ImGui::SetCursorPosY(13.0f);

        ImGui::BeginGroup();

        ImGui::PushFont(fontMedium, 19.0f);
        ImGui::TextUnformatted("Market Sim");
        ImGui::PopFont();

        ImGui::PushFont(fontUI, 12.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextDim());
        ImGui::TextUnformatted(
            "Avellaneda-Stoikov quoting  \xc2\xb7  GBM path  \xc2\xb7  Poisson fills"
        );
        ImGui::PopStyleColor();
        ImGui::PopFont();

        ImGui::EndGroup();

        // -----------------------------------------------------
        // Right side: status + actions
        // -----------------------------------------------------

        const bool hasRun = !parameters.getPath().empty();

        char status[96];

        if (hasRun)
        {
            std::snprintf(
                status, sizeof(status),
                "%d steps  \xc2\xb7  T = %.3f y",
                parameters.getSteps(),
                parameters.getDuration()
            );
        }
        else
        {
            std::snprintf(status, sizeof(status), "Idle");
        }

        const float btnH = 36.0f;
        const float startW = 148.0f;
        const float resetW = 88.0f;

        const float right = L.headerSize.x - 20.0f;
        const float btnY = (L.headerSize.y - btnH) * 0.5f;

        ImGui::PushFont(fontMedium, 11.0f);
        const float pillW = ImGui::CalcTextSize(status).x + 33.0f;
        ImGui::PopFont();

        pill(
            ImVec2(
                L.headerPos.x + right - startW - resetW - 10.0f - 16.0f - pillW,
                L.headerPos.y + (L.headerSize.y - 22.0f) * 0.5f
            ),
            status,
            hasRun ? Theme::Text() : Theme::TextDim()
        );

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

        // Reset: quiet, outlined
        ImGui::SetCursorPos(ImVec2(right - resetW, btnY));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::Fade(Theme::Text(), 0.08f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Theme::Fade(Theme::Text(), 0.14f));
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextDim());

        if (ImGui::Button("Reset", ImVec2(resetW, btnH)))
            resetPressed = true;

        ImGui::PopStyleColor(4);

        // Start: primary
        ImGui::SetCursorPos(ImVec2(right - resetW - 10.0f - startW, btnY));

        ImGui::PushStyleColor(ImGuiCol_Button, Theme::Accent());
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.290f, 0.290f, 0.290f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.145f, 0.145f, 0.145f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::Text());
        ImGui::PushStyleColor(ImGuiCol_Border, Theme::Fade(Theme::Text(), 0.30f));

        ImGui::PushFont(fontMedium, 14.0f);

        if (ImGui::Button("Start Simulation", ImVec2(startW, btnH)))
            startPressed = true;

        ImGui::PopFont();
        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar();

        ImGui::End();
    }

    void DrawParameters(const Layout& L)
    {
        beginPanel("Parameters", L.sidePos, L.sideSize, true);

        ImGui::PushFont(fontMedium, 15.0f);
        ImGui::TextUnformatted("Parameters");
        ImGui::PopFont();

        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextDim());
        ImGui::PushFont(fontUI, 12.0f);
        ImGui::TextWrapped("Applied on the next run.");
        ImGui::PopFont();
        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0, 6));
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 6));

        // -----------------------------------------------------
        label("MARKET");
        ImGui::Dummy(ImVec2(0, 4));

        float initialPrice = parameters.getInitialPrice();
        if (field("Initial Price", "##InitialPrice", &initialPrice,
                  0.25f, 0.01f, 100000.0f, "%.2f",
                  "Starting price of the GBM path."))
            parameters.setInitialPrice(initialPrice);

        float volatility = parameters.getVol();
        if (field("Volatility", "##Volatility", &volatility,
                  0.005f, 0.0f, 5.0f, "%.3f",
                  "Annualised relative volatility of the GBM path.\n"
                  "Converted to price units inside the quoting model."))
            parameters.setVol(volatility);

        float duration = parameters.getDuration();
        if (field("Duration  (T, years)", "##Duration", &duration,
                  0.001f, 0.0001f, 10.0f, "%.4f",
                  "Horizon the quoting model solves over.\n"
                  "Also sets dt for the path and the fill process."))
            parameters.setDuration(duration);

        ImGui::Dummy(ImVec2(0, 6));

        // -----------------------------------------------------
        label("QUOTING MODEL");
        ImGui::Dummy(ImVec2(0, 4));

        float riskAversion = parameters.getRiskAversion();
        if (field("Risk Aversion  (gamma)", "##RiskAversion", &riskAversion,
                  0.002f, 0.0001f, 5.0f, "%.4f",
                  "Higher gamma skews quotes harder against inventory\n"
                  "and widens the time-dependent part of the spread."))
            parameters.setRiskAversion(riskAversion);

        float liquidity = parameters.getLiquidity();
        if (field("Liquidity  (kappa)", "##Liquidity", &liquidity,
                  0.01f, 0.0001f, 100.0f, "%.3f",
                  "Order book depth decay. Higher kappa means fill\n"
                  "probability falls off faster with distance."))
            parameters.setLiquidity(liquidity);

        ImGui::Dummy(ImVec2(0, 6));

        // -----------------------------------------------------
        label("ORDER FLOW");
        ImGui::Dummy(ImVec2(0, 4));

        float intensity = parameters.getIntensity();
        if (field("Intensity  (A, per year)", "##Intensity", &intensity,
                  25.0f, 0.0f, 1000000.0f, "%.0f",
                  "Base arrival rate at zero distance from the mid,\n"
                  "in orders per year. Scaled by dt each step."))
            parameters.setIntensity(intensity);

        ImGui::Dummy(ImVec2(0, 6));

        // -----------------------------------------------------
        label("EXECUTION");
        ImGui::Dummy(ImVec2(0, 4));

        float latency = parameters.getLatency();
        if (field("Latency", "##Latency", &latency,
                  0.001f, 0.0f, 10.0f, "%.4f",
                  "Not yet wired into the simulation."))
            parameters.setLatency(latency);

        float makerFee = parameters.getMakerFee();
        if (field("Maker Fee", "##MakerFee", &makerFee,
                  0.0005f, 0.0f, 1.0f, "%.4f",
                  "Not yet wired into the simulation."))
            parameters.setMakerFee(makerFee);

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

        glfwGetFramebufferSize(window, &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);

        const ImVec4 bg = Theme::Background();

        glClearColor(bg.x, bg.y, bg.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    bool IsOpen()
    {
        return !glfwWindowShouldClose(window);
    }

    // =========================================================
    // Market Window
    // =========================================================

    void BeginMarketWindow()
    {
        const Layout L = layout();

        beginPanel("Market Quotes", L.chartPos, L.chartSize);

        ImGui::PushFont(fontMedium, 15.0f);
        ImGui::TextUnformatted("Market & Quotes");
        ImGui::PopFont();

        ImGui::Dummy(ImVec2(0, 2));
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
        ImDrawList* dl = ImGui::GetWindowDrawList();

        const ImVec2 origin = ImGui::GetCursorScreenPos();
        const ImVec2 avail  = ImGui::GetContentRegionAvail();

        if (avail.x < 60.0f || avail.y < 60.0f)
            return;

        // -----------------------------------------------------
        // Empty state
        // -----------------------------------------------------

        if (path.size() < 2)
        {
            ImGui::PushFont(fontUI, 13.0f);

            const char* msg = "No path yet - press Start Simulation";
            const ImVec2 ts = ImGui::CalcTextSize(msg);

            dl->AddText(
                ImVec2(
                    origin.x + (avail.x - ts.x) * 0.5f,
                    origin.y + (avail.y - ts.y) * 0.5f
                ),
                Theme::U32(Theme::TextFaint()),
                msg
            );

            ImGui::PopFont();
            return;
        }

        // -----------------------------------------------------
        // Plot area
        // -----------------------------------------------------

        const float axisW = 60.0f;   // right-hand price scale
        const float axisH = 22.0f;   // bottom step scale
        const float legendH = 20.0f;

        const ImVec2 p0(origin.x, origin.y + legendH);
        const ImVec2 p1(origin.x + avail.x - axisW, origin.y + avail.y - axisH);

        const float plotW = p1.x - p0.x;
        const float plotH = p1.y - p0.y;

        if (plotW < 40.0f || plotH < 40.0f)
            return;

        // Hover target, registered before the crosshair is drawn.
        ImGui::SetCursorScreenPos(p0);
        ImGui::InvisibleButton("##plot", ImVec2(plotW, plotH));
        const bool hovered = ImGui::IsItemHovered();

        // -----------------------------------------------------
        // Price range
        // -----------------------------------------------------

        float lo = path[0];
        float hi = path[0];

        auto expand = [&](const std::vector<float>& v)
        {
            for (float value : v)
            {
                if (!std::isfinite(value))
                    continue;

                lo = std::min(lo, value);
                hi = std::max(hi, value);
            }
        };

        expand(path);
        expand(bid);
        expand(ask);

        if (!(hi > lo))
        {
            hi += 1.0f;
            lo -= 1.0f;
        }

        const float padding = (hi - lo) * 0.08f;
        lo -= padding;
        hi += padding;

        auto toY = [&](float price)
        {
            const float n = (price - lo) / (hi - lo);
            return p1.y - n * plotH;
        };

        // Where the closing-price tag will sit, so the price scale
        // can leave a gap for it.
        const float lastY = toY(path[path.size() - 1]);

        const float xSteps = static_cast<float>(path.size() - 1);

        auto toX = [&](size_t i)
        {
            return p0.x + (static_cast<float>(i) / xSteps) * plotW;
        };

        // -----------------------------------------------------
        // Plot background
        // -----------------------------------------------------

        dl->AddRectFilled(p0, p1, Theme::U32(Theme::Panel()), 6.0f);

        dl->PushClipRect(p0, p1, true);

        // -----------------------------------------------------
        // Grid + price scale
        // -----------------------------------------------------

        ImGui::PushFont(fontMono, 11.0f);

        const float step = niceStep(hi - lo, 5);

        for (float v = std::ceil(lo / step) * step; v <= hi; v += step)
        {
            const float y = toY(v);

            dl->AddLine(
                ImVec2(p0.x, y),
                ImVec2(p1.x, y),
                Theme::U32(Theme::Grid()),
                1.0f
            );
        }

        dl->PopClipRect();

        for (float v = std::ceil(lo / step) * step; v <= hi; v += step)
        {
            const float y = toY(v);

            if (std::abs(y - lastY) < 11.0f)
                continue;

            char buf[32];
            std::snprintf(buf, sizeof(buf), "%.2f", v);

            dl->AddText(
                ImVec2(p1.x + 9.0f, y - 6.0f),
                Theme::U32(Theme::TextDim()),
                buf
            );
        }

        // Vertical grid + step scale
        const int xDiv = 6;

        for (int k = 0; k <= xDiv; ++k)
        {
            const float t = static_cast<float>(k) / static_cast<float>(xDiv);
            const float x = p0.x + t * plotW;

            if (k > 0 && k < xDiv)
            {
                dl->AddLine(
                    ImVec2(x, p0.y),
                    ImVec2(x, p1.y),
                    Theme::U32(Theme::Grid()),
                    1.0f
                );
            }

            char buf[32];
            std::snprintf(buf, sizeof(buf), "%d", static_cast<int>(t * xSteps));

            const ImVec2 ts = ImGui::CalcTextSize(buf);

            float tx = x - ts.x * 0.5f;
            tx = std::min(tx, p1.x - ts.x);
            tx = std::max(tx, p0.x);

            dl->AddText(
                ImVec2(tx, p1.y + 6.0f),
                Theme::U32(Theme::TextDim()),
                buf
            );
        }

        ImGui::PopFont();

        dl->PushClipRect(p0, p1, true);

        // -----------------------------------------------------
        // Series
        // -----------------------------------------------------

        auto polyline =
            [&](const std::vector<float>& values, ImVec4 colour, float thickness)
        {
            if (values.size() < 2)
                return;

            const ImU32 col = Theme::U32(colour);

            for (size_t i = 1; i < values.size(); ++i)
            {
                dl->AddLine(
                    ImVec2(toX(i - 1), toY(values[i - 1])),
                    ImVec2(toX(i),     toY(values[i])),
                    col,
                    thickness
                );
            }
        };

        polyline(bid,  Theme::Bid(),  1.6f);
        polyline(ask,  Theme::Ask(),  1.6f);
        polyline(path, Theme::Line(), 2.0f);

        // -----------------------------------------------------
        // Last price marker
        // -----------------------------------------------------

        const size_t last = path.size() - 1;
        const ImVec2 lastPt(toX(last), toY(path[last]));

        dl->AddCircleFilled(lastPt, 6.0f, Theme::U32(Theme::Fade(Theme::Line(), 0.30f)));
        dl->AddCircleFilled(lastPt, 3.0f, Theme::U32(Theme::Line()));

        // -----------------------------------------------------
        // Crosshair
        // -----------------------------------------------------

        if (hovered)
        {
            const ImVec2 mouse = ImGui::GetIO().MousePos;

            const float t = (mouse.x - p0.x) / plotW;

            size_t idx = static_cast<size_t>(
                std::lround(std::max(0.0f, std::min(1.0f, t)) * xSteps)
            );

            idx = std::min(idx, path.size() - 1);

            const float x = toX(idx);

            dl->AddLine(
                ImVec2(x, p0.y),
                ImVec2(x, p1.y),
                Theme::U32(Theme::Fade(Theme::Text(), 0.55f)),
                1.0f
            );

            dl->AddCircleFilled(
                ImVec2(x, toY(path[idx])), 3.5f, Theme::U32(Theme::Line()));

            if (idx < bid.size())
                dl->AddCircleFilled(
                    ImVec2(x, toY(bid[idx])), 3.0f, Theme::U32(Theme::Bid()));

            if (idx < ask.size())
                dl->AddCircleFilled(
                    ImVec2(x, toY(ask[idx])), 3.0f, Theme::U32(Theme::Ask()));

            dl->PopClipRect();

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 10));
            ImGui::PushStyleColor(ImGuiCol_PopupBg, Theme::Raised());

            ImGui::BeginTooltip();

            ImGui::PushFont(fontMedium, 11.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextFaint());
            ImGui::Text("STEP %d", static_cast<int>(idx));
            ImGui::PopStyleColor();
            ImGui::PopFont();

            ImGui::Dummy(ImVec2(0, 3));

            ImGui::PushFont(fontMono, 13.0f);

            ImGui::PushStyleColor(ImGuiCol_Text, Theme::Text());
            ImGui::Text("mid  %8.3f", path[idx]);
            ImGui::PopStyleColor();

            if (idx < ask.size())
            {
                ImGui::PushStyleColor(ImGuiCol_Text, Theme::Text());
                ImGui::Text("ask  %8.3f", ask[idx]);
                ImGui::PopStyleColor();
            }

            if (idx < bid.size())
            {
                ImGui::PushStyleColor(ImGuiCol_Text, Theme::Text());
                ImGui::Text("bid  %8.3f", bid[idx]);
                ImGui::PopStyleColor();
            }

            if (idx < bid.size() && idx < ask.size())
            {
                ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextDim());
                ImGui::Text("sprd %8.3f", ask[idx] - bid[idx]);
                ImGui::PopStyleColor();
            }

            ImGui::PopFont();

            ImGui::EndTooltip();

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }
        else
        {
            dl->PopClipRect();
        }

        // -----------------------------------------------------
        // Last price tag on the scale
        // -----------------------------------------------------

        {
            ImGui::PushFont(fontMono, 11.0f);

            char buf[32];
            std::snprintf(buf, sizeof(buf), "%.2f", path[last]);

            const ImVec2 ts = ImGui::CalcTextSize(buf);
            const float y = toY(path[last]);

            const ImVec2 t0(p1.x + 5.0f, y - ts.y * 0.5f - 3.0f);
            const ImVec2 t1(t0.x + ts.x + 10.0f, t0.y + ts.y + 6.0f);

            dl->AddRectFilled(t0, t1, Theme::U32(Theme::Line()), 4.0f);

            dl->AddText(
                ImVec2(t0.x + 5.0f, t0.y + 3.0f),
                Theme::U32(Theme::Background()),
                buf
            );

            ImGui::PopFont();
        }

        // -----------------------------------------------------
        // Legend
        // -----------------------------------------------------

        ImGui::PushFont(fontUI, 11.0f);

        ImVec2 cursor(origin.x, origin.y);

        legendItem(cursor, Theme::Line(), "Mid");
        legendItem(cursor, Theme::Bid(),  "Bid");
        legendItem(cursor, Theme::Ask(),  "Ask");

        ImGui::PopFont();

        // -----------------------------------------------------
        // Closing readout, right aligned on the legend row
        // -----------------------------------------------------

        if (!bid.empty() && !ask.empty())
        {
            ImGui::PushFont(fontMono, 11.0f);

            const size_t lb = bid.size() - 1;
            const size_t la = ask.size() - 1;

            char buf[64];
            std::snprintf(
                buf, sizeof(buf),
                "last %.2f    spread %.3f",
                path[last],
                ask[la] - bid[lb]
            );

            const ImVec2 ts = ImGui::CalcTextSize(buf);

            dl->AddText(
                ImVec2(origin.x + avail.x - ts.x, origin.y),
                Theme::U32(Theme::TextDim()),
                buf
            );

            ImGui::PopFont();
        }
    }

    // =========================================================
    // Inventory Window
    // =========================================================

    void BeginInventoryWindow()
    {
        const Layout L = layout();
        beginPanel("Inventory", L.invPos, L.invSize);
    }

    void EndInventoryWindow()
    {
        ImGui::End();
    }

    // Big value plus caption, shared by both stat cards.
    void statHeadline(
        const char* title,
        const char* value,
        ImVec4 colour,
        const char* caption
    )
    {
        label(title);
        ImGui::Dummy(ImVec2(0, 4));

        ImGui::PushFont(fontMono, 30.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, colour);
        ImGui::TextUnformatted(value);
        ImGui::PopStyleColor();
        ImGui::PopFont();

        ImGui::SameLine(0.0f, 10.0f);

        ImGui::PushFont(fontUI, 12.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::TextFaint());
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(caption);
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }

    // Fills whatever height the card has left with the series.
    void seriesPanel(
        const std::vector<float>& values,
        ImVec4 posColour,
        ImVec4 negColour
    )
    {
        ImGui::Dummy(ImVec2(0, 4));

        const ImVec2 avail = ImGui::GetContentRegionAvail();

        if (avail.x < 40.0f || avail.y < 24.0f)
            return;

        const ImVec2 p = ImGui::GetCursorScreenPos();

        miniSeries(
            p,
            ImVec2(p.x + avail.x, p.y + avail.y),
            values,
            posColour,
            negColour
        );
    }

    void DrawInventory(
        int inventory,
        const std::vector<int>& history
    )
    {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%+d", inventory);

        statHeadline(
            "NET POSITION",
            inventory == 0 ? "0" : buf,
            Theme::Text(),
            inventory > 0 ? "units long"
          : inventory < 0 ? "units short"
                          : "flat"
        );

        // Position through time, so a runaway book is obvious.
        std::vector<float> series;
        series.reserve(history.size());

        for (int q : history)
            series.push_back(static_cast<float>(q));

        seriesPanel(series, Theme::Line(), Theme::Line());
    }

    // =========================================================
    // PNL Window
    // =========================================================

    void BeginPNLWindow()
    {
        const Layout L = layout();
        beginPanel("Total PNL", L.pnlPos, L.pnlSize);
    }

    void EndPNLWindow()
    {
        ImGui::End();
    }

    void DrawPNL(const std::vector<float>& pnl)
    {
        const bool hasData = !pnl.empty();
        const float value = hasData ? pnl.back() : 0.0f;

        char buf[32];
        std::snprintf(buf, sizeof(buf), "%+.2f", value);

        statHeadline(
            "TOTAL PNL",
            hasData ? buf : "n/a",
            hasData ? Theme::Text() : Theme::TextFaint(),
            hasData ? "marked to mid" : "not run yet"
        );

        seriesPanel(pnl, Theme::Line(), Theme::Line());
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
