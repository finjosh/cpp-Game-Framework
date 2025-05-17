#include "Physics/Filter.hpp"

Filter::Filter() : m_filter(b2DefaultFilter()) {}
Filter::Filter(uint64_t categoryBits, uint64_t maskBits, int groupIndex) : m_filter({categoryBits, maskBits, groupIndex}) {}

uint64_t Filter::getCategoryBits() const
{
    return m_filter.categoryBits;
}

void Filter::setCategoryBits(uint64_t categoryBits)
{
    m_filter.categoryBits = categoryBits;
}

uint64_t Filter::getMaskBits() const
{
    return m_filter.maskBits;
}

void Filter::setMaskBits(uint64_t maskBits)
{
    m_filter.maskBits = maskBits;
}

int Filter::getGroupIndex() const
{
    return m_filter.groupIndex;
}

void Filter::setGroupIndex(int groupIndex)
{
    m_filter.groupIndex = groupIndex;
}

Filter::Filter(b2Filter filter) : m_filter(filter) {}
