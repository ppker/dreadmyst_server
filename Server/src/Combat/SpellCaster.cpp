// SpellCaster - Spell casting validation and execution
// Task 5.2: Cast Validation
// Task 5.3: Spell Targeting

#include "stdafx.h"
#include "Combat/SpellCaster.h"
#include "Combat/SpellUtils.h"
#include "Combat/CooldownManager.h"
#include "Combat/AuraSystem.h"
#include "Database/GameData.h"
#include "World/Entity.h"
#include "World/Player.h"
#include "World/Map.h"
#include "World/WorldManager.h"
#include "Core/Logger.h"
#include "ObjDefines.h"
#include <cmath>

// ============================================================================
// Cast Result String Conversion
// ============================================================================

const char* getCastResultString(CastResult result)
{
    switch (result)
    {
        case CastResult::Success:           return "Success";
        case CastResult::CasterDead:        return "You are dead";
        case CastResult::CasterStunned:     return "You are stunned";
        case CastResult::CasterSilenced:    return "You are silenced";
        case CastResult::CasterMoving:      return "Cannot cast while moving";
        case CastResult::CasterCasting:     return "Already casting";
        case CastResult::UnknownSpell:      return "Unknown spell";
        case CastResult::SpellNotLearned:   return "You haven't learned this spell";
        case CastResult::SpellDisabled:     return "Spell is disabled";
        case CastResult::NotEnoughMana:     return "Not enough mana";
        case CastResult::NotEnoughHealth:   return "Not enough health";
        case CastResult::OnCooldown:        return "Spell is not ready yet";
        case CastResult::OnGlobalCooldown:  return "Not ready yet";
        case CastResult::MissingReagent:    return "Missing required reagent";
        case CastResult::NoTarget:          return "No target";
        case CastResult::InvalidTarget:     return "Invalid target";
        case CastResult::TargetDead:        return "Target is dead";
        case CastResult::TargetImmune:      return "Target is immune";
        case CastResult::TargetFriendly:    return "Cannot attack friendly targets";
        case CastResult::TargetHostile:     return "Cannot cast on hostile targets";
        case CastResult::TargetSelf:        return "Cannot target yourself";
        case CastResult::OutOfRange:        return "Out of range";
        case CastResult::TooClose:          return "Target is too close";
        case CastResult::LineOfSight:       return "Target not in line of sight";
        case CastResult::WrongEquipment:    return "Requires proper equipment";
        case CastResult::AreaUnavailable:   return "Cannot cast here";
        case CastResult::CombatRequired:    return "Must be in combat";
        case CastResult::OutOfCombat:       return "Cannot use in combat";
        case CastResult::InternalError:     return "Internal error";
        default:                            return "Unknown error";
    }
}

// ============================================================================
// Main Validation
// ============================================================================

CastResult SpellCaster::validateCast(Entity* caster, int32_t spellId, Entity* target)
{
    // Get spell template
    const SpellTemplate* spell = sGameData.getSpell(spellId);
    if (!spell)
    {
        return CastResult::UnknownSpell;
    }

    return validateCast(caster, spell, target);
}

CastResult SpellCaster::validateCast(Entity* caster, const SpellTemplate* spell, Entity* target)
{
    if (!caster || !spell)
    {
        return CastResult::InternalError;
    }

    // 1. Check caster state (dead, stunned, silenced, etc.)
    CastResult result = checkCasterState(caster, spell);
    if (result != CastResult::Success)
        return result;

    // 2. Check resources (mana, health, cooldowns)
    result = checkResources(caster, spell);
    if (result != CastResult::Success)
        return result;

    // 3. Check equipment requirements
    result = checkEquipment(caster, spell);
    if (result != CastResult::Success)
        return result;

    // 4. Check target validity (only if spell requires target)
    if (!SpellUtils::isSelfOnly(spell))
    {
        result = checkTarget(caster, spell, target);
        if (result != CastResult::Success)
            return result;

        // 5. Check range (only for targeted spells)
        if (target && target != caster)
        {
            result = checkRange(caster, spell, target);
            if (result != CastResult::Success)
                return result;
        }
    }

    return CastResult::Success;
}

