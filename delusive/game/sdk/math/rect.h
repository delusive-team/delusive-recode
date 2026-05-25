#pragma once
#include "vector.h"

struct rect_t {
    float m_XMin, m_YMin, m_Width, m_Height;

    rect_t() : m_XMin(0), m_YMin(0), m_Width(0), m_Height(0) {}

    rect_t(float x, float y, float width, float height)
        : m_XMin(x), m_YMin(y), m_Width(width), m_Height(height) {}

    rect_t(vec2_t position, vec2_t size)
        : m_XMin(position.x), m_YMin(position.y), m_Width(size.x), m_Height(size.y) {}

    rect_t(const rect_t& source)
        : m_XMin(source.m_XMin), m_YMin(source.m_YMin), m_Width(source.m_Width), m_Height(source.m_Height) {}
};
