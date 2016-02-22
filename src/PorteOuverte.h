#pragma once
#include "Porte.h"

class PorteOuverte final : public Porte
{
public:
    PorteOuverte(const Point& position = {0, 0}, Direction direction = Direction::Front);

    void render() override;
};
