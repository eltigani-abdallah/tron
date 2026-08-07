#include "Engine/UI/UISystem.hpp"

#include "Engine/Input/InputManager.hpp"
#include "Engine/Math/Mat4.hpp"
#include "Engine/Render/Renderer.hpp"
#include "Engine/Render/Vertex.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>

namespace Engine {
namespace {

// 5x7 uppercase bitmap font. Each glyph is 5 columns, bit0 = top row.
constexpr int kGlyphW = 5;
constexpr int kGlyphH = 7;

const unsigned char* FindGlyph(char c) {
    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

    static const unsigned char space[5] = {0, 0, 0, 0, 0};
    static const unsigned char colon[5] = {0x00, 0x22, 0x00, 0x22, 0x00};
    static const unsigned char period[5] = {0x00, 0x00, 0x00, 0x60, 0x60};
    static const unsigned char slash[5] = {0x20, 0x10, 0x08, 0x04, 0x02};
    static const unsigned char dash[5] = {0x08, 0x08, 0x08, 0x08, 0x08};
    static const unsigned char digits[10][5] = {
        {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
        {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
        {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
        {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
        {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
        {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
        {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
        {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
        {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
        {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    };
    static const unsigned char letters[26][5] = {
        {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
        {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
        {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
        {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
        {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
        {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
        {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
        {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
        {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
        {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
        {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
        {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
        {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
        {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
        {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
        {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
        {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
        {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
        {0x46, 0x49, 0x49, 0x49, 0x31}, // S
        {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
        {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
        {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
        {0x3F, 0x40, 0x38, 0x40, 0x3F}, // W
        {0x63, 0x14, 0x08, 0x14, 0x63}, // X
        {0x07, 0x08, 0x70, 0x08, 0x07}, // Y
        {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    };

    if (c == ' ') {
        return space;
    }
    if (c == ':') {
        return colon;
    }
    if (c == '.') {
        return period;
    }
    if (c == '/') {
        return slash;
    }
    if (c == '-' || c == '_') {
        return dash;
    }
    if (c >= '0' && c <= '9') {
        return digits[c - '0'];
    }
    if (c >= 'A' && c <= 'Z') {
        return letters[c - 'A'];
    }
    return space;
}

void PushQuad(std::vector<Vertex>& vertices, float x, float y, float w, float h, const Vec3& color, float alpha) {
    const Vertex v0{x, y, 0.0f, color.x, color.y, color.z, alpha};
    const Vertex v1{x + w, y, 0.0f, color.x, color.y, color.z, alpha};
    const Vertex v2{x + w, y + h, 0.0f, color.x, color.y, color.z, alpha};
    const Vertex v3{x, y + h, 0.0f, color.x, color.y, color.z, alpha};
    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v0);
    vertices.push_back(v2);
    vertices.push_back(v3);
}

} // namespace

void UISystem::SetScreenSize(int width, int height) {
    m_screenWidth = width > 0 ? width : 1280;
    m_screenHeight = height > 0 ? height : 720;
}

void UISystem::Clear() {
    m_elements.clear();
    m_clickedThisFrame.clear();
}

void UISystem::SetText(
    const char* id,
    const char* text,
    float x,
    float y,
    float pixelSize,
    const Vec3& color) {
    if (id == nullptr) {
        return;
    }

    for (Element& element : m_elements) {
        if (element.id == id) {
            element.type = ElementType::Text;
            element.text = text != nullptr ? text : "";
            element.rect = {x, y, 0.0f, 0.0f};
            element.pixelSize = pixelSize;
            element.color = color;
            return;
        }
    }

    Element element;
    element.type = ElementType::Text;
    element.id = id;
    element.text = text != nullptr ? text : "";
    element.rect = {x, y, 0.0f, 0.0f};
    element.pixelSize = pixelSize;
    element.color = color;
    m_elements.push_back(element);
}

void UISystem::SetButton(const char* id, const char* label, const UIRect& rect, const Vec3& color) {
    if (id == nullptr) {
        return;
    }

    for (Element& element : m_elements) {
        if (element.id == id) {
            element.type = ElementType::Button;
            element.text = label != nullptr ? label : "";
            element.rect = rect;
            element.color = color;
            element.pixelSize = 3.0f;
            return;
        }
    }

    Element element;
    element.type = ElementType::Button;
    element.id = id;
    element.text = label != nullptr ? label : "";
    element.rect = rect;
    element.color = color;
    element.pixelSize = 3.0f;
    m_elements.push_back(element);
}

void UISystem::Remove(const char* id) {
    if (id == nullptr) {
        return;
    }
    m_elements.erase(
        std::remove_if(
            m_elements.begin(),
            m_elements.end(),
            [id](const Element& e) { return e.id == id; }),
        m_elements.end());
}

void UISystem::Update(const InputManager& input) {
    m_clickedThisFrame.clear();
    if (!m_visible) {
        return;
    }

    const float mx = input.MouseX();
    const float my = input.MouseY();
    const bool clicked = input.WasMousePressed(MouseButton::Left);

    for (Element& element : m_elements) {
        if (element.type != ElementType::Button) {
            continue;
        }
        element.hovered = element.rect.Contains(mx, my);
        if (element.hovered && clicked) {
            m_clickedThisFrame.insert(element.id);
        }
    }
}

bool UISystem::WasClicked(const char* id) const {
    if (id == nullptr) {
        return false;
    }
    return m_clickedThisFrame.find(id) != m_clickedThisFrame.end();
}

void UISystem::AppendTextQuads(
    std::vector<Vertex>& vertices,
    const std::string& text,
    float x,
    float y,
    float pixelSize,
    const Vec3& color) const {
    float cursorX = x;
    const float cursorY = y;

    for (char ch : text) {
        const unsigned char* glyph = FindGlyph(ch);
        for (int col = 0; col < kGlyphW; ++col) {
            const unsigned char bits = glyph[col];
            for (int row = 0; row < kGlyphH; ++row) {
                if ((bits & (1u << row)) != 0) {
                    PushQuad(
                        vertices,
                        cursorX + static_cast<float>(col) * pixelSize,
                        cursorY + static_cast<float>(row) * pixelSize,
                        pixelSize,
                        pixelSize,
                        color,
                        1.0f);
                }
            }
        }
        cursorX += (kGlyphW + 1) * pixelSize;
    }
}

void UISystem::Render(Renderer& renderer) const {
    if (!m_visible || m_elements.empty()) {
        return;
    }

    std::vector<Vertex> vertices;
    vertices.reserve(2048);

    for (const Element& element : m_elements) {
        if (element.type == ElementType::Button) {
            Vec3 bg = element.color;
            if (element.hovered) {
                bg = bg + Vec3{0.15f, 0.15f, 0.15f};
            }
            PushQuad(vertices, element.rect.x, element.rect.y, element.rect.width, element.rect.height, bg, 0.85f);

            const float textWidth = static_cast<float>(element.text.size()) * (kGlyphW + 1) * element.pixelSize;
            const float textHeight = static_cast<float>(kGlyphH) * element.pixelSize;
            const float textX = element.rect.x + (element.rect.width - textWidth) * 0.5f;
            const float textY = element.rect.y + (element.rect.height - textHeight) * 0.5f;
            AppendTextQuads(vertices, element.text, textX + 1.0f, textY + 1.0f, element.pixelSize, {0.0f, 0.0f, 0.0f});
            AppendTextQuads(vertices, element.text, textX, textY, element.pixelSize, Vec3::One());
        } else {
            // Soft shadow for HUD readability on busy scenes.
            AppendTextQuads(
                vertices,
                element.text,
                element.rect.x + 1.5f,
                element.rect.y + 1.5f,
                element.pixelSize,
                {0.0f, 0.02f, 0.05f});
            AppendTextQuads(
                vertices,
                element.text,
                element.rect.x,
                element.rect.y,
                element.pixelSize,
                element.color);
        }
    }

    if (vertices.empty()) {
        return;
    }

    // Pixel space, y down (matches Win32 mouse client coords).
    const Mat4 ortho = Mat4::Orthographic(
        0.0f,
        static_cast<float>(m_screenWidth),
        static_cast<float>(m_screenHeight),
        0.0f,
        0.0f,
        1.0f);
    renderer.DrawWorldTriangles(vertices.data(), static_cast<unsigned int>(vertices.size()), ortho, false);
}

} // namespace Engine
