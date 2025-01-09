#include "Physics/ContactDataArray.hpp" 

// public 

ContactDataArray::ContactDataArray(ContactDataArray&& other)
{
    if (this != &other) {
        m_dataArray = other.m_dataArray;
        m_capacity = other.m_capacity;
        m_owner = other.m_owner;
        other.m_dataArray = nullptr;
        other.m_capacity = 0;
    }
}

ContactDataArray& ContactDataArray::operator=(ContactDataArray&& other) {
    if (this != &other) {
        delete[] m_dataArray;
        m_dataArray = other.m_dataArray;
        m_owner = other.m_owner;
        m_capacity = other.m_capacity;
        other.m_dataArray = nullptr;
        other.m_capacity = 0;
    }
    return *this;
}

int ContactDataArray::size() const
{
    return m_capacity;
}

ContactData ContactDataArray::getContactData(int index)
{
    assert(index >= 0 && index < m_capacity && "ContactData index must be in range!");
    // return ContactData(&m_dataArray[index], m_owner);
    return ContactData(m_dataArray[index].shapeIdA, m_dataArray[index].shapeIdB); // TODO remove these as they do not give the proper data
}

const ContactData ContactDataArray::getContactData(int index) const
{
    assert(index >= 0 && index < m_capacity && "ContactData index must be in range!");
    // return ContactData(&m_dataArray[index], m_owner);
    return ContactData(m_dataArray[index].shapeIdA, m_dataArray[index].shapeIdB); // TODO remove these as they do not give the proper data
}

ContactData ContactDataArray::operator[](int index)
{
    assert(index >= 0 && index < m_capacity && "ContactData index must be in range!");
    // return ContactData(&m_dataArray[index], m_owner);
    return ContactData(m_dataArray[index].shapeIdA, m_dataArray[index].shapeIdB); // TODO remove these as they do not give the proper data
}

const ContactData ContactDataArray::operator[](int index) const
{
    assert(index >= 0 && index < m_capacity && "ContactData index must be in range!");
    // return ContactData(&m_dataArray[index], m_owner);
    return ContactData(m_dataArray[index].shapeIdA, m_dataArray[index].shapeIdB); // TODO remove these as they do not give the proper data
}

// private

ContactDataArray::ContactDataArray(int capacity, const Collider* owner)
{
    m_owner = owner;
    m_dataArray = new b2ContactData[capacity];
    m_capacity = capacity;
}

b2ContactData* ContactDataArray::getContactArray()
{
    return m_dataArray;
}

ContactDataArray::~ContactDataArray()
{
    if (m_dataArray == nullptr)
        return;
        
    delete[] m_dataArray;
    m_dataArray = nullptr;
    m_capacity = 0;
}
