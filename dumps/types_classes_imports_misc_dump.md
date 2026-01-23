**Analysis Date:** 2026-01-23
**Binary Name:** Dreadmyst Client
**Architecture:** x86 (32-bit)
**Compiler:** MSVC (Visual C++)
**Entry Point:** 0x0060aba2

## Overview

This is the game client for **Dreadmyst**, an MMORPG-style game. The client is built using:

- **SFML** (Simple and Fast Multimedia Library) for graphics, audio, and networking
- **SQLite** for local database storage
- **Steam API** for platform integration
- **libzip** for archive handling
- **nlohmann/json** for JSON parsing

## Memory Layout

| Segment | Start Address | End Address | Size | Description |
|---------|---------------|-------------|------|-------------|
| Headers | 0x00400000 | 0x004003ff | 1 KB | PE Headers |
| .text | 0x00401000 | 0x006283ff | ~2.2 MB | Code Section |
| .rdata | 0x00629000 | 0x0067cfff | ~335 KB | Read-only Data |
| .data | 0x0067d000 | 0x0068479b | ~30 KB | Initialized Data |
| .detourc | 0x00685000 | 0x006861ff | ~6 KB | Detours Code |
| .detourd | 0x00687000 | 0x006871ff | ~2 KB | Detours Data |
| .rsrc | 0x00688000 | 0x0068dbff | ~22 KB | Resources |
| .reloc | 0x0068e000 | 0x006a7dff | ~103 KB | Relocations |
| tdb | 0xffdff000 | 0xffdfffff | 4 KB | Thread Data Block |

**Note:** The `.detourc` and `.detourd` sections indicate the binary may use Microsoft Detours for API hooking.


## Imports Analysis

### Graphics & Multimedia (SFML)

The client uses SFML extensively for rendering and multimedia:

**Graphics:**
- `sf::RenderWindow` - Main game window
- `sf::Sprite`, `sf::Texture`, `sf::Font` - 2D graphics
- `sf::CircleShape`, `sf::RectangleShape`, `sf::ConvexShape` - Primitive shapes
- `sf::VertexArray` - Custom vertex rendering
- `sf::Shader` - GLSL shaders
- `sf::RenderTexture` - Off-screen rendering
- `sf::View` - Camera/viewport control

**Audio:**
- `sf::Sound`, `sf::SoundBuffer` - Sound effects
- `sf::Music` - Background music streaming

**Networking:**
- `sf::TcpSocket` - TCP networking
- `sf::IpAddress` - IP address handling

**System:**
- `sf::Clock`, `sf::Time` - Timing
- `sf::Keyboard`, `sf::Mouse` - Input
- `sf::Cursor` - Custom cursors

### Windows API (KERNEL32.DLL)

Key system functions used:

| Function | Purpose |
|----------|---------|
| `CreateFileA/W` | File operations |
| `VirtualAlloc/Free/Protect` | Memory management |
| `GetThreadContext/SetThreadContext` | Thread manipulation |
| `LoadLibraryA/W` | Dynamic library loading |
| `GetProcAddress` | Symbol resolution |
| `CreateMutexW` | Process synchronization |
| `HeapAlloc/HeapFree` | Heap memory |
| `GetModuleHandleA/W` | Module handles |
| `GetTickCount64` | Timing |
| `K32GetProcessMemoryInfo` | Memory diagnostics |

### Database (SQLite)

SQLite3 integration for local data storage:
- `sqlite3_*` functions for database operations
- Used for caching game data locally

### Compression (ZIP.DLL)

libzip for archive handling:
- `zip_open`, `zip_close`
- `zip_fopen`, `zip_fread`, `zip_fclose`
- `zip_get_num_entries`, `zip_get_name`
- `zip_stat`, `zip_stat_init`

### Steam Integration (STEAM_API.DLL)

Steam API for platform features:
- `SteamInternal_SteamAPI_Init`
- `SteamAPI_Shutdown`

Steam interfaces used:
- SteamUser023
- SteamFriends018
- SteamMatchMaking009
- SteamMatchMakingServers002
- SteamNetworking006
- SteamNetworkingSockets012
- STEAMUGC_INTERFACE_VERSION021
- STEAMINVENTORY_INTERFACE_V003
- And more...

### Graphics APIs

- **D3D11.DLL** - Direct3D 11
- **DXGI.DLL** - DirectX Graphics Infrastructure
- **OPENGL32.DLL** - OpenGL
- **GDI32.DLL** - Windows GDI

### NVIDIA Support

