#include "math.h"

void math::rotate(glm::vec2& p, const float& angle)
{
	const float x = p.x;
	const float c = cosf(angle);
	const float s = sinf(angle);

	p.x = x*c - p.y*s;
	p.y = p.y*c + x*s;
}


glm::vec2 math::lerp(const glm::vec2& start, const glm::vec2& end, float t)
{
    glm::vec2 result;
    result.x = math::lerp(start.x, end.x, t);
    result.y = math::lerp(start.y, end.y, t);

	return result;
}

glm::vec2 math::smoothstep(const glm::vec2& start, const glm::vec2& end, float t)
{
    glm::vec2 result;
	float m = t*t*(3 - 2 * t);
	result.x = start.x + m*(end.x - start.x);
	result.y = start.y + m*(end.y - start.y);

	return result;
}

glm::vec2 math::regress(const glm::vec2& start, const glm::vec2& end, float t)
{
    glm::vec2 result;
	float m = sqrtf(t)*2.f - t;
	result.x = start.x + m*(end.x - start.x);
	result.y = start.y + m*(end.y - start.y);

	return result;
}

glm::mat4 math::rotationX(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    return glm::mat4(1.f, 0.f, 0.f, 0.f,
                     0.f, c, s, 0.f,
                     0.f, -s, c, 0.f,
                     0.f, 0.f, 0.f, 1.f);
}

glm::mat4 math::rotationY(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    return glm::mat4(c, 0.f, -s, 0.f,
                     0.f, 1.f, 0.f, 0.f,
                     s, 0.f, c, 0.f,
                     0.f, 0.f, 0.f, 1.f);
}

glm::mat4 math::rotationZ(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    return glm::mat4(c, s, 0.f, 0.f,
                     -s, c, 0.f, 0.f,
                     0.f, 0.f, 1.f, 0.f,
                     0.f, 0.f, 0.f, 1.f);
}
