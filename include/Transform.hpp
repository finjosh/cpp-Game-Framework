#ifndef TRANSFORM_H
#define TRANSFORM_H

#pragma once

#include "Vector2.hpp"

class Transform
{
public:
    inline Transform() = default;
    Transform(const Vector2& position, float rotation);
	void set(const Vector2& position, float angle);

    Vector2 position = Vector2{0,0};
    float rotation = 0.f; // TODO store rotation like this? std::complex<float> polar = std::polar<float>(1.0, rot);
protected:

private:

};

#endif
