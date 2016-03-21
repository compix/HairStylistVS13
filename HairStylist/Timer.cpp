#include "Timer.h"
#include <SDL.h>

Timer Time::m_timer;
float Time::deltaTime;
float Time::totalTime;

Timer::Timer()
    :m_prevTime(SDL_GetTicks() / 1000.0f), m_deltaTime(0.f), m_totalTime(0.f)
{

}

void Timer::start()
{
    m_prevTime = SDL_GetTicks() / 1000.0f;
    m_deltaTime = 0.f;
    m_totalTime = 0.f;
}

float Timer::totalTime()
{
    return m_totalTime;
}

float Timer::tick()
{
    float curTime = SDL_GetTicks() / 1000.0f;

    m_deltaTime = curTime - m_prevTime;
    m_totalTime += m_deltaTime;

    m_prevTime = curTime;
    return m_deltaTime;
}

float Timer::deltaTime()
{
    return m_deltaTime;
}

void Time::update()
{
    m_timer.tick();
    deltaTime = m_timer.deltaTime();
    totalTime = m_timer.totalTime();
}
