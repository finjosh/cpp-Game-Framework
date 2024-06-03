#include "Object.hpp"
#include "ObjectManager.hpp"

bool _objectComp::operator()(const Object* lhs, const Object* rhs) const
{
    return lhs->getID() < rhs->getID();
}

std::atomic_ullong Object::m_lastID = 1;

Object::Object()
{
    m_id = m_lastID++; //! could become an issue if lots of creation and deletion happens
    ObjectManager::addObject(this);
}

Object::Object(unsigned long long id) : m_id(id) {}

void Object::setID(unsigned long long id)
{
    m_id = id;
}

Object::~Object()
{
    // this is not a real object
    if (m_id == 0)
        return;

    ObjectManager::removeObject(this);
    
    if (m_parent != nullptr)
    {
        m_parent->m_removeChild(this);
    }
    
    auto child = m_children.begin();
    while (child != m_children.end())
    {
        auto temp = child++;
        (*temp)->m_destroy();
    }
}

void Object::destroy()
{
    ObjectManager::addToDestroyQueue(this);
    m_enabled = false; // dont want the event to be called
    m_onDestroy.invoke();
    onDestroy.invoke();
}

void Object::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (m_enabled)
    {
        m_onEnabled.invoke();
        onEnabled.invoke();
    }
    else
    {
        m_onDisabled.invoke();
        onDisabled.invoke();
    }
}

bool Object::isEnabled() const
{
    return m_enabled;
}

unsigned long int Object::getID() const
{
    return m_id;
}

Object::Ptr<> Object::getPtr()
{
    return Object::Ptr<>(this);
}

void Object::setParent(Object::Ptr<>& parent)
{
    setParent(parent.get());
}

void Object::setParent(Object* parent)
{
    if (parent == this)
        return;

    if (m_parent != nullptr)
    {
        m_parent->m_removeChild(this);
    }

    m_parent = parent;
    // if not valid have no parent
    if (parent != nullptr)
    {
        parent->m_addChild(this);
        m_onParentSet.invoke();
        onParentSet.invoke();
    }
    else
    {
        m_onParentRemoved.invoke();
        onParentRemoved.invoke();
    }
}

Object::Ptr<> Object::getParent()
{
    return m_parent;
}

void Object::addChild(Object::Ptr<>& child)
{
    addChild(child.get());
}

void Object::addChild(Object* child)
{
    if (child == this || child == nullptr)
            return;
    
    child->setParent(this);
}

b2Vec2 Object::getLocalVector(const b2Vec2& vec) const
{
    return b2MulT(m_transform.q, vec);
}

b2Vec2 Object::getGlobalVector(const b2Vec2& vec) const
{
    return b2Mul(m_transform.q, vec);
}

void Object::rotateAround(const b2Vec2& center, float rot)
{
    b2Vec2 posChange = rotateAround(m_transform.p, center, rot) - m_transform.p;
    m_transform.p += posChange;
    m_onTransformUpdated.invoke();

    for (auto child: m_children)
    {
        child->move(posChange);
        child->rotateAround(m_transform.p, rot);
    }
}

b2Vec2 Object::rotateAround(const b2Vec2& vec, const b2Vec2& center, float rot)
{
    auto polar = std::polar<float>(1.0, rot);
    std::complex<float> temp(vec.x - center.x, vec.y - center.y);
    temp *= polar;
    return {temp.real() + center.x, temp.imag() + center.y};
}

void Object::setPosition(const b2Vec2& position)
{   
    b2Vec2 posChange = position - m_transform.p;
    m_transform.p = position;
    m_onTransformUpdated.invoke();

    for (auto child: m_children)
    {
        child->move(posChange);
    }
}

void Object::setPosition(float x, float y)
{
    setPosition({x,y});
}

b2Vec2 Object::getPosition() const
{   
    return m_transform.p;
}

void Object::setRotation(float rotation)
{
    float rotChange = rotation - m_transform.q.GetAngle();
    m_transform.q.Set(rotation);
    m_onTransformUpdated.invoke();

    for (auto child: m_children)
    {
        child->rotateAround(m_transform.p, rotChange);
        child->rotate(rotChange);
    }
}

float Object::getRotation() const
{
    return m_transform.q.GetAngle();
}

void Object::setTransform(const b2Transform& transform)
{
    Object::setPosition(transform.p);
    Object::setRotation(transform.q.GetAngle());
}

b2Transform Object::getTransform() const
{
    return m_transform;
}

void Object::move(const b2Vec2& move)
{
    m_transform.p += move;
    m_onTransformUpdated.invoke();

    for (auto child: m_children)
    {
        child->move(move);
    }
}

void Object::rotate(float rot)
{
    m_transform.q.Set(m_transform.q.GetAngle() + rot);
    m_onTransformUpdated.invoke();

    for (auto child: m_children)
    {
        child->rotate(rot);
    }
}

void Object::setLocalPosition(const b2Vec2& position)
{
    if (m_parent)
    {
        this->setPosition(m_parent->getPosition() + position);
    }
    else
    {
        this->setPosition(position);
    }
}

b2Vec2 Object::getLocalPosition() const
{
    if (m_parent)
    {
        return this->getPosition() - m_parent->getPosition();
    }
    return this->getPosition();
}

void Object::setLocalRotation(float rotation)
{
    if (m_parent)
    {
        this->setRotation(m_parent->getRotation() + rotation);
    }
    else
    {
        this->setRotation(rotation);
    }
}

float Object::getLocalRotation() const
{
    if (m_parent)
    {
        return this->getRotation() - m_parent->getRotation();
    }
    return this->getRotation();
}

b2Rot Object::getRotation_b2() const
{
    return m_transform.q;
}

void Object::m_addChild(Object* object)
{
    if (object == nullptr) return;
    m_children.push_back(object);
}

void Object::m_removeChild(Object* object)
{
    if (object == nullptr) return;
    m_children.remove_if([object](const Object* obj){ return obj == object; });
}
