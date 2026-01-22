#pragma once

#include "StlBuffer.h"
#include <stdint.h>

// Packet opcodes - UPDATED from Frida binary analysis (2026-01-22)
// Wire format: [4-byte length (uint32 LE)] [2-byte opcode (uint16 LE)] [payload]
//
// VERIFIED opcodes are marked with confidence level:
//   [VERIFIED] - Confirmed from packet capture with payload analysis
//   [MAPPED]   - Mapped based on source handler analysis and packet size patterns
//   [ESTIMATED] - Estimated based on opcode range patterns
//
namespace Opcode
{
    //=========================================================================
    // Bidirectional (0x00)
    //=========================================================================
    constexpr uint16_t Mutual_Ping = 0x00;

    //=========================================================================
    // Server -> Client (0x50 - 0xFF range based on binary analysis)
    //=========================================================================

    // Authentication & Login (0x50-0x57)
    constexpr uint16_t Server_Validate = 0x50;              // [MAPPED] Auth result
    constexpr uint16_t Server_QueuePosition = 0x51;         // [MAPPED] Queue position
    constexpr uint16_t Server_CharacterList = 0x52;         // [MAPPED] Character list
    constexpr uint16_t Server_CharaCreateResult = 0x53;     // [MAPPED] 11 bytes observed
    constexpr uint16_t Server_NewWorld = 0x54;              // [MAPPED] Map change
    constexpr uint16_t Server_SetController = 0x55;         // [MAPPED] Set player control
    constexpr uint16_t Server_ChannelInfo = 0x56;           // [MAPPED] Channel info
    constexpr uint16_t Server_ChannelChangeConfirm = 0x57;  // [MAPPED] Channel change

    // Object Management (0x58-0x5F)
    constexpr uint16_t Server_DestroyObject = 0x58;         // [VERIFIED] 4 bytes = single guid
    constexpr uint16_t Server_SetSubname = 0x59;            // [MAPPED] Set object subname
    constexpr uint16_t Server_GameObject = 0x5A;            // [MAPPED] Game object spawn
    constexpr uint16_t Server_UnitSpline = 0x5B;            // [VERIFIED] 18-26 bytes, movement
    constexpr uint16_t Server_Player = 0x5C;                // [VERIFIED] 54-94 bytes, player spawn
    constexpr uint16_t Server_Npc = 0x5D;                   // [MAPPED] NPC spawn
    constexpr uint16_t Server_UnitTeleport = 0x5E;          // [MAPPED] Unit teleport
    constexpr uint16_t Server_UnitOrientation = 0x5F;       // [MAPPED] Unit facing

    // Combat (0x60-0x6F)
    constexpr uint16_t Server_CastStart = 0x60;             // [MAPPED] Spell cast start
    constexpr uint16_t Server_CastStop = 0x61;              // [MAPPED] Spell cast stop
    constexpr uint16_t Server_SpellGo = 0x62;               // [MAPPED] Spell fired
    constexpr uint16_t Server_CombatMsg = 0x63;             // [VERIFIED] 12 bytes observed
    constexpr uint16_t Server_UnitAuras = 0x64;             // [VERIFIED] 8 bytes observed
    constexpr uint16_t Server_Cooldown = 0x65;              // [MAPPED] Cooldown update
    constexpr uint16_t Server_AggroMob = 0x66;              // [MAPPED] Aggro state
    constexpr uint16_t Server_PkNotify = 0x67;              // [VERIFIED] 21 bytes observed

    // Inventory & Items (0x68-0x77)
    constexpr uint16_t Server_Inventory = 0x68;             // [VERIFIED] 18 bytes observed
    constexpr uint16_t Server_Bank = 0x69;                  // [VERIFIED] 14-35 bytes variable
    constexpr uint16_t Server_OpenBank = 0x6A;              // [MAPPED] Open bank UI
    constexpr uint16_t Server_EquipItem = 0x6B;             // [MAPPED] Equipment change
    constexpr uint16_t Server_NotifyItemAdd = 0x6C;         // [VERIFIED] 14 bytes observed
    constexpr uint16_t Server_OpenLootWindow = 0x6D;        // [MAPPED] Loot window
    constexpr uint16_t Server_OnObjectWasLooted = 0x6E;     // [MAPPED] Loot notification
    constexpr uint16_t Server_UpdateVendorStock = 0x6F;     // [MAPPED] Vendor stock
    constexpr uint16_t Server_RepairCost = 0x70;            // [MAPPED] Repair cost
    constexpr uint16_t Server_SocketResult = 0x71;          // [MAPPED] Socket result
    constexpr uint16_t Server_EmpowerResult = 0x72;         // [MAPPED] Empower result

