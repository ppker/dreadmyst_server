### Packet Structure (Wire Format)
```
┌────────────────────┬────────────────────┬─────────────────────┐
│  Size (uint32)     │  Opcode (uint16)   │     Payload         │
│     4 bytes        │     2 bytes        │   Variable size     │
└────────────────────┴────────────────────┴─────────────────────┘
```

**Note:** The `Size` field contains the length of `(Opcode + Payload)`, i.e., `packet_length - 4`.

### Serialization
- Uses `StlBuffer` class for binary serialization
- Little-endian byte order (x86)
- Strings: length-prefixed (size + data)
- Transport: SFML TcpSocket (TCP) using `sf::Packet` class

### SFML Network Layer
SFML's `sf::TcpSocket::send(sf::Packet&)` method for sending packets, not raw byte buffers. The sf::Packet class handles framing internally.

## Opcode Ranges

| Range     | Direction        | Description                    |
|-----------|------------------|--------------------------------|
| 0         | Bidirectional    | Ping/Keepalive                 |
| 100-299   | Server → Client  | Server responses and updates   |
| 300-499   | Client → Server  | Client requests and actions    |


## Bidirectional Packets

| Opcode | Name          | Description        |
|--------|---------------|--------------------|
| 0      | Mutual_Ping   | Connection keepalive |


### Authentication & Session (100-109)

| Opcode | Name                    | Description                           |
|--------|-------------------------|---------------------------------------|
| 100    | Server_Validate         | Auth validation response              |
| 101    | Server_QueuePosition    | Login queue position update           |
| 102    | Server_CharacterList    | List of characters on account         |
| 103    | Server_CharaCreateResult| Character creation result             |
| 104    | Server_NewWorld         | World/map initialization data         |
| 105    | Server_SetController    | Set player's controlled unit          |
| 106    | Server_ChannelInfo      | Chat channel information              |
| 107    | Server_ChannelChangeConfirm | Channel change confirmation       |

### Entity Spawning (110-119)

| Opcode | Name                | Description                       |
|--------|---------------------|-----------------------------------|
| 110    | Server_Player       | Spawn/update player entity        |
| 111    | Server_Npc          | Spawn/update NPC entity           |
| 112    | Server_GameObject   | Spawn/update game object          |
| 113    | Server_DestroyObject| Remove entity from world          |
| 114    | Server_SetSubname   | Set entity subname/title          |

### Movement (120-129)

| Opcode | Name                | Description                       |
|--------|---------------------|-----------------------------------|
| 120    | Server_UnitSpline   | Unit movement spline data         |
| 121    | Server_UnitTeleport | Instant teleport                  |
| 122    | Server_UnitOrientation | Set unit facing direction      |

### Combat & Spells (130-139)

| Opcode | Name                | Description                       |
|--------|---------------------|-----------------------------------|
| 130    | Server_CastStart    | Spell cast begins                 |
| 131    | Server_CastStop     | Spell cast interrupted/cancelled  |
| 132    | Server_SpellGo      | Spell effect executed             |
| 133    | Server_CombatMsg    | Combat log message                |
| 134    | Server_UnitAuras    | Unit aura/buff list               |
| 135    | Server_Cooldown     | Spell cooldown update             |
| 136    | Server_ObjectVariable | Entity variable update          |
| 137    | Server_AggroMob     | Mob aggro notification            |

### Inventory & Items (140-159)

| Opcode | Name                  | Description                       |
|--------|-----------------------|-----------------------------------|
| 140    | Server_Inventory      | Full inventory contents           |
| 141    | Server_Bank           | Bank contents                     |
| 142    | Server_OpenBank       | Bank window opened                |
| 143    | Server_EquipItem      | Item equipped notification        |
| 144    | Server_NotifyItemAdd  | Item added to inventory           |
| 145    | Server_OpenLootWindow | Loot window opened                |
| 146    | Server_OnObjectWasLooted | Object looted notification     |
| 147    | Server_UpdateVendorStock | Vendor stock update            |
| 148    | Server_RepairCost     | Item repair cost response         |
| 149    | Server_SocketResult   | Item socketing result             |
| 150    | Server_EmpowerResult  | Item empowerment result           |

