#pragma once
#include <unordered_map>

class vec2_t
{
public:
    float x;
    float y;

    vec2_t() : x(0.f), y(0.f)
    {
    }

    vec2_t(float x, float y) : x(x), y(y)
    {
    }

    vec2_t& operator+=(const vec2_t& vector)
    {
        x += vector.x;
        y += vector.y;

        return *this;
    }

    vec2_t operator+(const vec2_t& vector) const
    {
        return vec2_t(x + vector.x, y + vector.y);
    }

    vec2_t operator*(const vec2_t& vector) const
    {
        return vec2_t(x * vector.x, y * vector.y);
    }

    vec2_t operator*(float number) const
    {
        return vec2_t(x * number, y * number);
    }

    vec2_t operator-(const vec2_t& vector) const
    {
        return vec2_t(x - vector.x, y - vector.y);
    }

    vec2_t operator/(float number) const
    {
        return vec2_t(x / number, y / number);
    }

    float length() const
    {
        return sqrtf((x * x) + (y * y));
    }

    float distance_to(vec2_t out) const
    {
        float num = x - out.x;
        float num2 = y - out.y;
        return sqrtf(num * num + num2 * num2);
    }

    vec2_t normalized() const
    {
        float len = length();
        return vec2_t(x / len, y / len);
    }

    vec2_t rotate(float angle)
    {
        vec2_t rotated_point;
        rotated_point.x = x * cosf(angle) - y * sinf(angle);
        rotated_point.y = x * sinf(angle) + y * cosf(angle);
        return rotated_point;
    }

    bool empty() const
    {
        return x == 0 && y == 0;
    }

    ImVec2 get() const
    {
        return ImVec2(x, y);
    }
};

class vec3_t
{
public:
    float x;
    float y;
    float z;

    vec3_t() : x(0.f), y(0.f), z(0.f)
    {
    }

    vec3_t(float x, float y, float z) : x(x), y(y), z(z)
    {
    }

    vec3_t operator+(const vec3_t& vector) const
    {
        return vec3_t(x + vector.x, y + vector.y, z + vector.z);
    }

    vec3_t operator-(const vec3_t& vector) const
    {
        return vec3_t(x - vector.x, y - vector.y, z - vector.z);
    }

    vec3_t operator-() const
    {
        return { -x, -y, -z };
    }

    vec3_t operator*(const vec3_t& vector) const
    {
        return vec3_t(x * vector.x, y * vector.y, z * vector.z);
    }

    vec3_t operator*(float number) const
    {
        return vec3_t(x * number, y * number, z * number);
    }

    vec3_t operator/(float number) const
    {
        return vec3_t(x / number, y / number, z / number);
    }

    bool operator==(vec3_t vec) const
    {
        return (x == vec.x && y == vec.y && z == vec.z);
    }
    bool operator!=(vec3_t vec) const
    {
        return (x != vec.x && y != vec.y && z != vec.z);
    }

    vec3_t& operator+=(const vec3_t& vector)
    {
        x += vector.x;
        y += vector.y;
        z += vector.z;

        return *this;
    }

    vec3_t& operator-=(const vec3_t& vector)
    {
        x -= vector.x;
        y -= vector.y;
        z -= vector.z;

        return *this;
    }

    vec3_t& operator*=(float number)
    {
        x *= number;
        y *= number;
        z *= number;

        return *this;
    }

    vec3_t& operator/=(float number)
    {
        x /= number;
        y /= number;
        z /= number;

        return *this;
    }

    bool empty() const
    {
        return x == 0 && y == 0 && z == 0;
    }

    float length() const
    {
        return sqrtf((x * x) + (y * y) + (z * z));
    }
    float magnitude() const
    {
        return sqrtf((x * x + y * y + z * z));
    }
    float magnitude2d() const
    {
        return sqrtf((x * x + z * z));
    }

    vec3_t xz3d() const
    {
        return vec3_t(x, 0.f, z);
    }

    float clamp01(float value)
    {
        if (value < 0.f)
            return 0.f;
        else if (value > 1.f)
            return 1.f;
        else
            return value;
    }

    vec3_t lerp(vec3_t a, vec3_t b, float t)
    {
        t = clamp01(t);
        return vec3_t(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t);
    }

    float inverse_lerp(float a, float b, float value)
    {
        float result;
        if (a != b)
        {
            result = clamp01((value - a) / (b - a));
        }
        else
        {
            result = 0.f;
        }
        return result;
    }

    vec3_t normalized()
    {
        float len = length();
        return vec3_t(x / len, y / len, z / len);
    }

