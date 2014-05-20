destruct-o
==========

A fully multiplatform Voxel Based Game with network sync built on SDL


## Summary ##
Destructo is a procedurally generated tribute to 90s games like duke nukem and quake. The player will be dropped into a strange world, using a method similar to the "ACID Shader" http://www.youtube.com/watch?v=JpksyojwqzE. Similar to the 90s era games, the player will be an overly macho guy with an alcohol dependency. If he doesn't keep drinking his vision gets worse and worse utilizing the "ACID Shader" we'll be writing. You can find source code for this shader here: http://pastebin.com/WbVNtMg7 which will be applied as a post effect to the rendered world. 


## World Generation ##
The world will be built as a set of procedurally generated tiles of around 2000x2000 voxels that when combined together to create the world. The main function behind this will be the city planner, which will try to create a coherent pattern of city / rural / desert / ocean / etc. It will first plan out the major land areas using some simple rules such as there should be a large body of water this large, rivers and roads should be drawn along vector paths randomly generated in the beginning. Roads passing through rivers should be built as bridges, buildings should be grouped together and cannot intersect, but must be placed along roads.

## Device Sync ##
Save / Load will be an interesting feat as the world is destructible. There have been a couple of suggestions such as saving only a diff and creating a deterministic process for city planning where the world can be rebuilt from a seed. Therefore the world can be regenerated from very little data. 

A Server written in Go will handle the Syncing of these saves across devices as well as user profiles as we'd like you to be able to run it on your mac, pc, android and iphone and all the games to be in sync. The engine must be able to interpolate to create better looking graphics from the base voxels, that way it's not device dependent; however, we will not be supporting older devices. 

## The Engine ##
It's built with voxel based graphics on top of a custom built game engine in C++. We could use UNITY for this task, but we'd rather build it ourselves to challenge our skill set. 

Engine Features

* Save / Load
* Destruction (Voxel)
* Sound
* Voxel Based Graphics
* FBX Loaded Graphics
* Texture Loading (compressed)
* Run On Mobile
* Input
* NPC / AI
* Physics
* Dialog System


## Main Concept ##
* Title: Destructo
* Description: Drunk Duke Nukem
* Summary: Run around, blow things up, be drunk
* Start Sequence: He's blackout drunk and hallucinating, opening seen is of him lying on the floor as he passes out. He wakes up in the world, etc. 

## Engine Features ##
* Save / Load
* Destruction (Voxel)
* Sound
* Voxel Based Graphics
* FBX Loaded Graphics
* Texture Loading (compressed)
* Run On Mobile
* Input
* NPC / AI
* Physics
* Dialog System

## World ##
* Procedural City / Rural Area
* Seed Based
* Destruction
* Voxel Based


## Music ##
https://www.facebook.com/SAVOY/app_212097992149339

## Generation ##

### Images ###
* Height Map, two bytes height, two bytes terrain type RGBA 
* Object Map, four bytes object id

### Object Format ###
* OBJF, 4 byte signature on the front of the file
* Version Number 4 byte unsigned int
* Voxels: 4 float position, 4 byte material id 

## Game Progression ##
* Weapon Unlocks
* You're Being Chased By A Really Fucking Badass Thing That's Slow, it can warp if the player gets too far
* If you don't keep drinking, [http://www.youtube.com/watch?v=JpksyojwqzE](http://www.youtube.com/watch?v=JpksyojwqzE) happens, drinking sobers you up somehow

## Items ##
Fuck this idea, the above one under game Progression is better