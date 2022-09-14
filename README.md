## Mashup Dimensions gamecode (игровой модуль)

## Description ##
Based on Open Arena gamecode, heavily modified gamecode aims to extend singleplayer creativity, multi-language support (string tables), as well as animation and gameplay flow improvements.
- RU and EN language included (multi-language support)
- Language-dependent content support (sounds and textures)
- Gauntlet block feature
- Zoom reticle feature
- Weapon placement (center,off,left,right) setting
- Third person unlocked and tweaked
- Randomized jump and land multi-sound
- Damage plums instead of blodspurts
- Jpeg sequences ui player
- Collaborative ai mode (enemy collaboration)
- missions support (multi-mission with briefing screen above scores TAB screen)
- coop mode support (TDM-based)
- func_button has destroyable object simulation mode
- Culled maximum ammo loadout
- Flight item improvement
- Ingame picture show feature via target_print
- Ingame console command executing via target_relay
- Silent NPC relocation on demand via target_relay
- Switchable cameras feature via target_print
- Definition of bot characteristics (botname_c.c) file during spawn (for example, console command addbot Xaero/Crash_c adds Xaero with Crash characteristics)
- Heavy integration with Trenchbroom level editor
- Player-model-gender-sensitive ingame jpeg sequence playback feature via target_print (see detailed description in cgame/cg_draw.c)


## Building ##
WIndows only
Use included .cmd files

## License ##

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
