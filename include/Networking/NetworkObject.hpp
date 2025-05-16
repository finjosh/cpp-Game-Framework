#ifndef NETWORK_OBJECT_HPP
#define NETWORK_OBJECT_HPP

#pragma once

#include "SFML/Network/Packet.hpp"

#include "Object.hpp"

class NetworkObjectManager;
class NetworkType;

/// @warning when and object is used on a network make sure to only use fixed size types (also built in with sfml)
/// @note the class of which this is a base class must have a constructor with zero inputs
class NetworkObject : public virtual Object
{
public:
    /// @param type the type of object that is being made
    /// @note the objectType should be declared in the ObjectType.hpp file first
    NetworkObject(uint64_t type); // TODO do this using typeid and a dict that converts typeid to the object type
    // NetworkObject();
    virtual ~NetworkObject();

    /// @brief the network ID is the id of either a client or the server to the network connection
    /// @returns the id that defined this program on the network
    std::uint64_t getNetworkID() const;
    uint64_t getTypeid() const;
    void createNetworkObject(); // TODO handle if this is a server or client properly
    /// @brief removes this object from the network
    /// @note does not destroy this object
    void removeNetworkObject();

    /// @brief called when the network connection is opened
    /// @note this is called when either the server is opened or the client is connected
    inline virtual void OnConnectionOpen(){};
    /// @brief called when the network connection is closed
    /// @note this is called when either the server is closed or the client is disconnected 
    inline virtual void OnConnectionClose(){};

    virtual void OnClientReceivedData(sf::Packet& data) = 0;
    virtual sf::Packet OnClientSendData() = 0;
    virtual void OnServerReceivedData(sf::Packet& data) = 0;
    virtual sf::Packet OnServerSendData() = 0;

protected:
    friend NetworkObjectManager;
    friend NetworkType;

private:
    /// @brief null id = 0
    std::uint64_t m_id = 0;
    const std::uint64_t m_typeid = 0;

    static std::uint64_t m_nextID;
};

namespace std {
    template <>
    struct hash<NetworkObject> {
        inline uint64_t operator()(const NetworkObject& obj) const noexcept
        {
            return hash<uint64_t>{}(obj.getNetworkID());
        }
    };
    template <>
    struct hash<NetworkObject*> {
        inline uint64_t operator()(const NetworkObject* obj) const noexcept
        {
            if (obj == nullptr)
                return 0;
            return hash<uint64_t>{}(obj->getNetworkID());
        }
    };
    template <>
    struct equal_to<NetworkObject> {
        inline bool operator()(const NetworkObject& obj, const NetworkObject& obj2) const noexcept
        {
            return obj.getNetworkID() == obj2.getNetworkID();
        }
    };
    template <>
    struct equal_to<NetworkObject*> {
        inline bool operator()(const NetworkObject* obj, const NetworkObject* obj2) const noexcept
        {
            return obj->getNetworkID() == obj2->getNetworkID();
        }
    };
}

#endif