// ============================================================================
// Caster State Checks
// ============================================================================

CastResult SpellCaster::checkCasterState(Entity* caster, const SpellTemplate* spell)
{
    // Check if caster is dead
    int health = caster->getVariable(ObjDefines::Variable::Health);
    if (health <= 0)
    {
        return CastResult::CasterDead;
    }

    // Check stunned state (Task 5.8 - Aura System)
    if (caster->isStunned())
    {
        return CastResult::CasterStunned;
    }

    // Check silenced state (for magic spells)
    // Only check for non-physical spells
    if (spell->castSchool != static_cast<int32_t>(SpellDefines::School::Physical))
    {
        if (caster->isSilenced())
        {
            return CastResult::CasterSilenced;
        }
    }

    // Check if already casting (for players)
    Player* player = dynamic_cast<Player*>(caster);
    if (player)
    {
        // TODO: Track current cast in Player class
        // if (player->isCasting()) return CastResult::CasterCasting;

        // Check if player knows this spell
        // TODO: Implement spellbook checking
        // if (!player->knowsSpell(spell->entry)) return CastResult::SpellNotLearned;
    }

    return CastResult::Success;
}

// ============================================================================
// Resource Checks
// ============================================================================

CastResult SpellCaster::checkResources(Entity* caster, const SpellTemplate* spell)
{
    // Get caster's resources
    int32_t currentMana = caster->getVariable(ObjDefines::Variable::Mana);
    int32_t maxMana = caster->getVariable(ObjDefines::Variable::MaxMana);
    int32_t currentHealth = caster->getVariable(ObjDefines::Variable::Health);
    int32_t level = caster->getVariable(ObjDefines::Variable::Level);

    // Calculate mana cost
    int32_t manaCost = SpellUtils::calculateManaCost(spell, level, maxMana);
    if (manaCost > 0 && currentMana < manaCost)
    {
        return CastResult::NotEnoughMana;
    }

    // Check health cost (some spells cost health instead of/in addition to mana)
    int32_t healthCost = spell->healthCost;
    if (spell->healthPctCost > 0)
    {
        int32_t maxHealth = caster->getVariable(ObjDefines::Variable::MaxHealth);
        healthCost += (maxHealth * spell->healthPctCost) / 100;
    }
    if (healthCost > 0 && currentHealth <= healthCost)
    {
        return CastResult::NotEnoughHealth;
    }

    // Check cooldown (Task 5.7)
    Player* player = dynamic_cast<Player*>(caster);
    if (player)
    {
        // Check if the specific spell is on cooldown
        if (player->getCooldowns().isOnCooldown(spell->entry))
        {
            return CastResult::OnCooldown;
        }

        // Check if the spell's category is on cooldown (for shared cooldowns)
        if (spell->cooldownCategory > 0 &&
            player->getCooldowns().isCategoryOnCooldown(spell->cooldownCategory))
        {
            return CastResult::OnCooldown;
        }

        // Check global cooldown
        if (player->getCooldowns().isOnGCD())
        {
            return CastResult::OnGlobalCooldown;
        }
    }

    // Check reagents (consumable items required for the spell)
    // TODO: Implement in Phase 6 (Inventory)
    // for (int i = 0; i < 5; ++i)
    // {
    //     if (spell->reagent[i] > 0)
    //     {
    //         if (!player->hasItem(spell->reagent[i], spell->reagentCount[i]))
    //             return CastResult::MissingReagent;
    //     }
    // }

    return CastResult::Success;
}

// ============================================================================
// Target Checks
// ============================================================================

