#include "Object.hpp"
#include "ObjectManager.hpp"

bool _objectComp::operator()(const Object* lhs, const Object* rhs) const
{
    return lhs->getID() < rhs->getID();
}

std::list<Object::Ptr<>> Object::_destroyQueue;
std::atomic_ullong Object::_lastID = 1;

Object::Object()
{
    _id = _lastID++; //! could become an issue if lots of creation and deletion happens
    ObjectManager::addObject(this);
}

Object::Object(unsigned long long id) : _id(id) {}

void Object::setID(unsigned long long id)
{
    _id = id;
}

Object::~Object()
{
    // this is not a real object
    if (_id == 0)
        return;

    ObjectManager::removeObject(this);
    
    if (_parent != nullptr)
    {
        _parent->_removeChild(this);
    }
    
    auto child = _children.begin();
    while (child != _children.end())
    {
        auto temp = child++;
        (*temp)->_destroy();
    }

    _onDestroy.invoke();
    onDestroy.invoke();
}

void Object::destroy()
{
    _addToDestroyQueue();
}

void Object::setEnabled(bool enabled)
{
    _enabled = enabled;
    if (_enabled)
    {
        _onEnabled.invoke();
        onEnabled.invoke();
    }
    else
    {
        _onDisabled.invoke();
        onDisabled.invoke();
    }
}

bool Object::isEnabled() const
{
    return _enabled;
}

unsigned long int Object::getID() const
{
    return _id;
}

Object::Ptr<> Object::getPtr()
{
    return Object::Ptr<>(this);
}

void Object::setParent(Object* parent)
{
    if (parent == this)
        return;

    if (_parent != nullptr)
    {
        _parent->_removeChild(this);
    }

    _parent = parent;
    // if not valid have no parent
    if (parent != nullptr)
    {
        parent->_addChild(this);
        _onParentSet.invoke();
        onParentSet.invoke();
    }
    else
    {
        _onParentRemoved.invoke();
        onParentRemoved.invoke();
    }
}

Object::Ptr<> Object::getParent()
{
    return _parent;
}

b2Vec2 Object::getLocalVector(const b2Vec2& vec) const
{
    return b2MulT(_transform.q, vec);
}

b2Vec2 Object::getGlobalVector(const b2Vec2& vec) const
{
    return b2Mul(_transform.q, vec);
}

void Object::rotateAround(const b2Vec2& center, const float& rot)
{
    b2Vec2 posChange = rotateAround(_transform.p, center, rot) - _transform.p;
    _transform.p += posChange;

    for (auto child: _children)
    {
        child->move(posChange);
        child->rotateAround(_transform.p, rot);
    }
}

b2Vec2 Object::rotateAround(const b2Vec2& vec, const b2Vec2& center, const float& rot)
{
    auto polar = std::polar<float>(1.0, rot);
    std::complex<float> temp(vec.x - center.x, vec.y - center.y);
    temp *= polar;
    return {temp.real() + center.x, temp.imag() + center.y};
}

void Object::setPosition(const b2Vec2& position)
{   
    b2Vec2 posChange = position - _transform.p;

    for (auto child: _children)
    {
        child->move(posChange);
    }

    _transform.p = position;
}

b2Vec2 Object::getPosition() const
{   
    return _transform.p;
}

void Object::setRotation(const float& rotation)
{
    float rotChange = rotation - _transform.q.GetAngle();

    for (auto child: _children)
    {
        child->rotateAround(_transform.p, rotChange);
        child->rotate(rotChange);
    }

    _transform.q.Set(rotation);
}

float Object::getRotation() const
{
    return _transform.q.GetAngle();
}

void Object::setTransform(const b2Transform& transform)
{
    Object::setPosition(transform.p);
    Object::setRotation(transform.q.GetAngle());
}

b2Transform Object::getTransform() const
{
    return _transform;
}

bool Object::canSetTransform() const
{
    return true;
}

void Object::move(const b2Vec2& move)
{
    _transform.p += move;

    for (auto child: _children)
    {
        child->move(move);
    }
}

void Object::rotate(const float& rot)
{
    _transform.q.Set(_transform.q.GetAngle() + rot);

    for (auto child: _children)
    {
        child->rotate(rot);
    }
}

void Object::setLocalPosition(const b2Vec2& position)
{
    if (_parent)
    {
        this->setPosition(_parent->getPosition() + position);
    }
    else
    {
        this->setPosition(position);
    }
}

b2Vec2 Object::getLocalPosition() const
{
    if (_parent)
    {
        return this->getPosition() - _parent->getPosition();
    }
    return this->getPosition();
}

void Object::setLocalRotation(const float& rotation)
{
    if (_parent)
    {
        this->setRotation(_parent->getRotation() + rotation);
    }
    else
    {
        this->setRotation(rotation);
    }
}

float Object::getLocalRotation() const
{
    if (_parent)
    {
        return this->getRotation() - _parent->getRotation();
    }
    return this->getRotation();
}

b2Rot Object::getRotation_b2() const
{
    return _transform.q;
}

void Object::_addChild(Object* object)
{
    if (object == nullptr) return;
    _children.push_back(object);
}

void Object::_removeChild(Object* object)
{
    if (object == nullptr) return;
    _children.remove_if([object](const Object* obj){ return obj == object; });
}

void Object::_addToDestroyQueue()
{
    _destroyQueue.emplace_back(this);
}
