#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#pragma once

#include "UpdateInterface.hpp"
#include "Graphics/DrawableObject.hpp"

class GameObject : virtual public UpdateInterface, virtual public DrawableObject {};

#endif
