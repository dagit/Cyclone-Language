/* Allocation profiler
   Copyright (C) 2001 Greg Morrisett, AT&T
   This file is part of the Cyclone compiler.

   The Cyclone compiler is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The Cyclone compiler is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the Cyclone compiler; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */

#ifndef __APROF_H__
#define __APROF_H__

// A cute hack to avoid defining datatypes twice
#ifdef APROF_LIB
#define EXTERN_APROF
#else
#define EXTERN_APROF extern
#endif

#include <core.h>
#include <fn.h>

namespace Aprof {
  using Core;
  using Fn;

  // Definitions of profiler events
  struct RegionStats {
    int region_size_bytes;
    int free_bytes;
    int total_alloc_bytes;
  };
  struct AllocEvent {
    string_t location;
    string_t region_name;
    int amount;
    struct RegionStats region_stats;
  };
  struct GcEvent {
    unsigned int num;
    struct RegionStats heap_region_stats;
  };
  struct CreateEvent {
    string_t location;
    string_t region_name;
    struct RegionStats heap_region_stats;
  };
  struct GrowEvent {
    string_t region_name;
    int amount;
    struct RegionStats heap_region_stats;
  };
  struct ShrinkEvent {
    string_t region_name;
    int amount;
    struct RegionStats heap_region_stats;
  };
  struct FreeEvent {
    string_t region_name;
    struct RegionStats heap_region_stats;
  };
  typedef struct AllocEvent alloc_event_t;
  typedef struct GcEvent gc_event_t;
  typedef struct CreateEvent create_event_t;
  typedef struct GrowEvent grow_event_t;
  typedef struct ShrinkEvent shrink_event_t;
  typedef struct FreeEvent free_event_t;
  
  // The main event definition
  typedef datatype AProfEvent @aprof_event_t;
  EXTERN_APROF datatype AProfEvent {
    AllocE(alloc_event_t);
    GcE(gc_event_t);
    CreateE(create_event_t);
    GrowE(grow_event_t);
    ShrinkE(shrink_event_t);
    FreeE(free_event_t);
  };
  
  // For processing profiling files.  Callback will return false
  //   if something goes wrong.
  extern int proc_file(string_t file, fn_t<aprof_event_t,bool,`H> callback);
}

#endif
