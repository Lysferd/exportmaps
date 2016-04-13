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


using namespace exportmaps_plugin;
using namespace DFHack;


/*****************************************************************************
 External functions declaration
*****************************************************************************/
extern int  fill_world_region_details  (int world_pos_x, int world_pos_y);


/*****************************************************************************
 Class methods
*****************************************************************************/


/*****************************************************************************
 Main method.
 This is the method that manages all the process.
 It visits all the world coordinates generating the region_details needed
 to get from DF the data needed for each different map.
 That data is put in one queue for each map type to be generated (producer)
 When the world has been visited completely, it puts a special marker in each
 queue to signal the consumers (threads) that there's no data left and that
 they must end its execution
*****************************************************************************/
bool MapsExporter::generate_maps(Logger& logger)
{
  if ((maps_to_generate == 0) && (maps_to_generate_raw == 0))
      return false;   // There's nothing to generate

  this->set_percentage_diplomacy(0);
  this->set_percentage_nobility(0);
  this->set_percentage_sites(0);
  this->set_percentage_trade(0);

  logger.log_line("Starting threads");

  // start the threads, one for each map to generate
  this->setup_threads();

  // Store the world coordinates of the current region, as the region_details are already generated
  std::set<int> current_region_x_coords;
  std::set<int> current_region_y_coords;
  for (unsigned int u = 0; u < df::global::world->world_data->region_details.size(); ++u)
  {
      current_region_x_coords.insert(df::global::world->world_data->region_details[u]->pos.x);
      current_region_y_coords.insert(df::global::world->world_data->region_details[u]->pos.y);
  }

  // No error
  bool exit_by_error = false;

  // Iterate over the whole world
  for (int y = 0; y < df::global::world->world_data->world_height; ++y)
    for (int x = 0; x < df::global::world->world_data->world_width; ++x)
    {
      logger.log("Processing world coordinates:[");logger.log_number(x,3);logger.log(",");logger.log_number(y,3);
      logger.log("]");logger.log_cr();

      df::world_region_details* ptr_rd;
      bool delete_region = true; // New generated region must be deleted after processing it

      // Check if this world coordinate is one from the currente embark
      if (current_region_x_coords.count(x))
      {
        if (current_region_y_coords.count(y))
        {
          for (unsigned int k = 0;
               k < df::global::world->world_data->region_details.size();
               ++k)
                    if (df::global::world->world_data->region_details[k]->pos.x == x)
                      if (df::global::world->world_data->region_details[k]->pos.y == y)
                      {
                        // For the current embark, the region_details are already present
                        // in world.world_data.region_details vector, so there's no need
                        // to generate nor destroy them
                        ptr_rd = df::global::world->world_data->region_details[k];
                        delete_region = false;
                        break;
                      }
        }
      }

      if (delete_region) // for on-demand generated ones
      {
        // size before inserting the new element
        int previous = df::global::world->world_data->region_details.size();

        // generate a new region details a push in the vector
        fill_world_region_details(x,y);

        // Check the new size of the vector
        int new_size = df::global::world->world_data->region_details.size();

        // The new size must be the original + 1. If not there was an error
        if ((new_size == 0) || (new_size == previous))
        {
          delete_region = false; // don't delete anything
          exit_by_error = true;  // finish the plugin execution as there were problems
        }
        else // Everything ok. Use the new generated region details
          ptr_rd = df::global::world->world_data->region_details[new_size-1];
      }

      // Push the data into the different queues
      if (!exit_by_error)
        this->push_data(ptr_rd, x, y);

      // Remove the generated region details
      if (delete_region)
      {
        // delete_world_region_details(ptr_rd);
        delete ptr_rd;

        // Remove also the entry in the vector
        df::global::world->world_data->region_details.erase(df::global::world->world_data->region_details.begin() +
                                                            df::global::world->world_data->region_details.size()-1);
      }
    }

    // The whole world has been swept
    // Signal no more data to the consumer (threads) to finish their execution
    if (!exit_by_error)
    {
      logger.log_endl();
      logger.log_line("World map visited");
    }

    // Signal no more data to the threads
    this->push_end();

    // Trading, diplomacy, nobility and specially sites map can be slow to generate
    // so show a progress counter to inform the user
    this->display_progress_special_maps(&logger);


    // Wait for the consumers to finish
    logger.log_line("Waiting for threads to finish");
    this->wait_for_threads();

    // Write the generated maps to disk
    if (!exit_by_error)
    {
      logger.log_line("Writing maps to disk");
      this->write_maps_to_disk();
    }

    // Free resources
    logger.log_line("Clean up resources");
    this->cleanup();

    if (!exit_by_error)
    {
      logger.log_line("Done.");
    }
    else
      logger.log_line("ERROR generating maps");

    return !exit_by_error;
}