NVIDIA API integration for GPU detection:
- `nvapi_QueryInterface`
- `nvapi_pepQueryInterface`
- Libraries: `nvapi.dll`, `nvldumd.dll`, `nvldumdx.dll`

## Classes and Namespaces

### Core Game Classes

| Class | Description |
|-------|-------------|
| `Application` | Main application entry |
| `Game` | Core game logic |
| `World` | World management |
| `WorldPanel` | World rendering |
| `ClientMap` | Map handling |
| `MapCellClient` | Map cell management |
| `MapEditor` | Map editing tools |

### Entity Classes

| Class | Description |
|-------|-------------|
| `ClientObject` | Base game object |
| `ClientUnit` | Unit (player/NPC) base |
| `ClientPlayer` | Player character |
| `ClientNpc` | NPC character |
| `ClientGameObj` | Game objects |
| `MutualObject` | Shared object data |
| `MutualUnit` | Shared unit data |

### UI Classes

| Class | Description |
|-------|-------------|
| `Window` | Base window class |
| `Button` | Clickable button |
| `TextBox` / `TextBoxRo` | Text input/display |
| `ScrollBar` | Scrollable container |
| `ContextMenu` | Right-click menus |
| `Tooltip` | Hover tooltips |
| `ConfirmMessageBox` | Confirmation dialogs |
| `PopupPrompt` | Popup prompts |
| `ExpandableWindow` | Collapsible windows |

### Game Systems UI

| Class | Description |
|-------|-------------|
| `Login` | Login screen |
| `CharacterSelection` | Character select |
| `CharacterCreation` | Character creator |
| `Inventory` | Inventory UI |
| `Equipment` | Equipment UI |
| `Bank` | Bank storage UI |
| `SpellIcon` / `Abilities` | Spell/ability bars |
| `QuestLog` / `QuestObjectives` | Quest tracking |
| `QuestOffer` / `QuestComplete` | Quest dialogs |
| `CastBar` | Spell casting bar |
| `UnitFrame` | Health/mana frames |
| `Minimap` | Minimap display |
| `GameChat` | Chat system |
| `GuildRoster` | Guild management |
| `TradeWindow` | Trading UI |
| `VendorNpc` | Vendor interface |
| `LootWindow` | Loot pickup |
| `ConsoleWindow` | Debug console |

### Rendering Classes

| Class | Description |
|-------|-------------|
| `Sprite` / `SpriteRo` | Sprite rendering |
| `SpriteAnimation` | Animated sprites |
| `ParticleSystem` | Particle effects |
| `WorldSpellAnimation` | Spell visual effects |
| `RenderObject` | Base render object |
| `RenderObjectHolder` | Render object container |
| `SpellVisualKit` | Spell effect data |

### Data/Template Classes

| Class | Description |
|-------|-------------|
| `GamePacket` | Network packet base |
| `GameIcon` / `GameIconList` | Icon management |
| `ItemIcon` / `ItemEntry` | Item display |
| `SpellEntry` | Spell data |
| `SqlConnector` / `SQLiteConnector` | Database connection |

### Network Classes

| Class | Description |
|-------|-------------|
| `Socket` / `SfSocket` | Network socket wrapper |
| `TcpSocketEx` | Extended TCP socket |


## Network Protocol

### Client-to-Server Packets (GP_Client_*)

| Packet | Description |
|--------|-------------|
| `GP_Client_Authenticate` | Login authentication |
| `GP_Client_CharCreate` | Create character |
| `GP_Client_RequestMove` | Movement request |
| `GP_Client_CastSpell` | Cast a spell |
| `GP_Client_UseItem` | Use an item |
| `GP_Client_EquipItem` | Equip item |
| `GP_Client_ChatMsg` | Send chat message |
| `GP_Client_LevelUp` | Level up action |
| `GP_Client_Repair` | Repair items |
| `GP_Client_OpenTradeWith` | Initiate trade |
| `GP_Client_TransmuteItems` | Transmute items |
| `GP_Client_MoveInventoryToBank` | Move to bank |
| `GP_Client_UnBankItem` | Withdraw from bank |
| `GP_Client_PartyInviteMember` | Invite to party |
| `GP_Client_PartyChanges` | Party modifications |
| `GP_Client_GuildCreate` | Create guild |
| `GP_Client_GuildInviteMember` | Invite to guild |
| `GP_Client_GuildInviteResponse` | Accept/decline guild invite |
| `GP_Client_GuildPromoteMember` | Promote guild member |
| `GP_Client_GuildDemoteMember` | Demote guild member |
| `GP_Client_GuildMotd` | Set guild MOTD |
| `GP_Client_UpdateArenaStatus` | Arena status update |
| `GP_Client_SetIgnorePlayer` | Ignore player |
| `GP_Client_ReportPlayer` | Report player |
| `GP_Client_MOD_KickPlayer` | Moderator: kick |
| `GP_Client_MOD_BanPlayer` | Moderator: ban |
| `GP_Client_MOD_MutePlayer` | Moderator: mute |
| `GP_Client_MOD_WarnPlayer` | Moderator: warn |

