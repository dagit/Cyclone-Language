/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

// This is implements the stack used by exceptions and lexical regions.

#include <stdio.h>

#include "runtime_internal.h"

// Need one per thread
static struct _RuntimeStack *_current_frame = NULL;

void _push_frame(struct _RuntimeStack *frame) {
  frame->next = _current_frame;
  _current_frame = frame;
}

// set _current_frame to its n+1'th tail (i.e. pop n+1 frames)
// Invariant: result is non-null
void _npop_frame(unsigned int n) {
  unsigned int i;
  for(i = n; i <= n; i--) {
    if(_current_frame == NULL) {
      fprintf(stderr,"internal error: empty frame stack\n");
      exit(1);
    } 
    if (_current_frame->cleanup != NULL)
      _current_frame->cleanup(_current_frame);
    _current_frame = _current_frame->next;
  }
}

struct _RuntimeStack * _top_frame() {
  return _current_frame;
}

// set _current_frame to the first frame with the given tag.
// If no such frame is found, _current_frame is set to NULL.
struct _RuntimeStack * _pop_frame_until(int tag) {
  while (_current_frame != NULL && _current_frame->tag != tag)
    _current_frame = _current_frame->next;
  return _current_frame;
}