### Spellbook & Abilities (160-169)

| Opcode | Name                  | Description                       |
|--------|-----------------------|-----------------------------------|
| 160    | Server_Spellbook      | Full spellbook contents           |
| 161    | Server_Spellbook_Update | Spellbook modification          |
| 162    | Server_LearnedSpell   | New spell learned notification    |

### Experience & Leveling (170-179)

| Opcode | Name                | Description                       |
|--------|---------------------|-----------------------------------|
| 170    | Server_ExpNotify    | Experience gain notification      |
| 171    | Server_LvlResponse  | Level up response                 |
| 172    | Server_SpentGold    | Gold spent notification           |

### Quests (180-189)

| Opcode | Name                      | Description                       |
|--------|---------------------------|-----------------------------------|
| 180    | Server_QuestList          | Full quest log                    |
| 181    | Server_AcceptedQuest      | Quest accepted notification       |
| 182    | Server_QuestTally         | Quest objective progress          |
| 183    | Server_QuestComplete      | Quest completed notification      |
| 184    | Server_RewardedQuest      | Quest reward received             |
| 185    | Server_AbandonQuest       | Quest abandoned confirmation      |
| 186    | Server_AvailableWorldQuests | Available world quests          |

### Chat & Communication (190-199)

| Opcode | Name              | Description                       |
|--------|-------------------|-----------------------------------|
| 190    | Server_ChatMsg    | Chat message received             |
| 191    | Server_ChatError  | Chat error message                |
| 192    | Server_GossipMenu | NPC gossip dialog menu            |

### Guild (200-209)

| Opcode | Name                       | Description                       |
|--------|----------------------------|-----------------------------------|
| 200    | Server_GuildRoster         | Guild member roster               |
| 201    | Server_GuildInvite         | Guild invite received             |
| 202    | Server_GuildAddMember      | Member joined guild               |
| 203    | Server_GuildRemoveMember   | Member left/kicked from guild     |
| 204    | Server_GuildOnlineStatus   | Member online status change       |
| 205    | Server_GuildNotifyRoleChange | Member role changed             |

### Party (210-219)

| Opcode | Name              | Description                       |
|--------|-------------------|-----------------------------------|
| 210    | Server_PartyList  | Party member list                 |
| 211    | Server_OfferParty | Party invite received             |

### Trading (220-229)

| Opcode | Name                | Description                       |
|--------|---------------------|-----------------------------------|
| 220    | Server_TradeUpdate  | Trade window update               |
| 221    | Server_TradeCanceled| Trade cancelled                   |

### Waypoints/Travel (230-239)

| Opcode | Name                        | Description                       |
|--------|-----------------------------|-----------------------------------|
| 230    | Server_QueryWaypointsResponse | Available waypoints             |
| 231    | Server_DiscoverWaypointNotify | New waypoint discovered         |

### PvP & Arena (240-249)

| Opcode | Name                | Description                       |
|--------|---------------------|-----------------------------------|
| 240    | Server_ArenaQueued  | Arena queue joined                |
| 241    | Server_ArenaReady   | Arena match ready                 |
| 242    | Server_ArenaStatus  | Arena match status                |
| 243    | Server_ArenaOutcome | Arena match result                |
| 244    | Server_OfferDuel    | Duel challenge received           |
| 245    | Server_PkNotify     | PvP kill notification             |

### Miscellaneous (250-299)

| Opcode | Name                 | Description                       |
|--------|----------------------|-----------------------------------|
| 250    | Server_WorldError    | World error message               |
| 251    | Server_InspectReveal | Player inspect data               |
| 252    | Server_PromptRespec  | Respec prompt                     |
| 253    | Server_RespawnResponse | Respawn confirmation            |
| 254    | Server_UnlockGameObj | Game object unlocked              |
| 255    | Server_MarkNpcsOnMap | NPCs marked on minimap            |



## Client → Server Packets (300+)

### Authentication & Character (300-309)

