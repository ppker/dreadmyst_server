#pragma once

#include "MutualObject.h"
#include "../Geo2d.h"
#include <string>
#include <map>

// Base class for unit state shared between client and server
// Units are entities that can move, attack, cast spells (players, NPCs)
// Uses virtual inheritance to avoid diamond problem with ClientUnit
class MutualUnit : virtual public MutualObject
{
public:
    MutualUnit() = default;
    explicit MutualUnit(uint32_t guid) : MutualObject(guid) {}
    explicit MutualUnit(const Geo2d::Vector2& pos) : m_x(pos.x), m_y(pos.y) {}

    // Position
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    void setPosition(float x, float y) { m_x = x; m_y = y; }

    // Orientation (facing direction)
    float getOrientation() const { return m_orientation; }
    void setOrientation(float orientation) { m_orientation = orientation; }

    // Name
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    // Equipment (slot -> itemId)
    void setEquipment(int slot, int itemId) { m_equipment[slot] = itemId; }
    int getEquipment(int slot) const
    {
        auto it = m_equipment.find(slot);
        return it != m_equipment.end() ? it->second : 0;
    }

    // Common checks
    bool isAlive() const;

protected:
    float m_x = 0, m_y = 0;
    float m_orientation = 0;
    std::string m_name;
    std::map<int, int> m_equipment;
};