    // Spells & Experience (0x78-0x7F)
    constexpr uint16_t Server_Spellbook = 0x78;             // [MAPPED] Full spellbook
    constexpr uint16_t Server_Spellbook_Update = 0x79;      // [MAPPED] Spell update
    constexpr uint16_t Server_LearnedSpell = 0x7A;          // [MAPPED] New spell learned
    constexpr uint16_t Server_ExpNotify = 0x7B;             // [MAPPED] XP gained
    constexpr uint16_t Server_LvlResponse = 0x7C;           // [MAPPED] Level up response
    constexpr uint16_t Server_SpentGold = 0x7D;             // [MAPPED] Gold spent

    // Quests (0x80-0x86)
    constexpr uint16_t Server_QuestList = 0x80;             // [MAPPED] Quest log
    constexpr uint16_t Server_AcceptedQuest = 0x81;         // [MAPPED] Quest accepted
    constexpr uint16_t Server_QuestTally = 0x82;            // [MAPPED] Quest progress
    constexpr uint16_t Server_QuestComplete = 0x83;         // [MAPPED] Quest complete
    constexpr uint16_t Server_RewardedQuest = 0x84;         // [MAPPED] Quest rewarded
    constexpr uint16_t Server_AbandonQuest = 0x85;          // [MAPPED] Quest abandoned
    constexpr uint16_t Server_AvailableWorldQuests = 0x86;  // [MAPPED] World quests

    // Chat & Gossip (0x87-0x89)
    constexpr uint16_t Server_ChatMsg = 0x87;               // [VERIFIED] 10 bytes observed
    constexpr uint16_t Server_ChatError = 0x88;             // [MAPPED] Chat error
    constexpr uint16_t Server_GossipMenu = 0x89;            // [MAPPED] NPC gossip

    // Guilds (0x8A-0x8F)
    constexpr uint16_t Server_GuildRoster = 0x8A;           // [MAPPED] Guild roster
    constexpr uint16_t Server_GuildInvite = 0x8B;           // [MAPPED] Guild invite
    constexpr uint16_t Server_GuildAddMember = 0x8C;        // [MAPPED] Member added
    constexpr uint16_t Server_GuildRemoveMember = 0x8D;     // [MAPPED] Member removed
    constexpr uint16_t Server_GuildOnlineStatus = 0x8E;     // [MAPPED] Online status
    constexpr uint16_t Server_GuildNotifyRoleChange = 0x8F; // [MAPPED] Role change

    // Object Variables & Bulk Updates (0x90)
    constexpr uint16_t Server_ObjectVariable = 0x90;        // [VERIFIED] 154 bytes, very frequent

    // Party & Social (0x91-0x95)
    constexpr uint16_t Server_PartyList = 0x91;             // [MAPPED] Party members
    constexpr uint16_t Server_OfferParty = 0x92;            // [MAPPED] Party invite

    // Trade (0x93-0x94)
    constexpr uint16_t Server_TradeUpdate = 0x93;           // [MAPPED] Trade state
    constexpr uint16_t Server_TradeCanceled = 0x94;         // [MAPPED] Trade canceled

    // Travel & Waypoints (0x95-0x96)
    constexpr uint16_t Server_QueryWaypointsResponse = 0x95; // [MAPPED] Waypoints
    constexpr uint16_t Server_DiscoverWaypointNotify = 0x96; // [MAPPED] New waypoint

    // Arena & PvP (0x97-0x9C)
    constexpr uint16_t Server_ArenaQueued = 0x97;           // [MAPPED] Arena queue
    constexpr uint16_t Server_ArenaReady = 0x98;            // [MAPPED] Arena ready
    constexpr uint16_t Server_ArenaStatus = 0x99;           // [MAPPED] Arena status
    constexpr uint16_t Server_ArenaOutcome = 0x9A;          // [MAPPED] Arena result
    constexpr uint16_t Server_OfferDuel = 0x9B;             // [MAPPED] Duel invite

    // Misc Server (0x9C-0xA0)
    constexpr uint16_t Server_WorldError = 0x9C;            // [MAPPED] Error message
    constexpr uint16_t Server_InspectReveal = 0x9D;         // [MAPPED] Inspect player
    constexpr uint16_t Server_PromptRespec = 0x9E;          // [MAPPED] Respec prompt
    constexpr uint16_t Server_RespawnResponse = 0x9F;       // [MAPPED] Respawn result
    constexpr uint16_t Server_UnlockGameObj = 0xA0;         // [MAPPED] Unlock object
    constexpr uint16_t Server_MarkNpcsOnMap = 0xA1;         // [MAPPED] NPC markers

