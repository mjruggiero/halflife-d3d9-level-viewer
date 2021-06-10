# Half-Life Direct3D 9 Level Viewer

A viewer for levels from the game Half-Life.

This viewer is written in C++ and renders the levels using Direct3D 9.

It includes the following features:
- BSP rendering using PVS
- Animated Textures
- Alpha Blending
- Lightmaps

This will compile with Visual Studio 2019.
You first need to install the DX9 SDK (June 2010).
In order to change the levels, modify the config.txt file.
The first line should point to the halflife.wad file.
The second line is not used.
The third line should points the the map (.bsp) file you want to view.
