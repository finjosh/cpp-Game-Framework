#include "Object.hpp"
#include "ObjectManager.hpp"

// bool _objectComp::operator()(const Object* lhs, const Object* rhs) const
// {
//     return lhs->getID() < rhs->getID();
// }

std::atomic_ullong Object::m_lastID = 1;

Object::Object()
{
    m_id = m_lastID++; //! could become an issue if lots of creation and deletion happens
    ObjectManager::addObject(this);
}

Object::Object(unsigned long long id) : m_id(id) {}

void Object::m_setID(unsigned long long id)
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
    m_invokeDestroyEvents();
    ObjectManager::addToDestroyQueue(this);
    // ObjectManager::removeObject(this);
}

void Object::m_invokeDestroyEvents()
{
    for (auto child: m_children)
    {
        child->m_invokeDestroyEvents();
    }
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
        OnEnable();
    }
    else
    {
        m_onDisabled.invoke();
        onDisabled.invoke();
        OnDisable();
    }
}

bool Object::isEnabled() const
{
    if (m_parent)
        return m_enabled && m_parent->isEnabled();
    else
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

Vector2 Object::getLocalPoint(const Vector2& point) const
{
    return m_transform.getLocalPoint(point);
}

Vector2 Object::getGlobalPoint(const Vector2& point) const
{
    return m_transform.getGlobalPoint(point);
}

Vector2 Object::getGlobalVector(const Vector2& vec) const
{
    return m_transform.getGlobalVector(vec);
}

Vector2 Object::getLocalVector(const Vector2& vec) const
{
    return m_transform.getLocalVector(vec);
}

void Object::rotateAround(const Vector2& center, Rotation rot)
{
    Vector2 posChange(Vector2::rotateAround({m_transform.position}, {center}, rot) - m_transform.position);
    m_transform.position += posChange;
    m_onTransformUpdated.invoke();
    onTransformUpdated.invoke();

    // for (auto child: m_children)
    // {
    //     child->move(posChange);
    //     child->rotateAround({0,0}, rot);
    // }
}

void Object::setPosition(const Vector2& position)
{   
    Vector2 posChange(position - m_transform.position);
    m_transform.position = position;
    m_onTransformUpdated.invoke();
    onTransformUpdated.invoke();

    // for (auto child: m_children)
    // {
    //     child->move(posChange);
    // }
}

void Object::setPosition(float x, float y)
{
    setPosition({x,y});
}

Vector2 Object::getPosition() const
{   
    return m_transform.position;
}

void Object::setRotation(Rotation rotation)
{
    Rotation rotChange = rotation - m_transform.rotation;
    m_transform.rotation = rotation;
    m_onTransformUpdated.invoke();
    onTransformUpdated.invoke();

    // for (auto child: m_children)
    // {
    //     child->rotateAround(m_transform.position, rotChange);
    //     child->rotate(rotChange);
    // }
}

Rotation Object::getRotation() const
{
    return m_transform.rotation;
}

void Object::setTransform(const Transform& transform)
{
    // disable events so they are only called once
    m_onTransformUpdated.setEnabled(false);
    onTransformUpdated.setEnabled(false);
    Object::setPosition(transform.position);
    // enable events so they are call on set rotation
    m_onTransformUpdated.setEnabled(true);
    onTransformUpdated.setEnabled(true);
    Object::setRotation(transform.rotation);
}

const Transform& Object::getTransform() const
{
    return m_transform;
}

void Object::move(const Vector2& move)
{
    m_transform.position += move;
    m_onTransformUpdated.invoke();
    onTransformUpdated.invoke();

    for (auto child: m_children)
    {
        child->move(move);
    }
}

void Object::move(float x, float y)
{
    move(Vector2(x,y));
}

void Object::rotate(Rotation rot)
{
    m_transform.rotation = m_transform.rotation + rot;
    m_onTransformUpdated.invoke();
    onTransformUpdated.invoke();

    // for (auto child: m_children)
    // {
    //     child->rotate(rot);
    // }
}

void Object::setGlobalPosition(const Vector2& position)
{
    if (m_parent)
    {
        Object::setPosition(position - m_parent->getGlobalPosition());
    }
    else
    {
        Object::setPosition(position);
    }
}

Vector2 Object::getGlobalPosition() const
{
    if (m_parent)
    {
        return (Vector2::rotateAround(Object::getPosition(), {0,0}, m_parent->getGlobalRotation()) + m_parent->getGlobalPosition());
    }
    return Object::getPosition();
}

void Object::setGlobalRotation(Rotation rotation)
{
    if (m_parent)
    {
        Object::setRotation(rotation - m_parent->getGlobalRotation());
    }
    else
    {
        Object::setRotation(rotation);
    }
}

Rotation Object::getGlobalRotation() const
{
    if (m_parent)
    {
        return Object::getRotation() + m_parent->getGlobalRotation();
    }
    return Object::getRotation();
}

void Object::setGlobalTransform(const Transform& transform)
{
    if (m_parent)
    {
        // disable events so they are only called once
        m_onTransformUpdated.setEnabled(false);
        onTransformUpdated.setEnabled(false);
        Object::setGlobalPosition(transform.position);
        // enable events so they are call on set rotation
        m_onTransformUpdated.setEnabled(true);
        onTransformUpdated.setEnabled(true);
        Object::setGlobalRotation(transform.rotation);
    }
    else
    {
        Object::setTransform(transform);
    }
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
