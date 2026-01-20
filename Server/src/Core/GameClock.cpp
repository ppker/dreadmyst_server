// Game Clock - Tick-based timing system
// Task 2.9: Tick-Based Update System

#include "stdafx.h"
#include "Core/GameClock.h"
#include "Core/Logger.h"

GameClock& GameClock::instance()
{
    static GameClock instance;
    return instance;
}

void GameClock::start()
{
    m_startTime = Clock::now();
    m_lastTickTime = m_startTime;
    m_currentTime = m_startTime;
    m_tickCount = 0;
    m_accumulator = 0.0f;
    m_started = true;

    LOG_INFO("Game clock started (tick rate: %d/sec, interval: %.0fms)",
             m_tickRate, m_tickInterval * 1000.0f);
}

bool GameClock::tick()
{
    if (!m_started) {
        start();
    }

    m_currentTime = Clock::now();

    // Calculate time since last frame
    auto elapsed = std::chrono::duration<float>(m_currentTime - m_lastTickTime);
    float frameTime = elapsed.count();

    // Cap frame time to prevent spiral of death
    const float maxFrameTime = 0.25f; // 250ms max
    if (frameTime > maxFrameTime) {
        m_wasLagging = true;
        m_lagAmount = frameTime - maxFrameTime;
        frameTime = maxFrameTime;
        LOG_WARN("Game clock lagging by %.1fms", m_lagAmount * 1000.0f);
    } else {
        m_wasLagging = false;
        m_lagAmount = 0.0f;
    }

    m_accumulator += frameTime;

    // Check if we should tick
    if (m_accumulator >= m_tickInterval) {
        m_deltaTime = m_tickInterval;
        m_accumulator -= m_tickInterval;
        m_tickCount++;
        m_lastTickTime = m_currentTime;
        return true;
    }

    return false;
}

double GameClock::getElapsedTime() const
{
    auto elapsed = std::chrono::duration<double>(Clock::now() - m_startTime);
    return elapsed.count();
}

std::string GameClock::getUptimeString() const
{
    double seconds = getElapsedTime();

    int days = static_cast<int>(seconds / 86400);
    seconds -= days * 86400;

    int hours = static_cast<int>(seconds / 3600);
    seconds -= hours * 3600;

    int minutes = static_cast<int>(seconds / 60);
    int secs = static_cast<int>(seconds) % 60;

    char buf[64];
    if (days > 0) {
        std::snprintf(buf, sizeof(buf), "%dd %02d:%02d:%02d", days, hours, minutes, secs);
    } else {
        std::snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hours, minutes, secs);
    }
    return buf;
}

void GameClock::setTickRate(int ticksPerSecond)
{
    if (ticksPerSecond <= 0) {
        ticksPerSecond = DEFAULT_TICK_RATE;
    }
    m_tickRate = ticksPerSecond;
    m_tickInterval = 1.0f / static_cast<float>(ticksPerSecond);
}
