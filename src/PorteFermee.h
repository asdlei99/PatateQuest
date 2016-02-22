#pragma once
#include "Porte.h"

class PorteFermee final : public Porte, public std::enable_shared_from_this<PorteFermee>
{
public:
    PorteFermee(const Point& position = {0, 0}, Direction direction = Direction::Front);

    void render() override;
};
