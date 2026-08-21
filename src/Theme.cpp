#pragma once

#include "imgui.h"

// =============================================================
// Palette
// =============================================================
//
// Deliberately plain: one dark grey for every widget, white
// text, white chart lines. The only colours in the whole app
// are the bid and the ask.
//
// =============================================================

namespace Theme {

    // Surfaces
    inline ImVec4 Background()  { return ImVec4(0.035f, 0.035f, 0.035f, 1.00f); }
    inline ImVec4 Panel()       { return ImVec4(0.086f, 0.086f, 0.086f, 1.00f); }
    inline ImVec4 Raised()      { return Panel(); }
    inline ImVec4 Border()      { return ImVec4(1.000f, 1.000f, 1.000f, 0.24f); }

    // Type: white throughout, stepped down in opacity for hierarchy
    inline ImVec4 Text()        { return ImVec4(1.000f, 1.000f, 1.000f, 1.00f); }
    inline ImVec4 TextDim()     { return ImVec4(1.000f, 1.000f, 1.000f, 0.74f); }
    inline ImVec4 TextFaint()   { return ImVec4(1.000f, 1.000f, 1.000f, 0.56f); }

    // Controls
    inline ImVec4 Accent()      { return ImVec4(0.200f, 0.200f, 0.200f, 1.00f); }
    inline ImVec4 AccentSoft()  { return ImVec4(1.000f, 1.000f, 1.000f, 0.14f); }

    // Chart
    inline ImVec4 Line()        { return ImVec4(1.000f, 1.000f, 1.000f, 1.00f); }
    inline ImVec4 Mid()         { return Line(); }
    inline ImVec4 Bid()         { return ImVec4(0.157f, 0.729f, 0.325f, 1.00f); }
    inline ImVec4 Ask()         { return ImVec4(0.886f, 0.267f, 0.220f, 1.00f); }
    inline ImVec4 Grid()        { return ImVec4(1.000f, 1.000f, 1.000f, 0.10f); }

    inline ImU32 U32(const ImVec4& c) { return ImGui::ColorConvertFloat4ToU32(c); }

    inline ImVec4 Fade(const ImVec4& c, float a)
    {
        return ImVec4(c.x, c.y, c.z, a);
    }

    // =========================================================
    // Metrics
    // =========================================================

    constexpr float HeaderHeight = 68.0f;
    constexpr float SidebarWidth = 344.0f;
    constexpr float StatHeight   = 180.0f;
    constexpr float Gap          = 12.0f;
}