| Opcode | Name                  | Description                       |
|--------|-----------------------|-----------------------------------|
| 300    | Client_Authenticate   | Login authentication request      |
| 301    | Client_CharacterList  | Request character list            |
| 302    | Client_CharCreate     | Create new character              |
| 303    | Client_DeleteCharacter| Delete character                  |
| 304    | Client_EnterWorld     | Enter game world with character   |

### Movement (310-319)

| Opcode | Name                | Description                       |
|--------|---------------------|-----------------------------------|
| 310    | Client_RequestMove  | Request movement to position      |
| 311    | Client_RequestStop  | Request stop movement             |

### Combat & Spells (320-329)

| Opcode | Name              | Description                       |
|--------|-------------------|-----------------------------------|
| 320    | Client_CastSpell  | Cast a spell                      |
| 321    | Client_CancelCast | Cancel current spell cast         |
| 322    | Client_SetSelected| Set target selection              |

### Inventory Management (330-339)

| Opcode | Name                  | Description                       |
|--------|-----------------------|-----------------------------------|
| 330    | Client_EquipItem      | Equip item from inventory         |
| 331    | Client_UnequipItem    | Unequip item to inventory         |
| 332    | Client_MoveItem       | Move item between slots           |
| 333    | Client_SplitItemStack | Split item stack                  |
| 334    | Client_DestroyItem    | Destroy/delete item               |
| 335    | Client_UseItem        | Use consumable item               |
| 336    | Client_SortInventory  | Auto-sort inventory               |
| 337    | Client_ReqAbilityList | Request ability list              |
| 338    | Client_LootItem       | Loot item from corpse/chest       |

### Banking (340-349)

| Opcode | Name                    | Description                       |
|--------|-------------------------|-----------------------------------|
| 340    | Client_OpenBank         | Open bank window                  |
| 341    | Client_MoveInventoryToBank | Deposit item to bank           |
| 342    | Client_MoveBankToBank   | Move item within bank             |
| 343    | Client_UnBankItem       | Withdraw item from bank           |
| 344    | Client_SortBank         | Auto-sort bank                    |

### Vendor & Trading (350-359)

| Opcode | Name                | Description                       |
|--------|---------------------|-----------------------------------|
| 350    | Client_BuyVendorItem| Purchase from vendor              |
| 351    | Client_SellItem     | Sell item to vendor               |
| 352    | Client_Buyback      | Buyback sold item                 |
| 353    | Client_OpenTradeWith| Initiate trade with player        |
| 354    | Client_TradeAddItem | Add item to trade window          |
| 355    | Client_TradeRemoveItem | Remove item from trade window  |
| 356    | Client_TradeSetGold | Set gold amount in trade          |
| 357    | Client_TradeConfirm | Confirm/accept trade              |
| 358    | Client_TradeCancel  | Cancel trade                      |
| 359    | Client_Repair       | Repair equipment                  |

### Quests (360-369)

| Opcode | Name                    | Description                       |
|--------|-------------------------|-----------------------------------|
| 360    | Client_AcceptQuest      | Accept quest from NPC             |
| 361    | Client_CompleteQuest    | Turn in completed quest           |
| 362    | Client_AbandonQuest     | Abandon active quest              |
| 363    | Client_ClickedGossipOption | Click gossip dialog option     |

### Chat (370-379)

| Opcode | Name                  | Description                       |
|--------|-----------------------|-----------------------------------|
| 370    | Client_ChatMsg        | Send chat message                 |
| 371    | Client_SetIgnorePlayer| Ignore/unignore player            |

### Guild (380-389)

| Opcode | Name                     | Description                       |
|--------|--------------------------|-----------------------------------|
| 380    | Client_GuildCreate       | Create new guild                  |
| 381    | Client_GuildInviteMember | Invite player to guild            |
| 382    | Client_GuildInviteResponse | Accept/decline guild invite     |
| 383    | Client_GuildQuit         | Leave guild                       |
| 384    | Client_GuildKickMember   | Kick member from guild            |
| 385    | Client_GuildPromoteMember| Promote guild member              |
| 386    | Client_GuildDemoteMember | Demote guild member               |
| 387    | Client_GuildDisband      | Disband guild                     |
| 388    | Client_GuildMotd         | Set guild message of the day      |
| 389    | Client_GuildRosterRequest| Request guild roster              |

