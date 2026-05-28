#pragma once
#include <cmath>
#include <algorithm>
#include <unordered_map>

class vec2_t
{
public:
    float x;
    float y;

    vec2_t() : x(0.f), y(0.f) {}
    vec2_t(float x, float y) : x(x), y(y) {}

    vec2_t& operator+=(const vec2_t& v) { x += v.x; y += v.y; return *this; }
    vec2_t& operator-=(const vec2_t& v) { x -= v.x; y -= v.y; return *this; }
    vec2_t& operator*=(float n) { x *= n;   y *= n;   return *this; }
    vec2_t& operator/=(float n) { x /= n;   y /= n;   return *this; }

    vec2_t  operator+(const vec2_t& v) const { return { x + v.x, y + v.y }; }
    vec2_t  operator-(const vec2_t& v) const { return { x - v.x, y - v.y }; }
    vec2_t  operator*(const vec2_t& v) const { return { x * v.x, y * v.y }; }
    vec2_t  operator*(float n)         const { return { x * n,   y * n }; }
    vec2_t  operator/(float n)         const { return { x / n,   y / n }; }

    bool operator==(const vec2_t& v) const { return x == v.x && y == v.y; }
    bool operator!=(const vec2_t& v) const { return x != v.x || y != v.y; }

    float length() const
    {
        return sqrtf(x * x + y * y);
    }

    float distance_to(const vec2_t& v) const
    {
        float dx = x - v.x;
        float dy = y - v.y;
        return sqrtf(dx * dx + dy * dy);
    }

    vec2_t normalized() const
    {
        float len = length();
        if (len < 0.0001f) return { 0.f, 0.f };
        return { x / len, y / len };
    }

    vec2_t rotate(float angle) const
    {
        return {
            x * cosf(angle) - y * sinf(angle),
            x * sinf(angle) + y * cosf(angle)
        };
    }

    bool empty() const { return x == 0.f && y == 0.f; }

    ImVec2 get() const { return ImVec2(x, y); }
};

class vec3_t
{
public:
    float x;
    float y;
    float z;

    vec3_t() : x(0.f), y(0.f), z(0.f) {}
    vec3_t(float x, float y, float z) : x(x), y(y), z(z) {}

    vec3_t  operator+(const vec3_t& v)  const { return { x + v.x, y + v.y, z + v.z }; }
    vec3_t  operator-(const vec3_t& v)  const { return { x - v.x, y - v.y, z - v.z }; }
    vec3_t  operator-()                 const { return { -x, -y, -z }; }
    vec3_t  operator*(const vec3_t& v)  const { return { x * v.x, y * v.y, z * v.z }; }
    vec3_t  operator*(float n)          const { return { x * n,   y * n,   z * n }; }
    vec3_t  operator/(float n)          const { return { x / n,   y / n,   z / n }; }

