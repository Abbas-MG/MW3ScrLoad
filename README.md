
# MW3ScrLoad

MW3ScrLoad is a bytecode injector for Call of Duty: Modern Warfare 3 (2011) single player. It is designed to handle .gscbin files created with [gsc-tool](https://github.com/xensik/gsc-tool). It can log various information and can also dump game's script files; you can then decompile and analyze them to study GSC capabilities.

There are similar and better projects too, and this project is the outcome of my curiosity about how the engine works and how the bytecodes are handled.


## Features

- Inject bytecode
- Log some game info
- Dump game's bytecode files
- Run command strings


## Build

To build this project, you need to install:

[PolyHook_2_0](https://github.com/stevemk14ebr/PolyHook_2_0); easier with vcpkg.

[zlib](https://github.com/madler/zlib); easier with vcpkg.

[mINI](https://github.com/metayeti/mINI); simply place it in the `.\include` dir of the project.





## Usage
Place the `MW3ScrLoad.dll` in the root directory of the game, where the iw5sp.exe is located, and then use an injector to inject this DLL into the game.

Upon loading, a file named `config.ini` will be created:

| Item             | Explanation                                                                |
| ----------------- | ------------------------------------------------------------------ |
| [script] ||
| gscBinDir | dir that gscbin files are to be loaded from |
| pseudoFFName | custom gscbin files will be packed into it |
|ffFilesLoad| name of ff files to load|
|[dump]||
|rawfiles| dump raw files?|
|scriptfiles| dump script files?|
|Dir|dump location|
|[log]||
| scriptfiles | log raw files? |
|AssetFiles|log all asset files?|
| misclog | any other logs |
|Dir| log location|


The most important one is `gscBinDir`; place any file you want to load into the game in it.

### Mod Menu
I also wrote a GSC script that gives you a ModMenu in `Special Ops` mode; it will crash the game in story mode as needed files are not linked there.

The menu was created by a guy named P!X, and I added my code to it.

Place the `MW3ScrLoad\_misc\ModMenu\ModMenu.gscbin` in the `.\_LoadGscBin` (default if not changed) folder. Structure must be:
```
C:\CoD MW3\_LoadGscBin\ModMenu.gscbin
```

To open the menu, press `aim + melee` buttons. Then press `aim` to go down, `shoot` to go up, `use` to select an option, and `melee` to go back.

Here is the list of what I added to the menu:
```
Call for Backup
    Call Delta
    Call GIGN
    Call Juggernaut
    Call Full Juggernaut
    Call Chopper
    Dismiss Called Choppers

Helper
    GodMode (Proper)
    Give XP
    Kill All Enemies
    Kill Everyone
    Befriend All Enemies
    All Armories Usable
    Print All Enemies

AirSupport
    Give AGM
    Give 40mm AC130

Equipments
Give Money
Give Sentry
Give Sentry_gl
Give Armor
Give Juggernaut Suit
Give Laststand
Give Claymore
Give C4

Chaos Items
Give Time
Give Bonus Multiplier
Give Laststand
Give Combo Freeze
Give Sentry

All Perks (stacked)
```


### Command Execute
A simple console is implemented to issue commands.

#### `ExecCommand`
If you want to execute commands from the console:
```bash
  ExecCommand "execnow" "set cl_paused 1"
```
Or chain several commands:
```bash
  ExecCommand "execnow" "set cl_paused 1" ; "setdvar" "jump_height" "200.0" ; "setdvar" "g_ai" 0
```

#### `ExecCommandFile`
You can also execute commands from a file:
```bash
  ExecCommandFile .\cmd.txt
```
See `MW3ScrLoad\_misc\cmd.txt` as an example. Here is a list of commands that each have their own formatting, and I didn't try all:
```
"fadeout"
"show"
"hide"
"showMenu"
"hideMenu"
"setcolor"
"open"
"close"
"forceClose"
"escape"
"closeForAllPlayers"
"ingameopen"
"ingameclose"
"setbackground"
"setitemcolor"
"focusfirst"
"setfocus"
"setfocusbydvar"
"setdvar"
"exec"
"execfirstclient"
"execnow"
"execkeypress"
"execOnDvarStringValue"
"execOnDvarIntValue"
"execOnDvarFloatValue"
"execNowOnDvarStringValue"
"execNowOnDvarIntValue"
"execNowOnDvarFloatValue"
"play"
"scriptmenuresponse"
"scriptMenuRespondOnDvarStringValue"
"scriptMenuRespondOnDvarIntValue"
"scriptMenuRespondOnDvarFloatValue"
"setPlayerData"
"setPlayerDataSplitScreen"
"resetStatsConfirm"
"resetStatsCancel"
"setGameMode"
"deleteEliteCacFile"
"integrateEliteCacFile"
"setMatchRulesData"
"setLocalVarBool"
"setLocalVarInt"
"setLocalVarFloat"
"setLocalVarString"
"feederTop"
"feederBottom"
"feederPageUp"
"feederPageDown"
"showGamerCard"
"showCoopGamerCard"
"showSplitscreenGamerCard"
"reportPlayerOffensive"
"reportPlayerExploiting"
"reportPlayerCheating"
"reportPlayerBoosting"
"setCardIcon"
"setCardTitle"
"setCardIconNew"
"setCardTitleNew"
"setCardIconSplitScreen"
"setCardTitleSplitScreen"
"setCardIconNewSplitScreen"
"setCardTitleNewSplitScreen"
"purchasePrestigeTitle"
"setProfileItemNew"
"setProfileItemNewSplitScreen"
"savegamehide"
"savegameshow"
"savegamesetlocalbool"
"saveDelay"
"writeSave"
"setSaveExecOnSuccess"
"nextlevel"
"disablePause"
"enablePause"
"resolveError"
"runCompletionResolve"
"clearCompletionResolve"
"execWithResolve"
"lerp"
"playMenuVideo"
"setBackgroundVideo"
"clearEntitlementNew"
"setPastTitleRank"
"setPastTitlePrestige"
```

## Notes
- Research and tests were done on v1.9.461 of the game.
- Some logging depends on the stack frame, and it may not work on all versions.
- DLL must be in the game's root folder.
- `pseudoFFName` will crash the game with big FF files.
- You can use `MW3ScrLoad\_misc\gsc-tool_loop.bat` to mass decompile the dumped gscbin files; edit the vars within the bat file.
- The debug version of the game has .ffdm files. You can use `MW3ScrLoad\_misc\dbgFF_GSC_extract.py` to extract .gsc files from them and study them; set the `directory_to_search` var in the script.
- Use `decompress.py` to decompress `buffer` sections of gscbin.
- See `MW3ScrLoad\_misc\IDA_py_scripts` for Python scripts that you can use in IDA for logging or calling game functions in real time using appcall.
