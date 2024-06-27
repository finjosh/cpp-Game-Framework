#include "Networking/NetworkObject.hpp"
#include "Networking/NetworkObjectManager.hpp"

bool _networkObjectComp::operator() (const NetworkObject* lhs, const NetworkObject* rhs) const
{
    return lhs->getNetworkID() < rhs->getNetworkID();
}

sf::Uint32 NetworkObject::m_nextID = 1;

NetworkObject::NetworkObject() : m_id(m_nextID++) {}

NetworkObject::~NetworkObject()
{
    if (m_id == 0) // only required in the deconstructor
        return;
    // removeNetworkObject();
}

sf::Uint32 NetworkObject::getNetworkID() const
{
    return m_id;
}

// size_t NetworkObject::getTypeid() const
// {
//     return _typeid;
// }

// void NetworkObject::createNetworkObject()
// {
//     _id = _nextID++;
//     NetworkObjectManager::createNetworkObject(this);
// }

// void NetworkObject::removeNetworkObject()
// {
//     NetworkObjectManager::removeNetworkObject(this);
//     _id = 0;
// }