//----------------------------------------------------------------------------//
// Write the generated maps to disk
//----------------------------------------------------------------------------//
void MapsExporter::write_maps_to_disk()
{
  if (maps_to_generate & MapType::TEMPERATURE)
    temperature_map.get()->write_to_disk();

  if (maps_to_generate & MapType::RAINFALL)
    rainfall_map.get()->write_to_disk();

  if (maps_to_generate & MapType::DRAINAGE)
    drainage_map.get()->write_to_disk();

  if (maps_to_generate & MapType::SAVAGERY)
    savagery_map.get()->write_to_disk();

  if (maps_to_generate & MapType::VOLCANISM)
    volcanism_map.get()->write_to_disk();

  if (maps_to_generate & MapType::VEGETATION)
    vegetation_map.get()->write_to_disk();

  if (maps_to_generate & MapType::EVILNESS)
    evilness_map.get()->write_to_disk();

  if (maps_to_generate & MapType::SALINITY)
    salinity_map.get()->write_to_disk();

  if (maps_to_generate & MapType::HYDROSPHERE)
    hydro_map.get()->write_to_disk();

  if (maps_to_generate & MapType::ELEVATION)
    elevation_map.get()->write_to_disk();

  if (maps_to_generate & MapType::ELEVATION_WATER)
    elevation_water_map.get()->write_to_disk();

  if (maps_to_generate & MapType::BIOME)
    biome_map.get()->write_to_disk();

//    if (maps_to_generate & MapType::GEOLOGY)
//        geology_map.get()->write_to_disk();

  if (maps_to_generate & MapType::TRADING)
    trading_map.get()->write_to_disk();

  if (maps_to_generate & MapType::NOBILITY)
    nobility_map.get()->write_to_disk();

  if (maps_to_generate & MapType::DIPLOMACY)
    diplomacy_map.get()->write_to_disk();

  if (maps_to_generate & MapType::SITES)
    sites_map.get()->write_to_disk();

//----------------------------------------------------------------------------//

  if (maps_to_generate_raw & MapTypeRaw::BIOME_TYPE_RAW)
    biome_type_raw_map.get()->write_to_disk();

  if (maps_to_generate_raw & MapTypeRaw::BIOME_REGION_RAW)
    biome_region_raw_map.get()->write_to_disk();

  if (maps_to_generate_raw & MapTypeRaw::DRAINAGE_RAW)
    drainage_raw_map.get()->write_to_disk();
}



//----------------------------------------------------------------------------//
// Methods to return the different maps from its smart pointer
//----------------------------------------------------------------------------//
ExportedMapDF* MapsExporter::get_temperature_map()
{
    return temperature_map.get();
}

ExportedMapDF* MapsExporter::get_rainfall_map()
{
    return rainfall_map.get();
}

ExportedMapBase* MapsExporter::get_drainage_map()
{
    return drainage_map.get();
}

ExportedMapDF* MapsExporter::get_savagery_map()
{
    return savagery_map.get();
}

ExportedMapDF* MapsExporter::get_volcanism_map()
{
    return volcanism_map.get();
}

ExportedMapDF* MapsExporter::get_vegetation_map()
{
    return vegetation_map.get();
}

ExportedMapDF* MapsExporter::get_evilness_map()
{
    return evilness_map.get();
}

ExportedMapDF* MapsExporter::get_salinity_map()
{
    return salinity_map.get();
}

ExportedMapDF* MapsExporter::get_hydro_map()
{
    return hydro_map.get();
}

ExportedMapDF* MapsExporter::get_elevation_map()
{
    return elevation_map.get();
}

ExportedMapDF* MapsExporter::get_elevation_water_map()
{
    return elevation_water_map.get();
}

ExportedMapBase* MapsExporter::get_biome_map()
{
    return biome_map.get();
}

ExportedMapDF* MapsExporter::get_geology_map()
{
    return geology_map.get();
}

ExportedMapDF* MapsExporter::get_trading_map()
{
    return trading_map.get();
}

ExportedMapDF* MapsExporter::get_nobility_map()
{
    return nobility_map.get();
}

ExportedMapDF* MapsExporter::get_diplomacy_map()
{
    return diplomacy_map.get();
}

ExportedMapDF* MapsExporter::get_sites_map()
{
    return sites_map.get();
}


ExportedMapBase* MapsExporter::get_biome_type_raw_map()
{
    return biome_type_raw_map.get();
}

ExportedMapBase* MapsExporter::get_biome_region_raw_map()
{
    return biome_region_raw_map.get();
}

ExportedMapBase* MapsExporter::get_drainage_raw_map()
{
    return drainage_raw_map.get();
}

//----------------------------------------------------------------------------//
// Methods to update the percentage
//----------------------------------------------------------------------------//
void MapsExporter::set_percentage_sites(int percentage)
{
  this->percentage_sites = percentage;
}

void MapsExporter::set_percentage_trade(int percentage)
{
  this->percentage_trade = percentage;
}

void MapsExporter::set_percentage_nobility(int percentage)
{
  this->percentage_nobility = percentage;
}

void MapsExporter::set_percentage_diplomacy(int percentage)
{
  this->percentage_diplomacy = percentage;
}

//----------------------------------------------------------------------------//
// Clean the resources used
//----------------------------------------------------------------------------//

