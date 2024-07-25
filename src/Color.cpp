#include "Color.hpp"

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

bool Color::operator==(const Color& rhs) const
{
    return (r == rhs.r)
        && (g == rhs.g)
        && (b == rhs.b)
        && (a == rhs.a);
}

bool Color::operator!=(const Color& right) const
{
    return !(*this == right);
}

Color Color::operator + (const Color& color) const
{
    return Color{(uint8_t)(r + color.r), (uint8_t)(g + color.g), (uint8_t)(b + color.b), (uint8_t)(a + color.a)};
}

void Color::operator += (const Color& color)
{
    r += color.r; g += color.g; b += color.b; a += color.a;
}

Color Color::operator - (const Color& color) const
{
    return Color{(uint8_t)(r - color.r), (uint8_t)(g - color.g), (uint8_t)(b - color.b), (uint8_t)(a - color.a)};
}

void Color::operator -= (const Color& color)
{
    r -= color.r; g -= color.g; b -= color.b; a -= color.a;
}

Color Color::operator * (const Color& color) const
{
    return Color{(uint8_t)(r * color.r), (uint8_t)(g * color.g), (uint8_t)(b * color.b), (uint8_t)(a * color.a)};
}

void Color::operator *= (const Color& color)
{
    r *= color.r; g *= color.g; b *= color.b; a *= color.a;
}

Color Color::operator / (const Color& color) const
{
    return Color{(uint8_t)(r / color.r), (uint8_t)(g / color.g), (uint8_t)(b / color.b), (uint8_t)(a / color.a)};
}

void Color::operator /= (const Color& color)
{
    r /= color.r; g /= color.g; b /= color.b; a /= color.a;
}

Color Color::operator * (float scaler) const
{
    return Color{(uint8_t)(r * scaler), (uint8_t)(g * scaler), (uint8_t)(b * scaler), (uint8_t)(a * scaler)};
}

void Color::operator *= (float scaler)
{
    r *= scaler; g *= scaler; b *= scaler; a *= scaler;
}

Color Color::operator / (float scaler) const
{
    return Color{(uint8_t)(r / scaler), (uint8_t)(g / scaler), (uint8_t)(b / scaler), (uint8_t)(a / scaler)};
}

void Color::operator /= (float scaler)
{
    r /= scaler; g /= scaler; b /= scaler; a /= scaler;
}