### Party (390-399)

| Opcode | Name                      | Description                       |
|--------|---------------------------|-----------------------------------|
| 390    | Client_PartyInviteMember  | Invite player to party            |
| 391    | Client_PartyInviteResponse| Accept/decline party invite       |
| 392    | Client_PartyChanges       | Party setting changes             |

### PvP (400-409)

| Opcode | Name                    | Description                       |
|--------|-------------------------|-----------------------------------|
| 400    | Client_DuelResponse     | Accept/decline duel               |
| 401    | Client_YieldDuel        | Yield/forfeit duel                |
| 402    | Client_TogglePvP        | Toggle PvP flag                   |
| 403    | Client_UpdateArenaStatus| Update arena queue status         |

### Character Progression (410-419)

| Opcode | Name                  | Description                       |
|--------|-----------------------|-----------------------------------|
| 410    | Client_Respec         | Respec character                  |
| 411    | Client_LevelUp        | Spend level-up point              |
| 412    | Client_QueryWaypoints | Request available waypoints       |
| 413    | Client_ActivateWaypoint| Use waypoint for travel          |
| 414    | Client_RequestRespawn | Request respawn after death       |
| 415    | Client_ResetDungeons  | Reset dungeon instances           |
| 416    | Client_SocketItem     | Socket gem into item              |
| 417    | Client_EmpowerItem    | Empower/enhance item              |
| 418    | Client_RollDice       | Roll dice (random)                |
| 419    | Client_ReportPlayer   | Report player for abuse           |

### Admin/Moderation (420+)

| Opcode | Name                  | Description                       |
|--------|-----------------------|-----------------------------------|
| 420    | Client_MOD            | Moderator command                 |
| 421    | Client_RecoverMailLoot| Recover mail attachment           |


## Unknowns (Server > Client only)

| Opcode | Count | Known Name | Notes |
|--------|-------|------------|-------|
| 91 | 3681 | **Unknown** | Most frequent - likely entity tick/state updates |
| 144 | 2684 | Server_NotifyItemAdd | Inventory/loot item additions |
| 92 | 304 | **Unknown** | Frequent updates |
| 88 | 169 | **Unknown** | |
| 83 | 131 | **Unknown** | |
| 103 | 71 | Server_CharaCreateResult? | Or different opcode |
| 100 | 48 | Server_Validate | Auth validation packets |
| 110 | 12 | Server_Player | Player entity spawns |
| 105 | 9 | Server_SetController | Controller set events |
| 104 | 8 | Server_NewWorld | Zone/world loading |


| Opcode | Count | Estimated Purpose |
|--------|-------|-------------------|
| 78 | 50 | Unknown - possibly UI/system |
| 79 | 1 | Unknown |
| 83 | 131 | High frequency - possibly object/entity updates |
| 87 | 11 | Unknown |
| 88 | 169 | High frequency - possibly state sync |
| 91 | 3681 | **Very high frequency** - likely tick/heartbeat updates |
| 92 | 304 | High frequency - possibly entity movement/state |
| 99 | 30 | Unknown |
| 103 | 71 | May be different from CharaCreateResult |
| 108 | 50 | Near Server_Player range - possibly entity-related |
| 109 | 35 | Near Server_Player range |
| 119 | 2 | Near Movement range |
| 126 | 6 | Unknown |
| 129 | 1 | Near Combat range |
| 138 | 5 | Near AggroMob (137) - combat related? |


| Event Type | Opcode | Count |
|------------|--------|-------|
| Spell Cast Start | 130 | 1 |
| Spell Cast Stop | 131 | 2 |
| Cooldown Update | 135 | 2 |
| Unit Movement | 120 | 2 |
| Unit Orientation | 122 | 2 |
| NPC Spawn | 111 | 1 |


