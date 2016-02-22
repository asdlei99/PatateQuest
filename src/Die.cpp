#include "Die.h"

#include "onut_old.h"

void Die::randomize()
{
    m_value = ORandInt(1, 6);
}

int Die::operator=(int value)
{
    m_value = value;
    return m_value;
}

Die::operator int() const
{
    return m_value;
}

int Die::operator--()
{
    return --m_value;
}

int Die::operator++()
{
    return ++m_value;
}

int Die::operator--(int)
{
    return m_value--;
}

int Die::operator++(int)
{
    return m_value--;
}

Die& Die::operator=(const Die& other)
{
    m_value = other.m_value;
    return *this;
}

void Dice::randomize()
{
    for (auto& die : *this)
    {
        //die = 6;
        die.randomize();
    }
}

int Dice::getTotal()
{
    int ret = 0;
    for (auto& die : *this)
    {
        ret += die;
    }
    return ret;
}
