#ifndef CONTACT_DATA_ARRAY_HPP
#define CONTACT_DATA_ARRAY_HPP

#pragma once

#include "ContactData.hpp"
#include <iterator>

class Collider;

/// @note looping through all contact data for each body
/// @note is said to be less efficient then using the contact events 
/// @note from the world but this is more effective for easy of use 
/// @warning dont ever store this object
class ContactDataArray {
public:
    // Iterator class definition
    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = ContactData;
        using difference_type = std::ptrdiff_t;
        using pointer = ContactData*;
        using reference = ContactData&;

        iterator(b2ContactData* ptr, const Collider* owner) : m_ptr(ptr), m_owner(owner) {}

        ContactData operator*() { /* return ContactData(m_ptr, m_owner); */ return ContactData(m_ptr->shapeIdA, m_ptr->shapeIdB); }
        iterator& operator++() { m_ptr++; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
        iterator& operator--() { m_ptr--; return *this; }
        iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }

        friend bool operator==(const iterator& a, const iterator& b) { return a.m_ptr == b.m_ptr; }
        friend bool operator!=(const iterator& a, const iterator& b) { return a.m_ptr != b.m_ptr; }

    private:
        b2ContactData* m_ptr;
        const Collider* m_owner = nullptr;
    };

    /// @note transfers ownership of the array to the new object
    ContactDataArray(ContactDataArray&& other);
    ContactDataArray& operator=(ContactDataArray&& other);
    ~ContactDataArray();
    int size() const;
    const ContactData getContactData(int index) const;
    ContactData getContactData(int index);
    const ContactData operator[](int index) const;
    ContactData operator[](int index);

    // iterator methods
    iterator begin() { return iterator(m_dataArray, m_owner); }
    iterator end() { return iterator(m_dataArray + m_capacity, m_owner); }

private:
    friend Collider;

    ContactDataArray(ContactDataArray& other) = delete;
    ContactDataArray& operator=(ContactDataArray& other) = delete;

    ContactDataArray(int capacity, const Collider* owner);
    b2ContactData* getContactArray();

    b2ContactData* m_dataArray = nullptr;
    const Collider* m_owner = nullptr;
    int m_capacity = 0;
};

#endif
