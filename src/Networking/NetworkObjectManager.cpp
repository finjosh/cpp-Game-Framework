#include "Networking/NetworkObjectManager.hpp"

udp::Client* NetworkObjectManager::m_client = nullptr;
udp::Server* NetworkObjectManager::m_server = nullptr;
bool NetworkObjectManager::m_isServer = false;
funcHelper::func<sf::Packet> NetworkObjectManager::m_sendPacketFunc;

std::mutex NetworkObjectManager::m_threadLock;

EventHelper::Event NetworkObjectManager::onConnectionOpen;
EventHelper::Event NetworkObjectManager::onConnectionClose;
// EventHelper::EventDynamic<sf::Packet*> NetworkObjectManager::onDataReceived;

std::unordered_set<NetworkObject*> NetworkObjectManager::m_objects;

NetworkObjectManager::_networkObject::_networkObject(sf::Uint32 id) 
{
    NetworkObject::m_id = id;
}

void NetworkObjectManager::init(udp::Server* server, udp::Client* client, funcHelper::func<sf::Packet> sendFunction)
{
    m_server = server;
    m_client = client;
    m_sendPacketFunc.setFunction(sendFunction);

    m_client->setThreadSafeOverride(true);

    // m_server->onConnectionOpen([](){ onConnectionOpen.invoke(); });
    // m_server->onConnectionOpen([](){ m_isServer = true; });
    // m_server->onConnectionClose([](){ onConnectionClose.invoke(); });
    // m_server->onConnectionClose([](){ m_isServer = false; });
    // m_server->onDataReceived(NetworkObjectManager::m_handleData);
    // m_server->setPacketSendFunction(m_sendData);
    // m_client->onConnectionOpen([](){ onConnectionOpen.invoke(); });
    // m_client->onConnectionOpen([](){ m_isServer = false; });
    // m_client->onConnectionClose([](){ onConnectionClose.invoke(); });
    // m_client->onDataReceived(NetworkObjectManager::m_handleData);
    // m_client->setPacketSendFunction(m_sendData);
}

udp::Server* NetworkObjectManager::getServer()
{
    return m_server;
}

udp::Client* NetworkObjectManager::getClient()
{
    return m_client;
}

bool NetworkObjectManager::isServer()
{
    return m_isServer;
}

NetworkObject* NetworkObjectManager::getObject(sf::Uint32 networkID)
{
    _networkObject temp(networkID);
    auto obj = m_objects.find(&temp);
    if (obj == m_objects.end())
        return nullptr;
    return *obj;
}

void NetworkObjectManager::m_sendData()
{
    m_threadLock.lock();
    sf::Packet packet = udp::Socket::DataPacketTemplate();

    //* Adding the extra data packet
    {
        packet << m_sendPacketFunc.invoke();
    }

    //* Adding object data
    // packet << m_objects.size();
    // for (NetworkObject* obj: m_objects)
    // {
    //     sf::Packet temp = obj->OnSendData();
    //     packet << temp;
    //     // packet << obj->getNetworkID() << obj->getTypeid() << temp.getDataSize();
    //     // packet.append(temp.getData(), temp.getDataSize());
    // }

    if (isServer())
        m_server->sendToAll(packet);
    else
        m_client->sendToServer(packet);
    m_threadLock.unlock();
}

void NetworkObjectManager::m_handleData(sf::Packet packet)
{
    sf::Packet temp;

    //* Unpacking the extra data
    {
        temp.clear();
        packet >> temp;
        // onDataReceived.invoke(&temp);
    }

    //* Unpacking object data
    // size_t objects;
    // packet >> objects;
    // for (int i = 0; i < objects; i++)
    // {
    //     unsigned long long id;
    //     size_t typeID;
    //     packet >> temp;
    //     // size_t dataSize;
    //     // packet >> id >> typeID >> dataSize;
    //     // temp.append(packet.getData(), dataSize); // TODO does not remove the data from the packet
        
    //     NetworkObject* obj = NetworkObjectManager::getObject(id);
        
    //     if (obj == nullptr)
    //     {
    //         // TODO create the object
    //         continue;
    //     }
    //     obj->OnDataReceived(temp);
    //     temp.clear();
    // }

    // TODO implement the object data handling
}

// void NetworkObjectManager::removeNetworkObject(NetworkObject* obj)
// {
//     m_objects.erase(obj);
// }

// void NetworkObjectManager::createNetworkObject(NetworkObject* obj)
// {
//     m_objects.insert(obj);
// }

void NetworkObjectManager::lock()
{
    m_threadLock.lock();
}

void NetworkObjectManager::unlock()
{
    m_threadLock.unlock();
}
