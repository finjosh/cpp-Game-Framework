#include "Networking/NetworkObject.hpp"
#include "Networking/NetworkObjectManager.hpp"

std::uint64_t NetworkObject::m_nextID = 1;

NetworkObject::NetworkObject(std::uint64_t type) : m_typeid(type) {}

NetworkObject::~NetworkObject()
{
    removeNetworkObject();
}

std::uint64_t NetworkObject::getNetworkID() const
{
    return m_id;
}

std::uint64_t NetworkObject::getTypeid() const
{
    return m_typeid;
}

void NetworkObject::createNetworkObject()
{
    m_id = m_nextID++;
    NetworkObjectManager::createNetworkObject(this);
}

void NetworkObject::removeNetworkObject()
{
    NetworkObjectManager::removeNetworkObject(this);
    m_id = 0;
}
