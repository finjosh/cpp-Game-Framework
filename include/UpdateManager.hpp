#ifndef UPDATE_MANAGER_HPP
#define UPDATE_MANAGER_HPP

#pragma once

#include <unordered_set>

// #include "BS_thread_pool.hpp"

#include "UpdateInterface.hpp"

class UpdateInterface;

class UpdateManager
{
public:
    // /// @brief called once every frame
    // static void Update(float deltaTime, BS::thread_pool& pool);
    // /// @brief called after update
    // static void LateUpdate(float deltaTime, BS::thread_pool& pool);
    // /// @brief called up to 50 times a second
    // static void FixedUpdate(BS::thread_pool& pool);
    // /// @brief called just before opening the window
    // static void Start(BS::thread_pool& pool);

    /// @note use this for single threaded updates
    static inline void Update(float deltaTime)
    {
        _update(deltaTime, m_objects.begin(), m_objects.end());
    }
    /// @note use this for single threaded updates
    static inline void LateUpdate(float deltaTime)
    {
        _lateUpdate(deltaTime, m_objects.begin(), m_objects.end());
    }
    /// @note use this for single threaded updates
    static inline void FixedUpdate()
    {
        _fixedUpdate(m_objects.begin(), m_objects.end());
    }
    /// @note use this for single threaded updates
    static inline void Start()
    {
        _start(m_objects.begin(), m_objects.end());
    }

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
