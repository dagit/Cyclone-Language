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
#include <time.h>

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
    clock_t time;
    string_t location;
    string_t region_name;
    int amount;
    struct RegionStats region_stats;
    unsigned int address;
  };
  struct GcEvent {
    clock_t time;
    unsigned int num;
    struct RegionStats heap_region_stats;
  };
  struct ReclaimEvent {
    clock_t time;
    unsigned int address;
  };
  struct CreateEvent {
    clock_t time;
    string_t location;
    string_t region_name;
    struct RegionStats heap_region_stats;
  };
  struct ResizeEvent {
    clock_t time;
    string_t region_name;
    int amount;
    struct RegionStats heap_region_stats;
  };
  struct FreeEvent {
    clock_t time;
    string_t region_name;
    struct RegionStats heap_region_stats;
  };
  typedef struct AllocEvent alloc_event_t;
  typedef struct GcEvent gc_event_t;
  typedef struct ReclaimEvent reclaim_event_t;
  typedef struct CreateEvent create_event_t;
  typedef struct ResizeEvent resize_event_t;
  typedef struct FreeEvent free_event_t;
  
  // The main event definition
  typedef datatype AProfEvent @aprof_event_t;
  EXTERN_APROF datatype AProfEvent {
    AllocE(alloc_event_t);
    GcE(gc_event_t);
    ReclaimE(reclaim_event_t);
    CreateE(create_event_t);
    ResizeE(resize_event_t);
    FreeE(free_event_t);
  };
  
  // For processing profiling files.  Callback will return false
  //   if something goes wrong.
  extern int proc_file(string_t file, fn_t<aprof_event_t,bool,`H> callback);

  // Controls debug printing level to stderr
  extern bool verbose;

  // Controls threshhold for printing dynamic regions in graph
  extern double dyn_threshhold;

  // Whether to aggregate or print individual dynamic regions for graph
  extern bool all_dynregions;

  // Uses proc_file above to generate a graph of the system's memory
  // usage.  The output graph is printed to stdout as a jgraph graph
  // (see http://www.cs.utk.edu/~plank/plank/jgraph/jgraph.html)
  // or a gnuplot graph (http://www.gnuplot.info/).
  extern int generate_graph(string_t<`H> file);

  extern int generate_newgraph(string_t<`H> file);
  extern int generate_svg(string_t<`H> file);

  // Uses proc_file above to generate a tabular summary of per-region
  // allocation information.
  extern int generate_summary(string_t<`H> file);
}

#endif
