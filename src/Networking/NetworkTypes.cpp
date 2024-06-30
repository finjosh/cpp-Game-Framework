#include "Networking/NetworkTypes.hpp"
#include "Networking/NetworkObject.hpp"
#include "Networking/NetworkObjectManager.hpp"

std::unordered_map<sf::Uint64, funcHelper::func<NetworkObject*>> NetworkType::m_factories;

bool NetworkType::initType(sf::Uint64 type, const funcHelper::func<NetworkObject*>& factoryFunc)
{
    return m_factories.emplace(type, factoryFunc).second;
}

NetworkObject* NetworkType::createObject(sf::Uint64 value, sf::Uint32 netID)
{
    auto iter = m_factories.find(value);
    if (iter == m_factories.end())
    {
        return nullptr;
    }
    else
    {
        NetworkObject* newObj = iter->second.invoke();
        newObj->m_id = netID;
        NetworkObjectManager::createNetworkObject(newObj);
        return newObj;
    }
}
