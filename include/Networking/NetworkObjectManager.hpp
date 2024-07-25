#ifndef NETWORK_OBJECT_MANAGER_HPP
#define NETWORK_OBJECT_MANAGER_HPP

#pragma once

#include <set>
#include <mutex>

#include "Networking/NetworkObject.hpp"
#include "Networking/SocketUI.hpp"
#include "Networking/Server.hpp"
#include "Networking/Client.hpp"

#include "Utils/EventHelper.hpp"
#include "Utils/funcHelper.hpp"

class NetworkType;

class NetworkObjectManager
{
public:
    /// @brief initializes the manager so you can make custom UI for handling server and client connection states
    /// @param sendFunction this should only be extra data not related to any specific object data
    /// @note make sure that the sendFunction is thread safe
    static void init(udp::Server* server, udp::Client* client, funcHelper::func<sf::Packet> sendFunction = {[](){ return sf::Packet(); }});
    static udp::Server* getServer();
    static udp::Client* getClient();

    /// @returns true if the current socket type is server
    static bool isServer();

    /// @warning do NOT store this ptr, get the object pointer if storage is required
    /// @returns nullptr if the network object does not exist 
    static NetworkObject* getObject(sf::Uint64 networkID);

    static EventHelper::Event onConnectionOpen;
    static EventHelper::Event onConnectionClose;

    /// @brief optional parameter of the packet data
    /// @note This will be called BEFORE any object data is handled
    /// @note this is intended for extra data (other than object data)
    static EventHelper::EventDynamic<sf::Packet> onDataReceived;

    // TODO probably not the best way to do this
    // TODO do this where the other threads have to wait for the network to get data for sending before being able to set data
    /// @brief stops the network manager from collecting data while it may be in use
    /// @warning do NOT forget to unlock after locking
    static void lock();
    /// @brief lets the network manager collect data again
    static void unlock();

protected:
    /// @brief called by the server/client when sending data
    static void m_sendData();
    /// @brief handles the data received 
    static void m_handleData(sf::Packet packet);

    /// @note not thread safe
    static void removeNetworkObject(NetworkObject* obj);
    /// @note not thread safe
    static void createNetworkObject(NetworkObject* obj);

    friend NetworkObject;
    friend NetworkType;

private:
    inline NetworkObjectManager() = default;

    static udp::Client* m_client;
    static udp::Server* m_server;
    static funcHelper::func<sf::Packet> m_sendPacketFunc;

    static std::unordered_set<NetworkObject*> m_objects;

    static std::mutex m_threadLock;

    /// @note used as network object can not be made on its own due to its pure virtual destroy function
    class _networkObject : public NetworkObject
    {
    public:
        _networkObject(sf::Uint32 id);
        void setID(sf::Uint32 id);
        inline void OnClientReceivedData(sf::Packet& data) override {}
        inline sf::Packet OnClientSendData() override { return sf::Packet(); }
        inline void OnServerReceivedData(sf::Packet& data) {}
        inline sf::Packet OnServerSendData() override { return sf::Packet(); }
    };
    static _networkObject m_networkComp;
};

#endif
