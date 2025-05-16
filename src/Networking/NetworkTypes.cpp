#include "Networking/NetworkTypes.hpp"
#include "Networking/NetworkObject.hpp"
#include "Networking/NetworkObjectManager.hpp"

std::unordered_map<std::uint64_t, funcHelper::func<NetworkObject*>> NetworkType::m_factories;

bool NetworkType::initType(std::uint64_t type, const funcHelper::func<NetworkObject*>& factoryFunc)
{
    return m_factories.emplace(type, factoryFunc).second;
}

NetworkObject* NetworkType::createObject(std::uint64_t value, std::uint32_t netID)
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