### Server-to-Client Packets (GP_Server_*)

| Packet | Description |
|--------|-------------|
| `GP_Server_CharacterList` | Character list |
| `GP_Server_Object` | Object spawn/update |
| `GP_Server_Unit` | Unit data |
| `GP_Server_Player` | Player data |
| `GP_Server_Npc` | NPC data |
| `GP_Server_GameObject` | Game object data |
| `GP_Server_Inventory` | Inventory contents |
| `GP_Server_Bank` | Bank contents |
| `GP_Server_Spellbook` | Spellbook data |
| `GP_Server_Spellbook_Update` | Spellbook changes |
| `GP_Server_SpellGo` | Spell cast notification |
| `GP_Server_UnitAuras` | Buff/debuff data |
| `GP_Server_UnitSpline` | Movement spline |
| `GP_Server_ChatMsg` | Chat message |
| `GP_Server_ChannelInfo` | Chat channel info |
| `GP_Server_QuestList` | Quest list |
| `GP_Server_AvailableWorldQuests` | Available quests |
| `GP_Server_GossipMenu` | NPC dialog menu |
| `GP_Server_OpenLootWindow` | Loot available |
| `GP_Server_NotifyItemAdd` | Item added |
| `GP_Server_TradeUpdate` | Trade window update |
| `GP_Server_PartyList` | Party members |
| `GP_Server_OfferParty` | Party invitation |
| `GP_Server_OfferDuel` | Duel challenge |
| `GP_Server_InspectReveal` | Inspect player |
| `GP_Server_GuildRoster` | Guild member list |
| `GP_Server_GuildInvite` | Guild invitation |
| `GP_Server_GuildAddMember` | Member joined guild |
| `GP_Server_GuildOnlineStatus` | Member online status |
| `GP_Server_GuildNotifyRoleChange` | Role changed |
| `GP_Server_PkNotify` | PK notification |
| `GP_Server_SetSubname` | Set player subname |
| `GP_Server_MarkNpcsOnMap` | NPC markers |
| `GP_Server_QueryWaypointsResponse` | Waypoint data |

### DLLs

| DLL | Purpose |
|-----|---------|
| KERNEL32.DLL | Windows core |
| USER32.DLL | Windows UI |
| GDI32.DLL | Graphics |
| ADVAPI32.DLL | Security/registry |
| SHELL32.DLL | Shell operations |
| OLE32.DLL | COM support |
| OLEAUT32.DLL | OLE automation |
| WS2_32.DLL | Winsock networking |
| WINHTTP.DLL | HTTP client |
| WINMM.DLL | Multimedia |
| IMM32.DLL | Input method |
| D3D11.DLL | Direct3D 11 |
| DXGI.DLL | DXGI |
| OPENGL32.DLL | OpenGL |
| MSVCP140.DLL | MSVC runtime |
| VCRUNTIME140.DLL | MSVC runtime |
| STEAM_API.DLL | Steam integration |
| ZIP.DLL | Archive support |

## SFML Graphics (sf::)

### Shapes and Drawing
```
~VertexArray
draw
RenderStates (multiple overloads)
operator[]
VertexArray
isSmooth
setOutlineColor (multiple overloads)
setOutlineThickness (multiple overloads)
setRadius
CircleShape
getColor
operator==
getFillColor
getOutlineColor
Cyan, Blue, Black, White, Red, Green, Yellow, Magenta, Transparent (Color constants)
getTexture (multiple overloads)
BlendAlpha, BlendMode, BlendNone, BlendMultiply, BlendAdd
Drawable
Vertex
~Transformable, Transformable
~Drawable
~CircleShape
setUniform
setPosition (multiple overloads)
Sprite (multiple overloads)
~Sprite
loadFromFile (multiple overloads)
Image
Text (multiple overloads)
~Text
setFont
setSmooth
getStyle
setTextureRect (multiple overloads)
getGlyph
getKerning
setLineSpacing
setScale (multiple overloads)
getLocalBounds
setOrigin (multiple overloads)
getGlobalBounds (multiple overloads)
Color (multiple overloads)
operator!=
setColor
setRepeated
~Font
Font
isAvailable
~ConvexShape
setPoint
setPointCount
ConvexShape
setString
loadFromMemory
toInteger
getString
setFillColor (multiple overloads)
View
setView
create (multiple overloads)
getSize (multiple overloads)
setTexture (multiple overloads)
getPosition (multiple overloads)
~Texture
Texture
getPixelsPtr
~Image
copyToImage
Default
setSize
RectangleShape
~RectangleShape
display (multiple overloads)
clear
getCharacterSize
setCharacterSize
~RenderTexture
RenderTexture
~RenderWindow
getFont
RenderWindow
setMouseCursorVisible
~Context
Context
```

