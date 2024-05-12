#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#pragma once

#include <unordered_set>

#include "BS_thread_pool.hpp"

#include "UpdateInterface.hpp"

class UpdateInterface;

class UpdateManager
{
public:
    /// @brief called once every frame
    static void Update(float deltaTime, BS::thread_pool& pool);
    /// @brief called after update
    static void LateUpdate(float deltaTime, BS::thread_pool& pool);
    /// @brief called up to 50 times a second
    static void FixedUpdate(BS::thread_pool& pool);
    /// @brief called just before opening the window
    static void Start(BS::thread_pool& pool);
    static size_t getNumberOfObjects();

protected:
    static void addUpdateObject(UpdateInterface* obj);
    static void removeUpdateObject(UpdateInterface* obj);

    static void _update(float deltaTime, std::unordered_set<UpdateInterface*>::iterator begin, std::unordered_set<UpdateInterface*>::iterator end);
    static void _lateUpdate(float deltaTime, std::unordered_set<UpdateInterface*>::iterator begin, std::unordered_set<UpdateInterface*>::iterator end);
    static void _fixedUpdate(std::unordered_set<UpdateInterface*>::iterator begin, std::unordered_set<UpdateInterface*>::iterator end);
    static void _start(std::unordered_set<UpdateInterface*>::iterator begin, std::unordered_set<UpdateInterface*>::iterator end);

    friend UpdateInterface;

private:
    inline UpdateManager() = default;

    static std::unordered_set<UpdateInterface*> m_objects;
};

#endif
