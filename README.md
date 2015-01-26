# lib50s

This is a very rough prototype of a graphics engine intended for a 1950's themed zombie game.
Development of this game, built on the open-source Irrlicht library, has unfortunately stopped for now.
However, I plan to continue to work on it when I have time, and the source code is being made available to anyone who wishes to contribute and/or use for their personal projects.

More information about this code: http://irrlicht.sourceforge.net/forum/viewtopic.php?f=6&t=49713&p=287989

Game development website: http://50sgame.com/

# Dependencies
Boost v1.53 - http://sourceforge.net/projects/boost/files/boost/1.53.0/

Bullet Physics v2.81 - http://code.google.com/p/bullet/downloads/detail?name=bullet-2.81-rev2613.zip&can=2&q=

Irrlicht SDK v1.8.1 - https://sourceforge.net/projects/irrlicht/files/Irrlicht%20SDK/1.8/1.8.1/irrlicht-1.8.1.zip/download

Lua 5.2.1 - http://softlayer-dal.dl.sourceforge.net/project/luabinaries/5.2.1/Windows%20Libraries/Dynamic/lua-5.2.1_Win32_dll11_lib.zip

# Build
This code is built using Visual Studio 2012. The output directory is set up to be ../bin/ relative to the repo location. Copy the Irrlicht and Lua DLLs into the bin/Debug/ directory.

You also need to build Bullet. To do this, go to the Bullet directory, build/vs2010. Open the vs2010 solution file, and have vs2012 upgrade it for you, then just press f7.

# Assets
Download the assets from the following link, and put them under the client/data/ folder (so it should be client/data/assets/...).

https://drive.google.com/folderview?id=0BzUVY5zj_6bkYzlmZGdDZVJkNW8&usp=sharing

Let me know if you would like to have edit permission.
