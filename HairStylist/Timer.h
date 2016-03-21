#pragma once

class Timer
{
public:
    Timer();

    void start();
    float totalTime();

    /**
     * @return deltaTime
     */
    float tick();
    float deltaTime();

private:
    float m_prevTime;
    float m_deltaTime;
    float m_totalTime;
};

class Time
{
public:
    static void update();

public:
    static float deltaTime;
    static float totalTime;
private:
    static Timer m_timer;
};
