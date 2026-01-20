#pragma once

#include <cstdint>

// Base class for map cells shared between client and server
// Client extends this with rendering data, server uses simpler MapCell
class MapCellT
{
public:
    // Cell flags for collision/pathfinding (matches server CellFlags namespace)
    enum Flags : uint8_t
    {
        None         = 0x00,
        Unwalkable   = 0x01,  // Cannot walk through
        CollideBlock = 0x02,  // Blocks line of sight
    };

    MapCellT() : m_flags(None) {}
    virtual ~MapCellT() = default;

    // Flag operations
    bool hasFlag(Flags flag) const { return (m_flags & flag) != 0; }
    void addFlag(Flags flag) { m_flags |= flag; }
    void removeFlag(Flags flag) { m_flags &= ~flag; }

    uint8_t getFlags() const { return m_flags; }
    void setFlags(uint8_t flags) { m_flags = flags; }

protected:
    uint8_t m_flags;
};
