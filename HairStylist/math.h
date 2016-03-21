#pragma once
#include <algorithm>
#include <glm/glm.hpp>

namespace math
{
	const float PI = 3.14159265359f;
	const float EPSILON = 0.0000001f;

    const float PI2 = PI * 2.0f;
    const float PI_DIV_2  = PI * 0.5f;
    const float PI_DIV_4  = PI * 0.25f;

    void rotate(glm::vec2& p, const float&  angle);
	inline float lerp(float start, float end, float t) { return (1 - t)*start + t*end; }
    glm::vec2 lerp(const glm::vec2& start, const glm::vec2& end, float t);

	inline float clamp(float v, float min, float max) { return v < min ? min : v > max ? max : v; }

	inline float smoothstep(float start, float end, float t) { return start + t*t*(3 - 2 * t) * (end - start); }
    glm::vec2 smoothstep(const glm::vec2& start, const glm::vec2& end, float t);

	inline float smootherstep(float start, float end, float t) { return start + t*t*t*(t*(t * 6 - 15) + 10) * (end-start); }

	inline float regress(float start, float end, float t) { return start + (sqrtf(t)*2.f - t)*(end-start); }
    glm::vec2 regress(const glm::vec2& start, const glm::vec2& end, float t);

	inline float min(float v0, float v1, float v2) { return std::min(v0, std::min(v1, v2)); }
	inline float max(float v0, float v1, float v2) { return std::max(v0, std::max(v1, v2)); }

	inline float toRadians(float degree) { return degree * PI / 180.f; }
	inline float toDegrees(float radians) { return radians * 180.f / PI; }

    inline bool nearEq(float n1, float n2) { return std::abs(n1 - n2) <= EPSILON; }

    glm::mat4 rotationX(float angle);
    glm::mat4 rotationY(float angle);
    glm::mat4 rotationZ(float angle);

    inline glm::mat4 identityMatrix()
    {
        return glm::mat4(
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f);
    }

    inline glm::mat4 rotationX90Deg()
    {
        return glm::mat4(1.f, 0.f, 0.f, 0.f,
                         0.f, 0, 1.f, 0.f,
                         0.f, -1.f, 0, 0.f,
                         0.f, 0.f, 0.f, 1.f);
    }

    inline glm::mat4 rotationY90Deg()
    {
        return glm::mat4(0.f, 0.f, -1.f, 0.f,
                         0.f, 1.f, 0.f, 0.f,
                         1.f, 0.f, 0.f, 0.f,
                         0.f, 0.f, 0.f, 1.f);
    }

    inline glm::mat4 rotationZ90Deg()
    {
        return glm::mat4(0.f, 1.f, 0.f, 0.f,
                         -1.f, 0.f, 0.f, 0.f,
                         0.f, 0.f, 1.f, 0.f,
                         0.f, 0.f, 0.f, 1.f);
    }
    inline glm::mat4 rotationXMinus90Deg()
    {
        return glm::mat4(1.f, 0.f, 0.f, 0.f,
                         0.f, 0, -1.f, 0.f,
                         0.f, 1.f, 0, 0.f,
                         0.f, 0.f, 0.f, 1.f);
    }

    inline glm::mat4 rotationYMinus90Deg()
    {
        return glm::mat4(0.f, 0.f, 1.f, 0.f,
                         0.f, 1.f, 0.f, 0.f,
                         -1.f, 0.f, 0.f, 0.f,
                         0.f, 0.f, 0.f, 1.f);
    }

    inline glm::mat4 rotationZMinus90Deg()
    {
        return glm::mat4(0.f, -1.f, 0.f, 0.f,
                         1.f, 0.f, 0.f, 0.f,
                         0.f, 0.f, 1.f, 0.f,
                         0.f, 0.f, 0.f, 1.f);
    }
}
