#pragma once

#include <vector>

class Die
{
public:
    void randomize();

    int operator=(int value);
    Die& operator=(const Die& other);
    operator int() const;
    int operator--();
    int operator++();
    int operator--(int);
    int operator++(int);

private:
    int m_value = 6;
};

class Dice : public std::vector<Die>
{
public:
    void randomize();
    int getTotal();
};
