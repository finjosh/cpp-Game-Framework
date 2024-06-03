#include "UpdateInterface.hpp"
#include "UpdateManager.hpp"

UpdateInterface::UpdateInterface()
{
    // _onParentRemoved(&UpdateManager::addUpdateObject, this);
    // _onParentSet(&UpdateManager::removeUpdateObject, this);

    UpdateManager::addUpdateObject(this);
}

UpdateInterface::~UpdateInterface()
{
    UpdateManager::removeUpdateObject(this);
}

void UpdateInterface::Update(float deltaTime) {}

void UpdateInterface::LateUpdate(float deltaTime) {}

void UpdateInterface::FixedUpdate() {}

void UpdateInterface::Start() {}
