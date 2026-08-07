#pragma once

#include "Engine/Export.hpp"
#include "Engine/Math/Vec3.hpp"

#include <string>
#include <unordered_set>
#include <vector>

namespace Engine {

class InputManager;
class Renderer;

struct UIRect {
    float x = 0.0f;
    float y = 0.0f;
    float width = 100.0f;
    float height = 40.0f;

    bool Contains(float px, float py) const {
        return px >= x && py >= y && px <= x + width && py <= y + height;
    }
};

class ENGINE_API UISystem {
public:
    void SetScreenSize(int width, int height);
    void Clear();

    void SetText(
        const char* id,
        const char* text,
        float x,
        float y,
        float pixelSize = 3.0f,
        const Vec3& color = Vec3::One());

    void SetButton(
        const char* id,
        const char* label,
        const UIRect& rect,
        const Vec3& color = {0.1f, 0.55f, 0.75f});

    void Remove(const char* id);

    void Update(const InputManager& input);
    void Render(Renderer& renderer) const;

    bool WasClicked(const char* id) const;
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }

    int ScreenWidth() const { return m_screenWidth; }
    int ScreenHeight() const { return m_screenHeight; }

private:
    enum class ElementType {
        Text,
        Button
    };

    struct Element {
        ElementType type = ElementType::Text;
        std::string id;
        std::string text;
        UIRect rect;
        Vec3 color = Vec3::One();
        float pixelSize = 3.0f;
        bool hovered = false;
    };

    void AppendTextQuads(
        std::vector<struct Vertex>& vertices,
        const std::string& text,
        float x,
        float y,
        float pixelSize,
        const Vec3& color) const;

    int m_screenWidth = 1280;
    int m_screenHeight = 720;
    bool m_visible = true;
    std::vector<Element> m_elements;
    std::unordered_set<std::string> m_clickedThisFrame;
};

} // namespace Engine
