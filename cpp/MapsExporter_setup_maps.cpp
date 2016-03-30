/* zlib.h -- interface of the 'zlib' general purpose compression library
  version 1.2.2, October 3rd, 2004

  Copyright (C) 1995-2004 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly jloup@gzip.org
  Mark Adler madler@alumni.caltech.edu

*/

// You can always find the latest version of this plugin in Github
// https://github.com/ragundo/exportmaps  

#include <set>
#include <modules/World.h>
#include "../include/MapsExporter.h"
#include "../include/RegionDetails.h"
#include "../include/ExportedMap.h"

using namespace DFHack;
using namespace exportmaps_plugin;

/*****************************************************************************
*****************************************************************************/

void MapsExporter::setup_maps(uint32_t maps)
{
    // Copy the data received from DFHack command line
    maps_to_generate = maps;

    // Get the date elements
    int year  = World::ReadCurrentYear();
    int month = World::ReadCurrentMonth();
    int day   = World::ReadCurrentDay();


    // Compose date
    std::stringstream ss_date;
    ss_date << "-" << year << "-" << month << "-" << day;
    std::string current_date = ss_date.str();
    std::string region_name = World::ReadWorldFolder();

    if (maps_to_generate & MapType::TEMPERATURE)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-temperature.png";

        temperature_producer.reset(new ProducerTemperature);
        if (!temperature_producer) throw std::bad_alloc();

        temperature_map.reset(new ExportedMapDF(file_name.str(),
                                                df::global::world->world_data->world_width,
                                                df::global::world->world_data->world_height,
                                                MapType::TEMPERATURE));
        if (!temperature_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::RAINFALL)
    {
        // Compose filename
        std::stringstream file_name;

        file_name << region_name << current_date << "-rainfall.png";
        rainfall_producer.reset(new ProducerRainfall);
        if (!rainfall_producer) throw std::bad_alloc();

        rainfall_map.reset(new ExportedMapDF(file_name.str(),
                                             df::global::world->world_data->world_width,
                                             df::global::world->world_data->world_height,
                                             MapType::RAINFALL));
        if (!rainfall_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::DRAINAGE)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-drainage.png";

        drainage_producer.reset(new ProducerDrainage);
        if (!drainage_producer) throw std::bad_alloc();

        drainage_map.reset(new ExportedMapDF(file_name.str(),
                                             df::global::world->world_data->world_width,
                                             df::global::world->world_data->world_height,
                                             MapType::DRAINAGE));
        if (!drainage_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::SAVAGERY)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-savagery.png";

        savagery_producer.reset(new ProducerSavagery);
        if (!savagery_producer) throw std::bad_alloc();

        savagery_map.reset(new ExportedMapDF(file_name.str(),
                                             df::global::world->world_data->world_width,
                                             df::global::world->world_data->world_height,
                                             MapType::SAVAGERY));
        if (!savagery_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::VOLCANISM)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-volcanism.png";

        volcanism_producer.reset(new ProducerVolcanism);
        if (!volcanism_producer) throw std::bad_alloc();

        volcanism_map.reset(new ExportedMapDF(file_name.str(),
                        df::global::world->world_data->world_width,
                        df::global::world->world_data->world_height,
						MapType::VOLCANISM));
        if (!volcanism_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::VEGETATION)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-vegetation.png";
        vegetation_producer.reset(new ProducerVegetation);
        if (!vegetation_producer) throw std::bad_alloc();

        vegetation_map.reset(new ExportedMapDF(file_name.str(),
                        df::global::world->world_data->world_width,
                        df::global::world->world_data->world_height,
						MapType::VEGETATION));
        if (!vegetation_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::EVILNESS)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-evilness.png";

        evilness_producer.reset(new ProducerEvilness);
        if (!evilness_producer) throw std::bad_alloc();

        evilness_map.reset(new ExportedMapDF(file_name.str(),
                        df::global::world->world_data->world_width,
                        df::global::world->world_data->world_height,
						MapType::EVILNESS));
        if (!evilness_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::SALINITY)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-salinity.png";

        salinity_producer.reset(new ProducerSalinity);
        if (!salinity_producer) throw std::bad_alloc();

        salinity_map.reset(new ExportedMapDF(file_name.str(),
                        df::global::world->world_data->world_width,
                        df::global::world->world_data->world_height,
						MapType::SALINITY));
        if (!salinity_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::HYDROSPHERE)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-hydrology.png";

        hydro_producer.reset(new ProducerHydro);
        if (!hydro_producer) throw std::bad_alloc();

        hydro_map.reset(new ExportedMapDF(file_name.str(),
                                        df::global::world->world_data->world_width,
                                        df::global::world->world_data->world_height,
                                        MapType::HYDROSPHERE));
        if (!hydro_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::ELEVATION)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-elevation.png";
        elevation_producer.reset(new ProducerElevation);
        if (!elevation_producer) throw std::bad_alloc();

        elevation_map.reset(new ExportedMapDF(file_name.str(),
                            df::global::world->world_data->world_width,
                            df::global::world->world_data->world_height,
                            MapType::ELEVATION));
        if (!elevation_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::ELEVATION_WATER)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-elevation-water.png";
        elevation_water_producer.reset(new ProducerElevationWater);
        if (!elevation_water_producer) throw std::bad_alloc();

        elevation_water_map.reset(new ExportedMapDF(file_name.str(),
                                                    df::global::world->world_data->world_width,
                                                    df::global::world->world_data->world_height,
                                                    MapType::ELEVATION_WATER));
        if (!elevation_water_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::BIOME)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-biome.png";
        biome_producer.reset(new ProducerBiome);
        if (!biome_producer) throw std::bad_alloc();

        biome_map.reset(new ExportedMapDF(file_name.str(),
                                          df::global::world->world_data->world_width,
                                          df::global::world->world_data->world_height,
                                          MapType::BIOME));
        if (!biome_map) throw std::bad_alloc();
    }

/*
    if (maps_to_generate & MapType::GEOLOGY)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-geology.png";
        geology_producer.reset(new ProducerGeology);
        if (!geology_producer) throw std::bad_alloc();

        geology_map.reset(new ExportedMapDF(file_name.str(),
                                            df::global::world->world_data->world_width,
                                            df::global::world->world_data->world_height,
                                            MapType::GEOLOGY));
        if (!geology_map) throw std::bad_alloc();
    }
*/

    if (maps_to_generate & MapType::TRADING)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-trading.png";
        trading_producer.reset(new ProducerTrading);
        if (!trading_producer) throw std::bad_alloc();

        trading_map.reset(new ExportedMapDF(file_name.str(),
                                            df::global::world->world_data->world_width,
                                            df::global::world->world_data->world_height,
                                            MapType::TRADING));
        if (!trading_map) throw std::bad_alloc();
    }    

    if (maps_to_generate & MapType::NOBILITY)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-nobility.png";
        nobility_producer.reset(new ProducerNobility);
        if (!nobility_producer) throw std::bad_alloc();

        nobility_map.reset(new ExportedMapDF(file_name.str(),
                                            df::global::world->world_data->world_width,
                                            df::global::world->world_data->world_height,
                                            MapType::NOBILITY));
        if (!nobility_map) throw std::bad_alloc();
    }        

    if (maps_to_generate & MapType::DIPLOMACY)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-diplomacy.png";
        diplomacy_producer.reset(new ProducerDiplomacy);
        if (!diplomacy_producer) throw std::bad_alloc();

        diplomacy_map.reset(new ExportedMapDF(file_name.str(),
                                              df::global::world->world_data->world_width,
                                              df::global::world->world_data->world_height,
                                              MapType::DIPLOMACY));
        if (!diplomacy_map) throw std::bad_alloc();
    }

    if (maps_to_generate & MapType::SITES)
    {
        // Compose filename
        std::stringstream file_name;
        file_name << region_name << current_date << "-sites-structures.png";
        sites_producer.reset(new ProducerSites);
        if (!sites_producer) throw std::bad_alloc();

        sites_map.reset(new ExportedMapDF(file_name.str(),
                                          df::global::world->world_data->world_width,
                                          df::global::world->world_data->world_height,
                                          MapType::SITES));
        if (!sites_map) throw std::bad_alloc();
    }
}