    vec3_t cross(vec3_t rhs)
    {
        return vec3_t(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
    }

    vec3_t unity_normalize()
    {
        vec3_t tis(x, y, z);
        float num = vec3_t(x, y, z).magnitude();
        if (num > 1E-05f)
        {
            tis /= num;
        }
        else
        {
            tis = vec3_t(0, 0, 0);
        }
        x = tis.x;
        y = tis.y;
        z = tis.z;

        return { x,y,z };
    }

    float clamp(float value, float min, float max)
    {
        bool flag = value < min;
        if (flag)
        {
            value = min;
        }
        else
        {
            bool flag2 = value > max;
            if (flag2)
            {
                value = max;
            }
        }
        return value;
    }

    auto xz()
    {
        return vec3_t(x, 0, z);
    }

#define powFF(n) (n)*(n)

    float distance_to(const vec3_t& vector) const
    {
        return sqrtf(powFF(x - vector.x) + powFF(y - vector.y) + powFF(z - vector.z));
    };
    float dot(const vec3_t& vector) const
    {
        return x * vector.x + y * vector.y + z * vector.z;
    }

    float length_2d() { return sqrt((x * x) + (z * z)); }


    ImVec2 get() const
    {
        return ImVec2(x, y);
    }
};

class quat_t
{
public:
    float x, y, z, w;

    quat_t()
    {
        this->x = 0.0f;
        this->y = 0.0f;
        this->z = 0.0f;
        this->w = 0.0f;
    }

    quat_t(float x, float y, float z, float w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    quat_t operator*(quat_t rhs)
    {
        return quat_t(
            this->w * rhs.x + this->x * rhs.w + this->y * rhs.z - this->z * rhs.y,
            this->w * rhs.y + this->y * rhs.w + this->z * rhs.x - this->x * rhs.z,
            this->w * rhs.z + this->z * rhs.w + this->x * rhs.y - this->y * rhs.x,
            this->w * rhs.w - this->x * rhs.x - this->y * rhs.y - this->z * rhs.z
        );
    }

    float dot(quat_t b)
    {
        return x * x + y * y + z * z + w * w;
    }

    bool empty() const
    {
        return (this->x == 0 && this->y == 0 && this->w == 0 && this->z == 0);
    }

    vec3_t operator*(vec3_t point)
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

        vec3_t result{ };
        result.x = (1.f - (num5 + num6)) * point.x + (num7 - num12) * point.y + (num8 + num11) * point.z;
        result.y = (num7 + num12) * point.x + (1.f - (num4 + num6)) * point.y + (num9 - num10) * point.z;
        result.z = (num8 - num11) * point.x + (num9 + num10) * point.y + (1.f - (num4 + num5)) * point.z;
        return result;
    }

    quat_t look_rot(vec3_t forward)
    {

        vec3_t vector = forward.unity_normalize();
        vec3_t vector2 = (vec3_t(0.f, 1.f, 0.f)).cross(vector).unity_normalize();
        vec3_t vector3 = (vector).cross(vector2);
        auto m00 = vector2.x;
        auto m01 = vector2.y;
        auto m02 = vector2.z;
        auto m10 = vector3.x;
        auto m11 = vector3.y;
        auto m12 = vector3.z;
        auto m20 = vector.x;
        auto m21 = vector.y;
        auto m22 = vector.z;


        float num8 = (m00 + m11) + m22;
        auto quaternion = quat_t();
        if (num8 > 0.f)
        {
            auto num = (float)sqrtf(num8 + 1.f);
            quaternion.w = num * 0.5f;
            num = 0.5f / num;
            quaternion.x = (m12 - m21) * num;
            quaternion.y = (m20 - m02) * num;
            quaternion.z = (m01 - m10) * num;
            return quaternion;
        }
        if ((m00 >= m11) && (m00 >= m22))
        {
            auto num7 = (float)sqrtf(((1.f + m00) - m11) - m22);
            auto num4 = 0.5f / num7;
            quaternion.x = 0.5f * num7;
            quaternion.y = (m01 + m10) * num4;
            quaternion.z = (m02 + m20) * num4;
            quaternion.w = (m12 - m21) * num4;
            return quaternion;
        }
        if (m11 > m22)
        {
            auto num6 = (float)sqrtf(((1.f + m11) - m00) - m22);
            auto num3 = 0.5f / num6;
            quaternion.x = (m10 + m01) * num3;
            quaternion.y = 0.5f * num6;
            quaternion.z = (m21 + m12) * num3;
            quaternion.w = (m20 - m02) * num3;
            return quaternion;
        }
        auto num5 = (float)sqrtf(((1.f + m22) - m00) - m11);
        auto num2 = 0.5f / num5;
        quaternion.x = (m20 + m02) * num2;
        quaternion.y = (m21 + m12) * num2;
        quaternion.z = 0.5f * num5;
        quaternion.w = (m01 - m10) * num2;
        return quaternion;
    }
};