CastResult SpellCaster::checkTarget(Entity* caster, const SpellTemplate* spell, Entity* target)
{
    // Self-targeted spells don't need target validation
    if (SpellUtils::isSelfOnly(spell))
    {
        return CastResult::Success;
    }

    // Check if we need a target
    if (SpellUtils::requiresTarget(spell) && !target)
    {
        return CastResult::NoTarget;
    }

    // If no target and no target required, use self
    if (!target)
    {
        return CastResult::Success;
    }

    // Check target alive/dead
    int targetHealth = target->getVariable(ObjDefines::Variable::Health);
    bool targetDead = (targetHealth <= 0);

    // Most spells require living targets
    // TODO: Check for resurrection-type spells
    if (targetDead)
    {
        return CastResult::TargetDead;
    }

    // Check friendly/hostile targeting
    bool canTargetFriendly = SpellUtils::canTargetFriendly(spell);
    bool canTargetHostile = SpellUtils::canTargetHostile(spell);
    bool isHostile = areHostile(caster, target);
    bool isFriendly = areFriendly(caster, target);

    // Hostile spell on friendly target
    if (canTargetHostile && !canTargetFriendly && isFriendly && target != caster)
    {
        return CastResult::TargetFriendly;
    }

    // Friendly spell on hostile target
    if (canTargetFriendly && !canTargetHostile && isHostile)
    {
        return CastResult::TargetHostile;
    }

    // Check if target is immune to this spell type
    // TODO: Implement immunity system
    // if (target->isImmuneToSchool(spell->castSchool))
    //     return CastResult::TargetImmune;

    return CastResult::Success;
}

// ============================================================================
// Range Checks
// ============================================================================

CastResult SpellCaster::checkRange(Entity* caster, const SpellTemplate* spell, Entity* target)
{
    if (!target || target == caster)
    {
        return CastResult::Success;  // Self-cast, no range check needed
    }

    float distance = getDistance(caster, target);

    // Check maximum range
    if (spell->range > 0 && distance > static_cast<float>(spell->range))
    {
        return CastResult::OutOfRange;
    }

    // Check minimum range
    if (spell->rangeMin > 0 && distance < static_cast<float>(spell->rangeMin))
    {
        return CastResult::TooClose;
    }

    // Check line of sight
    if (!hasLineOfSight(caster, target))
    {
        return CastResult::LineOfSight;
    }

    return CastResult::Success;
}

// ============================================================================
// Equipment Checks
// ============================================================================

CastResult SpellCaster::checkEquipment(Entity* caster, const SpellTemplate* spell)
{
    // Check required equipment type
    if (spell->requiredEquipment > 0)
    {
        Player* player = dynamic_cast<Player*>(caster);
        if (player)
        {
            // TODO: Check equipped weapon type in Phase 6 (Inventory)
            // if (!player->hasEquippedWeaponType(spell->requiredEquipment))
            //     return CastResult::WrongEquipment;
        }
    }

    return CastResult::Success;
}

// ============================================================================
// Targeting (Task 5.3)
// ============================================================================

