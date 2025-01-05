#ifndef FIXTURE_ARRAY_HPP
#define FIXTURE_ARRAY_HPP

#pragma once

#include "Fixture.hpp"

class Collider;

class FixtureArray
{
public:
    int getSize() const;
    /// @note same as using the operator[] 
    /// @warning asserts if index is out of range
    const Fixture getFixture(int index) const;
    /// @note same as using the operator[] 
    /// @warning asserts if index is out of range
    Fixture getFixture(int index);
    /// @warning asserts if index is out of range
    const Fixture operator[](int index) const;
    /// @warning asserts if index is out of range
    Fixture operator[](int index);

private:
    friend Collider;

    FixtureArray(int capacity);
    b2ShapeId* getShapeArray();
    ~FixtureArray();

    b2ShapeId* m_shapeArray = nullptr;
    int m_capacity = 0;
};

#endif
