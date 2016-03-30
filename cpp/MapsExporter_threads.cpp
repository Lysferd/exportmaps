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

#include <set>
#include <modules/World.h>
#include "../include/MapsExporter.h"
#include "../include/RegionDetails.h"
#include "../include/ExportedMap.h"

using namespace exportmaps_plugin;


/*****************************************************************************
 External functions declaration
 Here comes all the threads functions
*****************************************************************************/
extern void consumer_temperature    (void* arg);
extern void consumer_rainfall       (void* arg);
extern void consumer_drainage       (void* arg);
extern void consumer_savagery       (void* arg);
extern void consumer_volcanism      (void* arg);
extern void consumer_vegetation     (void* arg);
extern void consumer_evilness       (void* arg);
extern void consumer_salinity       (void* arg);
extern void consumer_hydro          (void* arg);
extern void consumer_elevation      (void* arg);
extern void consumer_elevation_water(void* arg);
extern void consumer_biome          (void* arg);
extern void consumer_geology        (void* arg);
extern void consumer_trading        (void* arg);
extern void consumer_nobility       (void* arg);
extern void consumer_diplomacy      (void* arg);
extern void consumer_sites          (void* arg);

/*****************************************************************************
*****************************************************************************/


tthread::mutex& MapsExporter::get_mutex()
{
    return mtx;
}


void MapsExporter::setup_threads()
{
    // Start a thread for each map type and put in a list
    // in order to wait later for finishing them
    if (maps_to_generate & MapType::TEMPERATURE)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_temperature,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::RAINFALL)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_rainfall,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::DRAINAGE)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_drainage,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::SAVAGERY)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_savagery,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::VOLCANISM)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_volcanism,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::VEGETATION)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_vegetation,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::EVILNESS)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_evilness,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::SALINITY)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_salinity,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::HYDROSPHERE)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_hydro,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::ELEVATION)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_elevation,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::ELEVATION_WATER)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_elevation_water,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::BIOME)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_biome,(void*)this);
        consumer_threads.push_back(pthread);
    }

/*
    if (maps_to_generate & MapType::GEOLOGY)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_geology,(void*)this);
        consumer_threads.push_back(pthread);
    }
*/

    if (maps_to_generate & MapType::TRADING)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_trading,(void*)this);
        consumer_threads.push_back(pthread);
    }    

    if (maps_to_generate & MapType::NOBILITY)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_nobility,(void*)this);
        consumer_threads.push_back(pthread);
    }        

    if (maps_to_generate & MapType::DIPLOMACY)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_diplomacy,(void*)this);
        consumer_threads.push_back(pthread);
    }

    if (maps_to_generate & MapType::SITES)
    {
        tthread::thread* pthread =  new tthread::thread(consumer_sites,(void*)this);
        consumer_threads.push_back(pthread);
    }

}

/*****************************************************************************
*****************************************************************************/

void MapsExporter::wait_for_threads()
{
    for(std::list<tthread::thread *>::iterator i = consumer_threads.begin(); i != consumer_threads.end(); ++i)
    {
      tthread::thread* t = *i;
      if (t!= nullptr)
      {
          // Wait for the thread to finish
          t->join();
          delete t;
          // No need to check this thread anymore
          *i = nullptr;
      }
    }
}
