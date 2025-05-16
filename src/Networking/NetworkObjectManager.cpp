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

NetworkObjectManager::_networkObject NetworkObjectManager::m_networkComp;

NetworkObjectManager::_networkObject::_networkObject() : Object(0), NetworkObject(0) // the value does not matter as we never use the create function
{}

void NetworkObjectManager::_networkObject::setID(std::uint32_t id)
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

NetworkObject* NetworkObjectManager::getObject(std::uint64_t networkID)
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
    packet << (std::uint64_t)m_objects.size();
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
    std::uint64_t objects; // the number of packets we will need to unpack
    packet >> objects;
    for (std::uint64_t i = 0; i < objects; i++)
    {
        std::uint64_t netID;
        std::uint64_t typeID;
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
