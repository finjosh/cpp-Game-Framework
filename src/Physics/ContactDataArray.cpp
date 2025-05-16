#include "Physics/ContactDataArray.hpp" 

#define GET_COLLIDER(b2Shape) ((Collider*)b2Body_GetUserData(b2Shape_GetBody(b2Shape)))

ContactData ContactDataArray::iterator::operator*() 
{ 
    if (m_owner == GET_COLLIDER(m_ptr->shapeIdA))
        return ContactData(m_ptr->shapeIdA, m_ptr->shapeIdB, &m_ptr->manifold); 
    else
        return ContactData(m_ptr->shapeIdA, m_ptr->shapeIdB, &m_ptr->manifold); 
}

ContactDataArray::iterator::iterator(b2ContactData* ptr, const Collider* owner) : m_ptr(ptr), m_owner(owner) {}
ContactDataArray::iterator& ContactDataArray::iterator::operator++() { m_ptr++; return *this; }
ContactDataArray::iterator ContactDataArray::iterator::operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
ContactDataArray::iterator& ContactDataArray::iterator::operator--() { m_ptr--; return *this; }
ContactDataArray::iterator ContactDataArray::iterator::operator--(int) { iterator tmp = *this; --(*this); return tmp; }
bool operator==(const ContactDataArray::iterator& a, const ContactDataArray::iterator& b) { return a.m_ptr == b.m_ptr; }
bool operator!=(const ContactDataArray::iterator& a, const ContactDataArray::iterator& b) { return a.m_ptr != b.m_ptr; }

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
    if (m_owner == GET_COLLIDER(m_dataArray[index].shapeIdA))
        return ContactData(m_dataArray[index].shapeIdA, m_dataArray[index].shapeIdB, &m_dataArray[index].manifold);
    else // the only other option is that shape B is from the collider this data is from
        return ContactData(m_dataArray[index].shapeIdB, m_dataArray[index].shapeIdA, &m_dataArray[index].manifold);
}

const ContactData ContactDataArray::getContactData(int index) const
{
    assert(index >= 0 && index < m_capacity && "ContactData index must be in range!");
    if (m_owner == GET_COLLIDER(m_dataArray[index].shapeIdA))
        return ContactData(m_dataArray[index].shapeIdA, m_dataArray[index].shapeIdB, &m_dataArray[index].manifold);
    else // the only other option is that shape B is from the collider this data is from
        return ContactData(m_dataArray[index].shapeIdB, m_dataArray[index].shapeIdA, &m_dataArray[index].manifold);
}

ContactData ContactDataArray::operator[](int index)
{
    assert(index >= 0 && index < m_capacity && "ContactData index must be in range!");
    if (m_owner == GET_COLLIDER(m_dataArray[index].shapeIdA))
        return ContactData(m_dataArray[index].shapeIdA, m_dataArray[index].shapeIdB, &m_dataArray[index].manifold);
    else // the only other option is that shape B is from the collider this data is from
        return ContactData(m_dataArray[index].shapeIdB, m_dataArray[index].shapeIdA, &m_dataArray[index].manifold);
}

const ContactData ContactDataArray::operator[](int index) const
{
    assert(index >= 0 && index < m_capacity && "ContactData index must be in range!");
    if (m_owner == GET_COLLIDER(m_dataArray[index].shapeIdA))
        return ContactData(m_dataArray[index].shapeIdA, m_dataArray[index].shapeIdB, &m_dataArray[index].manifold);
    else // the only other option is that shape B is from the collider this data is from
        return ContactData(m_dataArray[index].shapeIdB, m_dataArray[index].shapeIdA, &m_dataArray[index].manifold);
}

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