    //=========================================================================
    // Client -> Server (0x01 - 0x30 range based on binary analysis)
    //=========================================================================

    // Ping & Auth (0x01-0x06)
    constexpr uint16_t Client_Ping = 0x01;                  // [VERIFIED] 10 bytes periodic
    constexpr uint16_t Client_Authenticate = 0x02;          // [MAPPED] Login token
    constexpr uint16_t Client_CharacterList = 0x03;         // [MAPPED] Request chars
    constexpr uint16_t Client_CharCreate = 0x04;            // [MAPPED] Create char
    constexpr uint16_t Client_DeleteCharacter = 0x05;       // [MAPPED] Delete char
    constexpr uint16_t Client_EnterWorld = 0x06;            // [MAPPED] Enter world

    // Spells (0x07-0x09)
    constexpr uint16_t Client_CastSpell = 0x07;             // [MAPPED] Cast spell
    constexpr uint16_t Client_CancelCast = 0x08;            // [MAPPED] Cancel cast
    constexpr uint16_t Client_CancelBuff = 0x09;            // [MAPPED] Cancel buff

    // Chat & Social (0x0A-0x0E)
    constexpr uint16_t Client_ChatMsg = 0x0A;               // [MAPPED] Send message
    constexpr uint16_t Client_ClickedGossipOption = 0x0B;   // [MAPPED] Gossip click
    constexpr uint16_t Client_AcceptQuest = 0x0C;           // [MAPPED] Accept quest
    constexpr uint16_t Client_AbandonQuest = 0x0D;          // [MAPPED] Abandon quest
    constexpr uint16_t Client_CompleteQuest = 0x0E;         // [MAPPED] Complete quest

    // Movement (0x0F-0x10)
    constexpr uint16_t Client_RequestMove = 0x0F;           // [VERIFIED] 11 bytes, very high freq
    constexpr uint16_t Client_RequestStop = 0x10;           // [VERIFIED] 10 bytes, follows movement

    // Abilities & Items (0x11-0x1A)
    constexpr uint16_t Client_ReqAbilityList = 0x11;        // [MAPPED] Request abilities
    constexpr uint16_t Client_ReqTheoreticalSpell = 0x12;   // [MAPPED] Spell preview
    constexpr uint16_t Client_SetSelected = 0x13;           // [MAPPED] Select target
    constexpr uint16_t Client_EquipItem = 0x14;             // [MAPPED] Equip item
    constexpr uint16_t Client_UnequipItem = 0x15;           // [MAPPED] Unequip item
    constexpr uint16_t Client_MoveItem = 0x16;              // [MAPPED] Move inventory
    constexpr uint16_t Client_SplitItemStack = 0x17;        // [MAPPED] Split stack
    constexpr uint16_t Client_DestroyItem = 0x18;           // [MAPPED] Destroy item
    constexpr uint16_t Client_UseItem = 0x19;               // [MAPPED] Use item
    constexpr uint16_t Client_SortInventory = 0x1A;         // [MAPPED] Sort inventory

    // Actions (0x1B)
    constexpr uint16_t Client_Action = 0x1B;                // [VERIFIED] 19 bytes, frequent

    // Bank (0x1C-0x1F)
    constexpr uint16_t Client_MoveInventoryToBank = 0x1C;   // [MAPPED] Inv to bank
    constexpr uint16_t Client_OpenBank = 0x1D;              // [VERIFIED] 6 bytes
    constexpr uint16_t Client_MoveBankToBank = 0x1E;        // [VERIFIED] 12 bytes
    constexpr uint16_t Client_UnBankItem = 0x1F;            // [MAPPED] Bank to inv
    constexpr uint16_t Client_SortBank = 0x20;              // [MAPPED] Sort bank

    // Vendor & Trade (0x21-0x28)
    constexpr uint16_t Client_BuyVendorItem = 0x21;         // [MAPPED] Buy from vendor
    constexpr uint16_t Client_SellItem = 0x22;              // [MAPPED] Sell to vendor
    constexpr uint16_t Client_Buyback = 0x23;               // [MAPPED] Buyback item
    constexpr uint16_t Client_LootItem = 0x24;              // [MAPPED] Loot item
    constexpr uint16_t Client_OpenTradeWith = 0x25;         // [MAPPED] Open trade
    constexpr uint16_t Client_TradeAddItem = 0x26;          // [MAPPED] Add to trade
    constexpr uint16_t Client_TradeRemoveItem = 0x27;       // [MAPPED] Remove from trade
    constexpr uint16_t Client_TradeSetGold = 0x28;          // [MAPPED] Set trade gold
    constexpr uint16_t Client_TradeConfirm = 0x29;          // [MAPPED] Confirm trade
    constexpr uint16_t Client_TradeCancel = 0x2A;           // [MAPPED] Cancel trade
    constexpr uint16_t Client_Repair = 0x2B;                // [MAPPED] Repair items