std::vector<Entity*> SpellCaster::getTargets(Entity* caster, const SpellTemplate* spell, Entity* target)
{
    std::vector<Entity*> targets;

    if (!caster || !spell)
    {
        return targets;
    }

    // Self-targeted spells
    if (SpellUtils::isSelfOnly(spell))
    {
        targets.push_back(caster);
        return targets;
    }

    // Check each effect's targeting
    for (int i = 0; i < 3; ++i)
    {
        if (spell->effect[i] == 0)
            continue;

        int32_t targetType = spell->effectTargetType[i];
        int32_t radius = spell->effectRadius[i];

        // Target type 0 = self
        if (targetType == 0)
        {
            if (std::find(targets.begin(), targets.end(), caster) == targets.end())
            {
                targets.push_back(caster);
            }
        }
        // Target type 1 = single friendly
        else if (targetType == 1)
        {
            if (target && areFriendly(caster, target))
            {
                if (std::find(targets.begin(), targets.end(), target) == targets.end())
                {
                    targets.push_back(target);
                }
            }
        }
        // Target type 2 = single hostile
        else if (targetType == 2)
        {
            if (target && areHostile(caster, target))
            {
                if (std::find(targets.begin(), targets.end(), target) == targets.end())
                {
                    targets.push_back(target);
                }
            }
        }
        // AoE targeting (radius > 0)
        else if (radius > 0)
        {
            // Get center point for AoE
            float centerX, centerY;
            if (targetType == 3)  // AoE around self
            {
                centerX = caster->getX();
                centerY = caster->getY();
            }
            else if (targetType == 4 && target)  // AoE around target
            {
                centerX = target->getX();
                centerY = target->getY();
            }
            else
            {
                continue;
            }

            // Get all players in range from WorldManager
            int mapId = caster->getMapId();
            auto playersOnMap = sWorldManager.getPlayersOnMap(mapId);

            bool isHostileEffect = (spell->effectPositive[i] == 0);
            float radiusF = static_cast<float>(radius);

            for (Player* player : playersOnMap)
            {
                // Check distance from center
                float dx = player->getX() - centerX;
                float dy = player->getY() - centerY;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist > radiusF)
                    continue;

                // Skip self if not self-buff
                if (player == caster && isHostileEffect)
                    continue;

                // Check friendly/hostile
                if (isHostileEffect && !areHostile(caster, player))
                    continue;
                if (!isHostileEffect && !areFriendly(caster, player))
                    continue;

                // Add to targets if not already present
                if (std::find(targets.begin(), targets.end(), static_cast<Entity*>(player)) == targets.end())
                {
                    targets.push_back(player);
                }
            }

            // TODO: Also check NPCs when NPC system is implemented (Task 5.13)
        }
    }

    // Limit to max targets if specified
    if (spell->maxTargets > 0 && targets.size() > static_cast<size_t>(spell->maxTargets))
    {
        targets.resize(static_cast<size_t>(spell->maxTargets));
    }

    return targets;
}

// ============================================================================
// Utility Functions
// ============================================================================

bool SpellCaster::hasLineOfSight(Entity* caster, Entity* target)
{
    // TODO: Implement line of sight checking using map collision data
    // For now, always return true
    (void)caster;
    (void)target;
    return true;
}

float SpellCaster::getDistance(Entity* a, Entity* b)
{
    if (!a || !b)
        return 0.0f;

    float dx = a->getX() - b->getX();
    float dy = a->getY() - b->getY();
    return std::sqrt(dx * dx + dy * dy);
}

bool SpellCaster::areHostile(Entity* a, Entity* b)
{
    if (!a || !b)
        return false;

    // Same entity is never hostile to itself
    if (a == b)
        return false;

    // TODO: Implement proper faction system
    // For now: Players are hostile to NPCs, NPCs are hostile to players
    // Players are friendly to other players (unless PvP flagged)

    bool aIsPlayer = (dynamic_cast<Player*>(a) != nullptr);
    bool bIsPlayer = (dynamic_cast<Player*>(b) != nullptr);

    // Players vs NPCs are hostile by default
    // (In a real system, this would check faction relationships)
    if (aIsPlayer != bIsPlayer)
    {
        return true;
    }

    // Two players - check PvP status
    // TODO: Implement PvP flag system
    return false;
}

bool SpellCaster::areFriendly(Entity* a, Entity* b)
{
    if (!a || !b)
        return false;

    // Same entity is always friendly to itself
    if (a == b)
        return true;

    // For now, opposite of hostile
    // (In a real system, neutral entities exist too)
    return !areHostile(a, b);
}

bool SpellCaster::hasAura(Entity* entity, int32_t auraId)
{
    if (!entity)
        return false;

    return entity->hasAura(auraId);
}

bool SpellCaster::hasMechanic(Entity* entity, int32_t mechanic)
{
    // Mechanics are special states applied by auras (stun, root, silence, etc.)
    // Currently not implemented - would require checking all auras for their mechanic type
    // For now, return false as mechanic system is future work
    (void)entity;
    (void)mechanic;
    return false;
}
