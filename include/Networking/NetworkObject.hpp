#ifndef NETWORK_OBJECT_HPP
#define NETWORK_OBJECT_HPP

#pragma once

#include "SFML/Network/Packet.hpp"

#include "Object.hpp"

class NetworkObjectManager;

/// @warning when and object is used on a network make sure to only use fixed size types
/// @note fixed sized types are built in with SFML
class NetworkObject : public virtual Object
{
public:
    // /// @param objectType the type of object that is being made
    // /// @note the objectType should be declared in the ObjectType.hpp file first
    // NetworkObject(const size_t& objectType);
    NetworkObject();
    ~NetworkObject();

    /// @brief the network ID is the id of either a client or the server to the network connection
    /// @returns the id that defined this program on the network
    sf::Uint32 getNetworkID() const;
    // size_t getTypeid() const;
    // void createNetworkObject();
    /// @brief removes this object from the network
    /// @note does not destroy this object
    // void removeNetworkObject();

    /// @brief called when the network connection is opened
    /// @note this is called when either the server is opened or the client is connected
    inline virtual void OnConnectionOpen(){};
    /// @brief called when the network connection is closed
    /// @note this is called when either the server is closed or the client is disconnected 
    inline virtual void OnConnectionClose(){};
    /// @brief called when data is received for this Object
    /// @param data the packet containing data for this obj
    virtual void OnDataReceived(sf::Packet& data) = 0;
    /// @brief Called when data is required from this object
    /// @note make sure that the send function is thread safe
    /// @returns a packet with the data that will be unpacked in "OnDataReceived"
    virtual sf::Packet OnSendData() = 0;

protected:
    friend NetworkObjectManager;

private:
    /// @brief null id = 0
    sf::Uint32 m_id = 0;
    // const size_t _typeid = 0;

    static sf::Uint32 m_nextID;
};

class _networkObjectComp
{
public:
    bool operator() (const NetworkObject* lhs, const NetworkObject* rhs) const;
};

namespace std {
    template <>
    struct hash<NetworkObject> {
        inline size_t operator()(const NetworkObject& obj) const noexcept
        {
            return hash<size_t>{}(obj.getNetworkID());
        }
    };
    template <>
    struct hash<NetworkObject*> {
        inline size_t operator()(const NetworkObject* obj) const noexcept
        {
            if (obj == nullptr)
                return 0;
            return hash<size_t>{}(obj->getNetworkID());
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