### Window and Input
```
getSystemHandle
isButtonPressed (Mouse)
setPosition (Mouse/Window)
getPosition (Mouse/Window)
setIcon
setKeyRepeatEnabled
setFramerateLimit
setVerticalSyncEnabled
VideoMode
getDesktopMode
isKeyPressed (Keyboard)
setMouseCursor
loadFromPixels (Cursor)
Cursor
~Cursor
pollEvent
close
isOpen
```

### System
```
operator!= (String)
find
isEmpty
String (multiple overloads)
~String
asMilliseconds
milliseconds
sleep
asSeconds
seconds
Time
Clock
restart
```

### Audio
```
stop (Sound/Music)
setPosition (Sound)
onLoop
onSeek
onGetData
pause (Sound/Music)
~SoundBuffer
~Music
Sound
~Sound
setBuffer
Music
SoundBuffer
openFromMemory
loadFromMemory
setLoop (Sound/Music)
getDuration
play (Sound/Music)
getBuffer
getChannelCount
getStatus (Sound/Music)
setVolume
setPlayingOffset
getPlayingOffset
```

### Network
```
None (Socket::Status)
connect (TcpSocket)
disconnect (TcpSocket)
send
receive
getRemoteAddress
setBlocking
getHandle
~TcpSocket
TcpSocket
IpAddress
```

---

## libzip (ZIP.DLL)

```
zip_stat
zip_close
zip_fread
zip_fclose
zip_stat_init
zip_open
zip_get_name
zip_fopen
zip_get_num_entries
```

---

## Windows API - KERNEL32.DLL

### Process and Thread
```
WaitForSingleObjectEx
WaitForSingleObject
GetCurrentProcess
GetCurrentThreadId
GetCurrentProcessId
GetCurrentThread
IsWow64Process
TerminateProcess
ResumeThread
SuspendThread
GetThreadContext
SetThreadContext
```

### Memory Management
```
HeapSize
HeapReAlloc
HeapAlloc
HeapCompact
HeapDestroy
HeapFree
HeapCreate
HeapValidate
GetProcessHeap
VirtualQuery
VirtualFree
VirtualProtect
VirtualAlloc
GlobalFree
GlobalAlloc
GlobalUnlock
GlobalLock
LocalFree
LocalAlloc
FlushInstructionCache
```

### File Operations
```
CreateFileA
CreateFileW
ReadFile
WriteFile
CloseHandle
GetFileSizeEx
GetFileSize
SetFilePointer
SetFilePointerEx
FlushFileBuffers
SetEndOfFile
GetFileAttributesA
GetFileAttributesW
GetFileAttributesExW
GetFileInformationByHandle
GetFileInformationByHandleEx
SetFileInformationByHandle
GetFullPathNameA
GetFullPathNameW
FindFirstFileA
FindFirstFileW
FindFirstFileExW
FindNextFileA
FindNextFileW
FindClose
DeleteFileA
DeleteFileW
CopyFileW
LockFile
UnlockFile
LockFileEx
UnlockFileEx
FlushViewOfFile
```

### Memory Mapping
```
MapViewOfFile
UnmapViewOfFile
CreateFileMappingW
```

### Directory Operations
```
CreateDirectoryW
GetCurrentDirectoryA
SetCurrentDirectoryA
GetTempPathA
GetTempPathW
```

### Module/Library
```
LoadLibraryA
LoadLibraryW
LoadLibraryExW
FreeLibrary
GetModuleHandleA
GetModuleHandleW
GetModuleFileNameW
GetProcAddress
```

### System Information
```
GetSystemInfo
GetNativeSystemInfo
GlobalMemoryStatusEx
GetComputerNameA
GetComputerNameW
GetSystemTime
GetSystemTimeAsFileTime
GetTickCount
GetTickCount64
QueryPerformanceCounter
GetTimeZoneInformation
GetSystemPowerStatus
K32GetProcessMemoryInfo
```

