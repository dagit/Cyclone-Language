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
#include <sys/types.h>
#include <time.h>
#include <list.h>

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
  struct GcAddToHeapEvent {
    clock_t time;
    unsigned int address;
    unsigned int amount;
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
  struct MarkEvent {
    clock_t time;
    string_t m;
  };
  typedef struct AllocEvent alloc_event_t;
  typedef struct GcEvent gc_event_t;
  typedef struct GcAddToHeapEvent gcaddtoheap_event_t;
  typedef struct ReclaimEvent reclaim_event_t;
  typedef struct CreateEvent create_event_t;
  typedef struct ResizeEvent resize_event_t;
  typedef struct FreeEvent free_event_t;
  typedef struct MarkEvent mark_event_t;
  
  // The main event definition
  typedef datatype AProfEvent @aprof_event_t;
  EXTERN_APROF datatype AProfEvent {
    AllocE(alloc_event_t);
    GcE(gc_event_t);
    GcAddToHeapE(gcaddtoheap_event_t);
    ReclaimE(reclaim_event_t);
    CreateE(create_event_t);
    ResizeE(resize_event_t);
    FreeE(free_event_t);
    MarkE(mark_event_t);
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

  // Whether to use events instead of clocks
  extern bool use_events;

  // Whether to print the reserved memory on the graph
  extern List::list_t<stringptr_t> omits;

  // Uses proc_file above to generate a graph of the system's memory
  // usage.  The output graph is printed to stdout as a jgraph graph
  // (see http://www.cs.utk.edu/~plank/plank/jgraph/jgraph.html)
  // or a gnuplot graph (http://www.gnuplot.info/).
  extern int generate_graph(string_t<`H> file);

  extern int generate_newgraph(string_t<`H> file);
  extern int generate_svg(string_t<`H> file);
  extern int generate_svg2(string_t<`H> file);
  extern int generate_svg3(string_t<`H> file);
  extern int generate_sizes(string_t<`H> file);

  // Uses proc_file above to generate a tabular summary of per-region
  // allocation information.
  extern int generate_summary(string_t<`H> file);

  // Clock granularity
  extern unsigned int clockdivisor;

  // Statistics
  extern clock_t max_time;

  extern unsigned int num_create_events;
  extern unsigned int num_alloc_events;
  extern unsigned int num_free_events;
  extern unsigned int num_reclaim_events;
  extern unsigned int num_resize_events;
  extern unsigned int num_gc_events;
  extern unsigned int num_gc_add_to_heap;

  extern unsigned int num_alloc_heap;
  extern unsigned int bytes_alloc_heap;

  extern unsigned int num_alloc_unique;
  extern unsigned int bytes_alloc_unique;
  extern unsigned int num_freed_unique;
  extern unsigned int bytes_freed_unique;

  extern unsigned int num_alloc_refcnt;
  extern unsigned int bytes_alloc_refcnt;
  extern unsigned int num_freed_refcnt;
  extern unsigned int bytes_freed_refcnt;

  extern unsigned int num_alloc_other;
  extern unsigned int bytes_alloc_other;

}

// Colors etc. to use on the graph
extern double WIDTH;
extern double HEIGHT;
extern unsigned int MAX_Y;
extern unsigned int XMAXRES;
extern bool landscape;
extern const char ??colors;
extern const char ?curve_stroke;
extern const char ?gc_fill;
extern const char ?gc_opacity;
extern const char ?graph_translate;
extern double x_caption_gap;
extern double y_caption_gap;
extern const char ?x_caption;
extern const char ?y_caption;
extern const char ?font_family;
extern const char ?font_size;
extern bool black_and_white;
extern bool gc_caps;
extern bool sort_changes;
extern bool fixed_colors;
extern bool no_legend;
extern bool omit_gc;

#endif
