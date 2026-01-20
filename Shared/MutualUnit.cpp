// MutualUnit - Shared unit logic for client and server
// TODO: Implement fully in Task 1.10

#include "MutualUnit.h"
#include "ObjDefines.h"

bool MutualUnit::isAlive() const
{
    // Check if HP > 0 using the variable system
    int currentHp = getVariable(static_cast<int>(ObjDefines::Variable::Health));
    return currentHp > 0;
}