### Locale
```
GetACP
GetLocaleInfoW
GetLocaleInfoEx
GetUserDefaultLocaleName
GetSystemDefaultLocaleName
```

### Disk
```
GetDiskFreeSpaceA
GetDiskFreeSpaceW
GetDiskFreeSpaceExA
GetDriveTypeA
GetLogicalDrives
GetLogicalProcessorInformationEx
```

### Synchronization
```
InitializeCriticalSection
EnterCriticalSection
LeaveCriticalSection
DeleteCriticalSection
TryEnterCriticalSection
CreateMutexW
InitializeSListHead
AcquireSRWLockExclusive
ReleaseSRWLockExclusive
WakeAllConditionVariable
SleepConditionVariableSRW
InterlockedIncrement
InterlockedDecrement
```

### Error Handling
```
GetLastError
SetLastError
UnhandledExceptionFilter
SetUnhandledExceptionFilter
```

### Environment
```
GetEnvironmentVariableW
ExpandEnvironmentStringsW
GetCommandLineW
GetStartupInfoW
```

### String
```
MultiByteToWideChar
WideCharToMultiByte
lstrcmpW
AreFileApisANSI
```

### INI Files
```
GetPrivateProfileStringA
GetPrivateProfileIntA
WritePrivateProfileStringA
```

### Version
```
VerSetConditionMask
VerifyVersionInfoW
```

### Debug
```
IsDebuggerPresent
OutputDebugStringA
OutputDebugStringW
IsProcessorFeaturePresent
```

### Other
```
Sleep
FormatMessageA
FormatMessageW
GetSystemDirectoryW
SystemTimeToFileTime
```

---

## Windows API - USER32.DLL

```
SetClipboardData
EmptyClipboard
CloseClipboard
GetClipboardData
OpenClipboard
GetKeyboardLayout
ReleaseDC
GetDC
EnumDisplaySettingsW
EnumDisplayMonitors
GetSystemMetrics
MessageBoxA
MessageBoxW
EnumDisplayDevicesW
GetMonitorInfoA
GetMonitorInfoW
MonitorFromWindow
SetWindowPos
GetWindowLongA
SetWindowLongA
CallWindowProcA
GetAsyncKeyState
MapVirtualKeyA
PostMessageA
GetDeviceCaps
```

---

## Windows API - ADVAPI32.DLL

```
GetTokenInformation
OpenProcessToken
GetUserNameA
GetUserNameW
RegOpenKeyExW
RegCloseKey
RegQueryValueExW
RegEnumValueW
```

---

## Windows API - SHELL32.DLL

```
ShellExecuteA
ShellExecuteExA
SHGetFolderPathA
SHGetFolderPathW
```

---

## Windows API - OLE32.DLL / OLEAUT32.DLL

```
CoInitializeEx
CoInitialize
CoUninitialize
CoCreateInstance
CoInitializeSecurity
PropVariantClear
SysAllocString
SysFreeString
VariantClear
VariantInit
```

---

## Windows API - WINHTTP.DLL

```
WinHttpConnect
WinHttpOpenRequest
WinHttpSendRequest
```

---

## C++ Standard Library (MSVCP140.DLL)

### Streams
```
sputc
setstate
basic_ostream<char>
basic_istream<char>
basic_ios<char>
basic_iostream<char>
basic_streambuf<char>
basic_ostream<wchar_t>
basic_ios<wchar_t>
basic_streambuf<wchar_t>
operator<<
setw
put
tellp
getloc
widen
flush
good
sputn
xsputn
xsgetn
_Osfx
sync
_Unlock
_Lock
imbue
setbuf
uflow
showmanyc
snextc
sgetc
sbumpc
_Fiopen (multiple)
_Ipfx
```

### Locale
```
id
_Getcat (multiple)
in
out
unshift
always_noconv
_Init
_Id_cnt
_Getgloballocale
_Lockit
~_Locinfo
_Locinfo
_Makeloc
_Getname
_New_Locimp
```

### Threading
```
_Mtx_lock
_Mtx_unlock
_Thrd_join
_Thrd_id
_Cnd_do_broadcast_at_thread_exit
```

### Exceptions
```
_Xout_of_range
_Xbad_function_call
_Xbad_alloc
_Xruntime_error
_Throw_Cpp_error
_Winerror_map
uncaught_exception
```

### Random
```
_Random_device
_Query_perf_counter
_Query_perf_frequency
```

### Time
```
_Xtime_get_ticks
```

