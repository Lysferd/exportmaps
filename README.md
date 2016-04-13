# **exportmaps**
A DFHack plugin for exporting world maps while in game.

## What do it does?
It exports all the world maps while in game. There's no need to enter Legends mode.

The following maps can be generated:
* Elevations including lake and ocean floors
* Elevation respecting water level
* Biome
* Hydrosphere
* Temperature
* Rainfall
* Drainage
* Savagery
* Volcanism
* Current Vegetation
* Evil
* Salinity
* Structures/fields/roads/etc
* Trade
* Nobility/holdings
* Diplomacy

Only enhanced biome map is not generated, as I consider it confusing and useless.

## Is it fast?
Very,very fast. Exportmaps uses a thread for each map to be generated, so it uses all the cores of your computer for generating them.
In contrast, DF only allows you to generate them one map each time.

Sites/Structures/Roads is, by far, the slowest map to generate as a full realization of each world site needs to be generated, added to the map and destroyed.

## Is it multiplatform?

Unfortunately no. This plugin requires some trickery to get the data natively from DF. As I don't have access to a Mac computer, MacOS is not supported.
The plugin is Linux and Windows only.


## But there's already a exportlegends script, isn't?
Yes. You can use `exportlegends maps` to generate the maps, but:

* You need to exit your game and enter legends mode
* It only simulates the keypresses needed to tell DF to generate the map. You're single core again.

## Are the exported maps identical to the ones generated by DF?
Yes. The only difference is that the exported map are generated in .PNG format instead of .BMP. 

Below is a sites/structure map comparison between the map generated by DF and the one generated by the plugin.

![alt tag](https://github.com/ragundo/exportmaps/blob/master/docs/xites.png)

## How do I use it?
For the impatient, load a game and type in the DFHack console:

`exportmaps -all`

This will generate all the maps and write them to disk. This is the prefered way to do it as it's more efficient than generating one by one.

If you want to generate only a specific map, you can provide a command line option:

`exportmaps -biome`


will generate only the biome map. You can also chain options, so

`exportmaps -biome -vegetation`

will generate the biome and the vegetation maps.

The availble options are the following:

| Command | Map generated |
| --- | --- |
| -temperature     | TEMPERATURE |
| -rainfall        | RAINFALL |
| -drainage        | DRAINAGE |
| -savagery        | SAVAGERY |
| -volcanism       | VOLCANISM |                           
| -vegetation      | VEGETATION |
| -evilness        | EVILNESS |
| -salinity        | SALINITY |
| -hydrosphere     | HYDROSPHERE |
| -elevation       | ELEVATION |
| -elevation-water | ELEVATION RESPECTING WATER |
| -biome           | BIOME |
| -trading         | TRADING |                  
| -nobility        | NOBILITY AND HOLDINGS |
| -diplomacy       | DIPLOMACY |                                
| -sites           | SITES / STRUCTURES / ROADS |

## Raw maps
Raw maps are binary files with data extracted directly from DF.
[Read more details about them.](https://github.com/ragundo/exportmaps/dock/raw_maps.md)


## What's next?
For next releases, this is what's planned:

#### RPC interface
For generating maps on demand without needing to use the DFHack console. Just connect to DF and request a map using Google's protocol buffers.

#### Site exporting
Similar to DF export site. Get a detailed PNG of a world site.

## Support
You can use Github's Issues to get support about the plugin. Also you can contact me in #dfhack on IRC freenode.

## Thanks to
Warmist, Lethosor, Japa, Q, Angavrilov and all the wonderful people of #dfhack in freenode.
Without their support, this plugin will never have been developed!!!.