    // Guild (0x2C-0x35)
    constexpr uint16_t Client_GuildCreate = 0x2C;           // [MAPPED] Create guild
    constexpr uint16_t Client_GuildInviteMember = 0x2D;     // [MAPPED] Invite member
    constexpr uint16_t Client_GuildInviteResponse = 0x2E;   // [MAPPED] Accept/decline
    constexpr uint16_t Client_GuildQuit = 0x2F;             // [MAPPED] Leave guild
    constexpr uint16_t Client_GuildKickMember = 0x30;       // [MAPPED] Kick member
    constexpr uint16_t Client_GuildPromoteMember = 0x31;    // [MAPPED] Promote
    constexpr uint16_t Client_GuildDemoteMember = 0x32;     // [MAPPED] Demote
    constexpr uint16_t Client_GuildDisband = 0x33;          // [MAPPED] Disband guild
    constexpr uint16_t Client_GuildMotd = 0x34;             // [MAPPED] Set MOTD
    constexpr uint16_t Client_GuildRosterRequest = 0x35;    // [MAPPED] Request roster

    // Party (0x36-0x38)
    constexpr uint16_t Client_PartyInviteMember = 0x36;     // [MAPPED] Party invite
    constexpr uint16_t Client_PartyInviteResponse = 0x37;   // [MAPPED] Accept/decline
    constexpr uint16_t Client_PartyChanges = 0x38;          // [MAPPED] Party changes

    // PvP & Duel (0x39-0x3C)
    constexpr uint16_t Client_DuelResponse = 0x39;          // [MAPPED] Duel accept
    constexpr uint16_t Client_YieldDuel = 0x3A;             // [MAPPED] Yield duel
    constexpr uint16_t Client_TogglePvP = 0x3B;             // [MAPPED] Toggle PvP
    constexpr uint16_t Client_UpdateArenaStatus = 0x3C;     // [MAPPED] Arena queue

    // Misc Client (0x3D-0x48)
    constexpr uint16_t Client_Respec = 0x3D;                // [MAPPED] Respec
    constexpr uint16_t Client_LevelUp = 0x3E;               // [MAPPED] Spend points
    constexpr uint16_t Client_QueryWaypoints = 0x3F;        // [MAPPED] Get waypoints
    constexpr uint16_t Client_ActivateWaypoint = 0x40;      // [MAPPED] Use waypoint
    constexpr uint16_t Client_RequestRespawn = 0x41;        // [MAPPED] Respawn
    constexpr uint16_t Client_ResetDungeons = 0x42;         // [MAPPED] Reset dungeons
    constexpr uint16_t Client_SocketItem = 0x43;            // [MAPPED] Socket gem
    constexpr uint16_t Client_EmpowerItem = 0x44;           // [MAPPED] Empower item
    constexpr uint16_t Client_RollDice = 0x45;              // [MAPPED] Roll dice
    constexpr uint16_t Client_ReportPlayer = 0x46;          // [MAPPED] Report player
    constexpr uint16_t Client_MOD = 0x47;                   // [MAPPED] GM command
    constexpr uint16_t Client_RecoverMailLoot = 0x48;       // [MAPPED] Get mail
    constexpr uint16_t Client_SetToolbarSlot = 0x49;        // [MAPPED] Set hotbar
    constexpr uint16_t Client_ChangeChannels = 0x4A;        // [MAPPED] Change channel
    constexpr uint16_t Client_SetIgnorePlayer = 0x4B;       // [MAPPED] Ignore player

    constexpr uint16_t MaxOpcode = 0x100;
}

// Base packet interface
class GamePacket
{
public:
    virtual ~GamePacket() = default;
    virtual uint16_t getOpcode() const = 0;
    virtual void pack(StlBuffer& buf) const = 0;
    virtual void unpack(StlBuffer& buf) = 0;

    // Build packet with opcode header
    StlBuffer build(StlBuffer buf) const
    {
        (void)buf;
        StlBuffer result;
        uint16_t op = getOpcode();
        result << op;
        pack(result);
        return result;
    }

    static bool validOpcode(uint16_t opcode)
    {
        return opcode < Opcode::MaxOpcode;
    }
};
