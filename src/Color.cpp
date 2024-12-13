#include "Color.hpp"
#include <sstream>
#include "Utils/StringHelper.hpp"

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

std::string Color::toString() const
{
    return "rgba(" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + "," + std::to_string(a) + ")";
}

std::string Color::toHexString() const
{
    return "#" + StringHelper::intToHex(this->r) + StringHelper::intToHex(this->g) + StringHelper::intToHex(this->b);
}

Color Color::fromString(const std::string& str, Color defaultColor)
{
    std::string string = StringHelper::toLower_copy(StringHelper::trim_copy(str));

    if (string.starts_with("rgba("))
        string.erase(0,5);
    else
        return defaultColor;

    if (string.ends_with(')'))
        string.erase(string.size()-1);

    auto tokens = StringHelper::tokenize(string, ',');

    if (tokens.size() == 4)
    {
        return Color{static_cast<std::uint8_t>(StringHelper::toInt(tokens[0], defaultColor.r)),
                     static_cast<std::uint8_t>(StringHelper::toInt(tokens[1], defaultColor.g)),
                     static_cast<std::uint8_t>(StringHelper::toInt(tokens[2], defaultColor.b)),
                     static_cast<std::uint8_t>(StringHelper::toInt(tokens[3], defaultColor.a))};
    }

    return defaultColor;
}
