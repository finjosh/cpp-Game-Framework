#include "Physics/FixtureArray.hpp"

// public 

int FixtureArray::getSize() const
{
    return m_capacity;
}

Fixture FixtureArray::getFixture(int index)
{
    assert(index >= 0 && index < m_capacity && "Fixture index must be in range!");
    return Fixture(m_shapeArray[index]);
}

const Fixture FixtureArray::getFixture(int index) const
{
    assert(index >= 0 && index < m_capacity && "Fixture index must be in range!");
    return Fixture(m_shapeArray[index]);
}

Fixture FixtureArray::operator[](int index)
{
    assert(index >= 0 && index < m_capacity && "Fixture index must be in range!");
    return Fixture(m_shapeArray[index]);
}

const Fixture FixtureArray::operator[](int index) const
{
    assert(index >= 0 && index < m_capacity && "Fixture index must be in range!");
    return Fixture(m_shapeArray[index]);
}

// private

FixtureArray::FixtureArray(int capacity)
{
    m_shapeArray = new b2ShapeId[capacity];
    m_capacity = capacity;
}

b2ShapeId* FixtureArray::getShapeArray()
{
    return m_shapeArray;
}

FixtureArray::~FixtureArray()
{
    delete[] m_shapeArray;
    m_shapeArray = nullptr;
    m_capacity = 0;
}