void MapsExporter::cleanup()
{
    // Empty data queues if not already done
    while (!temperature_queue.empty())      temperature_queue.pop();
    while (!rainfall_queue.empty())         rainfall_queue.pop();
    while (!drainage_queue.empty())         drainage_queue.pop();
    while (!savagery_queue.empty())         savagery_queue.pop();
    while (!volcanism_queue.empty())        volcanism_queue.pop();
    while (!vegetation_queue.empty())       vegetation_queue.pop();
    while (!evilness_queue.empty())         evilness_queue.pop();
    while (!salinity_queue.empty())         salinity_queue.pop();
    while (!hydro_queue.empty())            hydro_queue.pop();
    while (!elevation_queue.empty())        elevation_queue.pop();
    while (!elevation_water_queue.empty())  elevation_water_queue.pop();
    while (!biome_queue.empty())            biome_queue.pop();
    while (!geology_queue.empty())          geology_queue.pop();
    while (!trading_queue.empty())          trading_queue.pop();
    while (!nobility_queue.empty())         nobility_queue.pop();
    while (!diplomacy_queue.empty())        diplomacy_queue.pop();
    while (!sites_queue.empty())            sites_queue.pop();

    while (!biome_raw_type_queue.empty())   biome_raw_type_queue.pop();
    while (!biome_raw_region_queue.empty()) biome_raw_region_queue.pop();
    while (!drainage_raw_queue.empty())     drainage_raw_queue.pop();

    // Destroy the generated maps
    temperature_map.reset();
    rainfall_map.reset();
    drainage_map.reset();
    savagery_map.reset();
    volcanism_map.reset();
    vegetation_map.reset();
    evilness_map.reset();
    salinity_map.reset();
    hydro_map.reset();
    elevation_map.reset();
    elevation_water_map.reset();
    biome_map.reset();
    geology_map.reset();
    trading_map.reset();
    nobility_map.reset();
    diplomacy_map.reset();
    sites_map.reset();

    biome_type_raw_map.reset();
    biome_region_raw_map.reset();
    drainage_raw_map.reset();

    // Destroy the generated producers
    temperature_producer.reset();
    rainfall_producer.reset();
    drainage_producer.reset();
    savagery_producer.reset();
    volcanism_producer.reset();
    vegetation_producer.reset();
    evilness_producer.reset();
    salinity_producer.reset();
    hydro_producer.reset();
    elevation_producer.reset();
    elevation_water_producer.reset();
    biome_producer.reset();
    geology_producer.reset();
    trading_producer.reset();
    nobility_producer.reset();    
    diplomacy_producer.reset();
    sites_producer.reset();

    biome_type_raw_producer.reset();
    biome_region_raw_producer.reset();
    drainage_raw_producer.reset();
}

//----------------------------------------------------------------------------//
// Display a progress counter in the console for the slowest maps to generate:
// nobility, diplomacy, trading and specially sites map
//----------------------------------------------------------------------------//
void MapsExporter::display_progress_special_maps(Logger* logger)
{
  if ((maps_to_generate & MapType::TRADING) ||
      (maps_to_generate & MapType::NOBILITY) ||
      (maps_to_generate & MapType::DIPLOMACY) ||
      (maps_to_generate & MapType::SITES)
     )
  {
    while (((this->percentage_trade     != -1) && (maps_to_generate & MapType::TRADING))   ||
           ((this->percentage_sites     != -1) && (maps_to_generate & MapType::SITES))     ||
           ((this->percentage_diplomacy != -1) && (maps_to_generate & MapType::DIPLOMACY)) ||
           ((this->percentage_nobility  != -1) && (maps_to_generate & MapType::NOBILITY))
          )
    {
      if (maps_to_generate & MapType::TRADING)
      {
        if (this->percentage_trade == -1)
        {
          logger->log("Trading map:100% - ");
        }
        else
        {
          logger->log("Trading map:");
          logger->log_number(this->percentage_trade,3);
          logger->log("% - ");
        }
      }

      if (maps_to_generate & MapType::NOBILITY)
      {
        if (this->percentage_nobility == -1)
        {
          logger->log("Nobility map:100% - ");
        }
        else
        {
          logger->log("Nobility map:");
          logger->log_number(this->percentage_nobility,3);
          logger->log("% - ");
        }
      }

      if (maps_to_generate & MapType::SITES)
      {
        if (this->percentage_sites == -1)
        {
          logger->log("Sites map:100% - ");
        }
        else
        {
          logger->log("Sites map:");
          logger->log_number(this->percentage_sites,3);
          logger->log("% - ");
        }
      }

      if (maps_to_generate & MapType::DIPLOMACY)
      {
        if (this->percentage_diplomacy == -1)
        {
          logger->log("Diplomacy map:100% - ");
        }
        else
        {
          logger->log("Diplomacy map:");
          logger->log_number(this->percentage_diplomacy,3);
          logger->log("% - ");
        }
      }

      logger->log_cr();
    }
    logger->log_endl();
  }

}
