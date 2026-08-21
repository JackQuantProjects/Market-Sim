#pragma once

#include "imgui.h"

// =============================================================
// Palette
// =============================================================
//
// One place to retune the whole look. Everything the UI draws
// pulls from here, so swapping these values reskins the app.
//
// =============================================================

namespace Theme {

    // Surfaces
    inline ImVec4 Background()  { return ImVec4(0.043f, 0.051f, 0.063f, 1.00f); }
    inline ImVec4 Panel()       { return ImVec4(0.075f, 0.086f, 0.106f, 1.00f); }
    inline ImVec4 Raised()      { return ImVec4(0.106f, 0.122f, 0.149f, 1.00f); }
    inline ImVec4 Border()      { return ImVec4(0.157f, 0.180f, 0.216f, 1.00f); }

    // Type
    inline ImVec4 Text()        { return ImVec4(0.902f, 0.929f, 0.953f, 1.00f); }
    inline ImVec4 TextDim()     { return ImVec4(0.475f, 0.533f, 0.596f, 1.00f); }
    inline ImVec4 TextFaint()   { return ImVec4(0.325f, 0.373f, 0.427f, 1.00f); }

    // Meaning
    inline ImVec4 Accent()      { return ImVec4(0.259f, 0.588f, 1.000f, 1.00f); }
    inline ImVec4 AccentSoft()  { return ImVec4(0.259f, 0.588f, 1.000f, 0.18f); }
    inline ImVec4 Bid()         { return ImVec4(0.149f, 0.729f, 0.612f, 1.00f); }
    inline ImVec4 Ask()         { return ImVec4(0.937f, 0.325f, 0.314f, 1.00f); }
    inline ImVec4 Mid()         { return ImVec4(0.949f, 0.706f, 0.161f, 1.00f); }
    inline ImVec4 Grid()        { return ImVec4(1.000f, 1.000f, 1.000f, 0.075f); }

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
    constexpr float StatHeight   = 148.0f;
    constexpr float Gap          = 12.0f;
}