    vec3_t& operator+=(const vec3_t& v) { x += v.x; y += v.y; z += v.z; return *this; }
    vec3_t& operator-=(const vec3_t& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    vec3_t& operator*=(float n) { x *= n;   y *= n;   z *= n;   return *this; }
    vec3_t& operator/=(float n) { x /= n;   y /= n;   z /= n;   return *this; }

    bool operator==(const vec3_t& v) const { return x == v.x && y == v.y && z == v.z; }

    bool operator!=(const vec3_t& v) const { return x != v.x || y != v.y || z != v.z; }

    bool empty() const { return x == 0.f && y == 0.f && z == 0.f; }

    float length() const
    {
        return sqrtf(x * x + y * y + z * z);
    }

    float length_2d() const
    {
        return sqrtf(x * x + z * z);
    }

    float magnitude2d() const
    {
        return sqrtf(x * x + z * z);
    }

    vec3_t normalized() const
    {
        float len = length();
        if (len < 0.0001f) return { 0.f, 0.f, 0.f };
        return { x / len, y / len, z / len };
    }

    vec3_t unity_normalize() const
    {
        float num = length();
        if (num > 1e-5f)
            return { x / num, y / num, z / num };
        return { 0.f, 0.f, 0.f };
    }

    vec3_t cross(const vec3_t& rhs) const
    {
        return {
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        };
    }

    float dot(const vec3_t& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    float distance_to(const vec3_t& v) const
    {
        float dx = x - v.x;
        float dy = y - v.y;
        float dz = z - v.z;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    vec3_t xz3d() const { return { x, 0.f, z }; }
    vec3_t xz()   const { return { x, 0.f, z }; }

    static float clamp01(float value)
    {
        if (value < 0.f) return 0.f;
        if (value > 1.f) return 1.f;
        return value;
    }

    static float clamp(float value, float min_v, float max_v)
    {
        if (value < min_v) return min_v;
        if (value > max_v) return max_v;
        return value;
    }

    static vec3_t lerp(const vec3_t& a, const vec3_t& b, float t)
    {
        t = clamp01(t);
        return {
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        };
    }

    static float inverse_lerp(float a, float b, float value)
    {
        if (a == b) return 0.f;
        return clamp01((value - a) / (b - a));
    }

    ImVec2 get() const { return ImVec2(x, y); }
};


class quat_t
{
public:
    float x, y, z, w;

    quat_t() : x(0.f), y(0.f), z(0.f), w(0.f) {}
    quat_t(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    quat_t operator*(const quat_t& rhs) const
    {
        return {
            w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
            w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
            w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x,
            w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
        };
    }

    float dot(const quat_t& b) const
    {
        return x * b.x + y * b.y + z * b.z + w * b.w;
    }

    bool empty() const
    {
        return x == 0.f && y == 0.f && z == 0.f && w == 0.f;
    }

    vec3_t operator*(const vec3_t& point) const
    {
        float num = x * 2.f;
        float num2 = y * 2.f;
        float num3 = z * 2.f;
        float num4 = x * num;
        float num5 = y * num2;
        float num6 = z * num3;
        float num7 = x * num2;
        float num8 = x * num3;
        float num9 = y * num3;
        float num10 = w * num;
        float num11 = w * num2;
        float num12 = w * num3;

        return {
            (1.f - (num5 + num6)) * point.x + (num7 - num12) * point.y + (num8 + num11) * point.z,
            (num7 + num12) * point.x + (1.f - (num4 + num6)) * point.y + (num9 - num10) * point.z,
            (num8 - num11) * point.x + (num9 + num10) * point.y + (1.f - (num4 + num5)) * point.z
        };
    }

    quat_t look_rot(const vec3_t& forward) const
    {
        vec3_t vector = forward.unity_normalize();
        vec3_t vector2 = vec3_t(0.f, 1.f, 0.f).cross(vector).unity_normalize();
        vec3_t vector3 = vector.cross(vector2);

        float m00 = vector2.x, m01 = vector2.y, m02 = vector2.z;
        float m10 = vector3.x, m11 = vector3.y, m12 = vector3.z;
        float m20 = vector.x, m21 = vector.y, m22 = vector.z;

        float num8 = (m00 + m11) + m22;

        quat_t q{};

        if (num8 > 0.f)
        {
            float num = sqrtf(num8 + 1.f);
            q.w = num * 0.5f;
            num = 0.5f / num;
            q.x = (m12 - m21) * num;
            q.y = (m20 - m02) * num;
            q.z = (m01 - m10) * num;
            return q;
        }

        if (m00 >= m11 && m00 >= m22)
        {
            float num7 = sqrtf(((1.f + m00) - m11) - m22);
            float num4 = 0.5f / num7;
            q.x = 0.5f * num7;
            q.y = (m01 + m10) * num4;
            q.z = (m02 + m20) * num4;
            q.w = (m12 - m21) * num4;
            return q;
        }

        if (m11 > m22)
        {
            float num6 = sqrtf(((1.f + m11) - m00) - m22);
            float num3 = 0.5f / num6;
            q.x = (m10 + m01) * num3;
            q.y = 0.5f * num6;
            q.z = (m21 + m12) * num3;
            q.w = (m20 - m02) * num3;
            return q;
        }

        float num5 = sqrtf(((1.f + m22) - m00) - m11);
        float num2 = 0.5f / num5;
        q.x = (m20 + m02) * num2;
        q.y = (m21 + m12) * num2;
        q.z = 0.5f * num5;
        q.w = (m01 - m10) * num2;
        return q;
    }
};