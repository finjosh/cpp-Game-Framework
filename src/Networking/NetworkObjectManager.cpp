#include "Networking/NetworkObjectManager.hpp"
#include "Networking/NetworkTypes.hpp"

udp::Client* NetworkObjectManager::m_client = nullptr;
udp::Server* NetworkObjectManager::m_server = nullptr;
funcHelper::func<sf::Packet> NetworkObjectManager::m_sendPacketFunc;

std::mutex NetworkObjectManager::m_threadLock;

EventHelper::Event NetworkObjectManager::onConnectionOpen;
EventHelper::Event NetworkObjectManager::onConnectionClose;
EventHelper::EventDynamic<sf::Packet> NetworkObjectManager::onDataReceived;

std::unordered_set<NetworkObject*> NetworkObjectManager::m_objects;

NetworkObjectManager::_networkObject NetworkObjectManager::m_networkComp(0);

NetworkObjectManager::_networkObject::_networkObject(sf::Uint32 id) : NetworkObject(0) // the value does not matter as we never use the create function
{
    NetworkObject::m_id = id;
}

void NetworkObjectManager::_networkObject::setID(sf::Uint32 id)
{
    NetworkObject::m_id = id;
}

void NetworkObjectManager::init(udp::Server* server, udp::Client* client, funcHelper::func<sf::Packet> sendFunction)
{
    m_server = server;
    m_client = client;
    m_sendPacketFunc.setFunction(sendFunction);

    m_client->setThreadSafeOverride(true);

    m_server->onConnectionOpen([](){ onConnectionOpen.invoke(); });
    m_server->onConnectionClose([](){ onConnectionClose.invoke(); });
    m_server->onDataReceived(NetworkObjectManager::m_handleData);
    m_server->setPacketSendFunction(m_sendData);
    m_client->onConnectionOpen([](){ onConnectionOpen.invoke(); });
    m_client->onConnectionClose([](){ onConnectionClose.invoke(); });
    m_client->onDataReceived(NetworkObjectManager::m_handleData);
    m_client->setPacketSendFunction(m_sendData);
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
    return m_server->isConnectionOpen();
}

NetworkObject* NetworkObjectManager::getObject(sf::Uint64 networkID)
{
    m_networkComp.setID(networkID);
    auto obj = m_objects.find(&m_networkComp);
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
    packet << (sf::Uint64)m_objects.size();
    for (NetworkObject* obj: m_objects)
    {
        if (!obj->isEnabled())
            return;

        // sf::Packet temp; // object id, object type, object data
        packet << obj->getNetworkID() << obj->getTypeid();
        if (isServer())
            packet << obj->OnServerSendData();
        else
            packet << obj->OnClientSendData();
        // packet << temp;
    }

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
        onDataReceived.invoke(temp);
        temp.clear();
    }

    //* Unpacking object data
    sf::Uint64 objects; // the number of packets we will need to unpack
    packet >> objects;
    for (sf::Uint64 i = 0; i < objects; i++)
    {
        sf::Uint64 netID;
        sf::Uint64 typeID;
        packet >> netID >> typeID >> temp;
        
        NetworkObject* obj = NetworkObjectManager::getObject(netID);
        
        if (obj == nullptr)
        {
            auto newObj = NetworkType::createObject(typeID, netID);
            if (newObj != nullptr)
            {
                if (isServer())
                    newObj->OnServerReceivedData(temp);
                else
                    newObj->OnClientReceivedData(temp);
            }
        }
        else
        {
            if (isServer())
                obj->OnServerReceivedData(temp);
            else
                obj->OnClientReceivedData(temp);
        }
        temp.clear();
    }
}

void NetworkObjectManager::removeNetworkObject(NetworkObject* obj)
{
    m_objects.erase(obj);
}

void NetworkObjectManager::createNetworkObject(NetworkObject* obj)
{
    m_objects.insert(obj);
}

void NetworkObjectManager::lock()
{
    m_threadLock.lock();
}

void NetworkObjectManager::unlock()
{
    m_threadLock.unlock();
}
