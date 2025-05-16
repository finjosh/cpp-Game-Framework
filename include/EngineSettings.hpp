#ifndef ENGINE_SETTINGS_HPP
#define ENGINE_SETTINGS_HPP

#pragma once

#define PIXELS_PER_METER 10.0f

/// @brief draws from low to high
enum class DrawStage
{
    Particles = 0,
    Default = 10,
    GUI = 20,
};

#endif
