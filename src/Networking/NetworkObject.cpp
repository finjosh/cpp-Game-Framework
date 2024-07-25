#include "Networking/NetworkObject.hpp"
#include "Networking/NetworkObjectManager.hpp"

sf::Uint64 NetworkObject::m_nextID = 1;

NetworkObject::NetworkObject(uint64_t type) : m_typeid(type) {}

NetworkObject::~NetworkObject()
{
    removeNetworkObject();
}

sf::Uint64 NetworkObject::getNetworkID() const
{
    return m_id;
}

uint64_t NetworkObject::getTypeid() const
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
