#ifndef ENGINE_SETTINGS_HPP
#define ENGINE_SETTINGS_HPP

#pragma once

// TODO do this better

const float PIXELS_PER_METER = 10.0F;

/// @brief draws from low to high
enum DrawStage
{
    Particles = 0,
    Default = 10,
    UI = 20,
};

#endif
