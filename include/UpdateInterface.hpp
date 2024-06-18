#ifndef UPDATEINTERFACE_H
#define UPDATEINTERFACE_H

#pragma once

#include "Object.hpp"

class UpdateInterface : public virtual Object
{
public:
    using Ptr = Object::Ptr<UpdateInterface>;

    UpdateInterface();
    ~UpdateInterface();

    /// @brief called every frame
    virtual void Update(float deltaTime);
    /// @brief called every frame after update
    virtual void LateUpdate(float deltaTime);
    /// @brief called a fixed amount of times per second
    virtual void FixedUpdate();
    /// @brief called right before window opens
    /// @note called even if the object is disabled
    virtual void Start();

protected:

private:
    
};

#endif
