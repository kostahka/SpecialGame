#pragma once

#include "imgui.h"

namespace gui
{
// clang-format off
    constexpr ImGuiWindowFlags window_flags =
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoNav |
//                                            ImGuiWindowFlags_NoBackground |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoScrollbar |
                                            ImGuiWindowFlags_NoScrollWithMouse |
                                            ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoTitleBar
;
// clang-format on
} // namespace gui