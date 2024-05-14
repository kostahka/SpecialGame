#pragma once

#include "Kengine/components/component.hxx"

struct progress_bar_component : public Kengine::component
{
    static constexpr auto name = "progress_bar_component";

    progress_bar_component();

    std::size_t serialize(std::ostream& os) const override;
    std::size_t deserialize(std::istream& is) override;
    std::size_t serialize_size() const override;

    bool imgui_editable_render() override;

    float progress = 1.0f;

    Kengine::vec2 uv_size_start{ 1.0f, 1.0f };
    Kengine::vec2 uv_size_end{ 0.0f, 0.0f };

    Kengine::vec2 scale_start{ 1.0f, 1.0f };
    Kengine::vec2 scale_end{ 0.0f, 0.0f };
};