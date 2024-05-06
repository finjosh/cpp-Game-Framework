#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#pragma once

#include "UpdateInterface.hpp"

#include <unordered_set>

class UpdateInterface;

class UpdateManager
{
public:
    /// @brief called once every frame
    static void Update(float deltaTime, int numThreads);
    /// @brief called after update
    static void LateUpdate(float deltaTime);
    /// @brief called up to 50 times a second
    static void FixedUpdate();
    /// @brief called just before opening the window
    static void Start();
    static size_t getNumberOfObjects();

    static void _update(float deltaTime); // TODO make this protected after testing
protected:
    static void addUpdateObject(UpdateInterface* obj);
    static void removeUpdateObject(UpdateInterface* obj);


    friend UpdateInterface;

private:
    inline UpdateManager() = default;

    static std::unordered_set<UpdateInterface*> m_objects;

    // iterator used for thread updating
    static std::unordered_set<UpdateInterface*>::iterator m_iterator;
    static std::mutex m_iteratorLock;
};

#endif
