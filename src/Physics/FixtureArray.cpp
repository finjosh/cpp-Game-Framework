#include "Physics/FixtureArray.hpp"

// public 

FixtureArray::FixtureArray(FixtureArray&& other)
{
    if (this != &other)
    {
        m_shapeArray = other.m_shapeArray;
        m_capacity = other.m_capacity;
        other.m_capacity = 0;
        other.m_shapeArray = nullptr;
    }
}

FixtureArray& FixtureArray::operator=(FixtureArray&& other)
{
    if (this != &other)
    {
        m_shapeArray = other.m_shapeArray;
        m_capacity = other.m_capacity;
        other.m_capacity = 0;
        other.m_shapeArray = nullptr;
    }
    return *this;
}

int FixtureArray::size() const
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
    if (m_shapeArray == nullptr)
        return;
        
    delete[] m_shapeArray;
    m_shapeArray = nullptr;
    m_capacity = 0;
}
