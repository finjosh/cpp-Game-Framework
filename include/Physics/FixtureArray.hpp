#ifndef FIXTURE_ARRAY_HPP
#define FIXTURE_ARRAY_HPP

#pragma once

#include "Fixture.hpp"
#include <iterator>

class Collider;

/// @note dont ever store this object
class FixtureArray {
public:
    // Iterator class definition
    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Fixture;
        using difference_type = std::ptrdiff_t;
        using pointer = Fixture*;
        using reference = Fixture&;

        iterator(b2ShapeId* ptr) : m_ptr(ptr) {}

        Fixture operator*() { return Fixture(*m_ptr); }
        iterator& operator++() { m_ptr++; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
        iterator& operator--() { m_ptr--; return *this; }
        iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }

        friend bool operator==(const iterator& a, const iterator& b) { return a.m_ptr == b.m_ptr; }
        friend bool operator!=(const iterator& a, const iterator& b) { return a.m_ptr != b.m_ptr; }

    private:
        b2ShapeId* m_ptr;
    };

    /// @note transfers ownership of the array to the new object
    FixtureArray(FixtureArray&& other);
    /// @note transfers ownership of the array to the new object
    FixtureArray& operator=(FixtureArray&& other);
    ~FixtureArray();
    int size() const;
    const Fixture getFixture(int index) const;
    Fixture getFixture(int index);
    const Fixture operator[](int index) const;
    Fixture operator[](int index);

    // iterator methods
    iterator begin() { return iterator(m_shapeArray); }
    iterator end() { return iterator(m_shapeArray + m_capacity); }

private:
    friend Collider;

    FixtureArray(const FixtureArray& other) = delete;
    FixtureArray& operator=(const FixtureArray& other) = delete;

    FixtureArray(int capacity);
    b2ShapeId* getShapeArray();

    b2ShapeId* m_shapeArray = nullptr;
    int m_capacity = 0;
};

#endif
