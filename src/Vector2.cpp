#include "Vector2.hpp"

Vector2::Vector2(float x, float y) : x(x), y(y) {}
Vector2::Vector2(const b2Vec2& box2DVector) : x(box2DVector.x), y(box2DVector.y) {}

Vector2::Vector2(tgui::String str)
{
    if (str.empty())
    {
        std::cerr << "Failed to parse Vector2. String was empty.\n";
        return;
    }

    // Remove the brackets around the value
    if (((str.front() == '(') && (str.back() == ')')) || ((str.front() == '{') && (str.back() == '}')))
        str = str.substr(1, str.length() - 2);

    if (str.empty())
    {
        x = 0;
        y = 0;
        return;
    }

    auto commaPos = str.find(',');
    if (commaPos == std::string::npos)
    {
        std::cerr << "Failed to parse Vector2 '" + str + "'. Expected numbers separated with a comma.\n";
        return;
    }

    x = str.substr(0, commaPos).trim().toFloat();
    y = str.substr(commaPos + 1).trim().toFloat();
}

void Vector2::operator+=(const Vector2& vector)
{
    x += vector.x;
    y += vector.y;
}

Vector2 Vector2::operator+(const Vector2& vector) const
{
    return Vector2{x+vector.x, y+vector.y};
}

void Vector2::operator-=(const Vector2& vector)
{
    x -= vector.x;
    y -= vector.y;
}

Vector2 Vector2::operator-(const Vector2& vector) const
{
    return Vector2{x-vector.x, y-vector.y};
}

void Vector2::operator*=(float scaler)
{
    x *= scaler;
    y *= scaler;
}

Vector2 Vector2::operator*(float scaler) const
{
    return Vector2{x*scaler, y*scaler};
}

void Vector2::operator/=(float scaler)
{
    x /= scaler;
    y /= scaler;
}

Vector2 Vector2::operator/(float scaler) const
{
    return Vector2{x/scaler, y/scaler};
}

bool Vector2::operator==(const Vector2& vector) const
{
    return (x == vector.x && y == vector.y);
}

bool Vector2::operator!=(const Vector2& vector) const
{
    return (x != vector.x || y != vector.y);
}

void Vector2::operator=(const Vector2& vector)
{
    x = vector.x;
    y = vector.y;
}

void Vector2::operator+=(const b2Vec2& vector)
{
    x += vector.x;
    y += vector.y;
}

void Vector2::operator-=(const b2Vec2& vector)
{
    x -= vector.x;
    y -= vector.y;
}

void Vector2::operator=(const b2Vec2& vector)
{
    x = vector.x;
    y = vector.y;
}

b2Vec2 Vector2::getb2Vec2() const
{
    return b2Vec2{x,y};
}

bool Vector2::IsValid() const
{
    return std::isfinite(x) && std::isfinite(y);
}

float Vector2::Length() const
{
    return std::sqrt(x * x + y * y);
}

float Vector2::LengthSquared() const
{
    return x * x + y * y;
}

float Vector2::Normalize()
{
    float length = Length();
    if (length < b2_epsilon)
    {
        return 0.0f;
    }
    float invLength = 1.0f / length;
    x *= invLength;
    y *= invLength;

    return length;
}

tgui::String Vector2::toString() const
{
    return "(" + tgui::String::fromNumber(x) + ", " + tgui::String::fromNumber(y) + ")";
}
