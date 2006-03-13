/*
 * Copyright (c) 2005
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include "annotations.h"

/* Empty annotations */

struct annotation_ {
};

struct annotations_ {
};

/* Get the next annotation */
bool annotation_next(annotations_scanner *scanner, annotation *a) {
  return FALSE;
}

/* function that returns true if a is the empty annotation */
bool is_empty_annotation(annotation a) {
  return TRUE;
}

/* function that returns the empty annotation */
annotation get_empty_annotation(void) {
  return NULL;
}

/* function that returns the concatenation of two annotations */
annotation concat_annotation(annotation a1, annotation a2) {
  return NULL;
}

/* function that returns the kleene closure of an annotation  */
annotation star_annotation(annotation a) {
  return NULL;
}

/* function that returns TRUE if one annotation subsumes another */
bool subsumed_by_annotation(annotation a1, annotation a2) {
  return TRUE;
}

/* function that prints an annotation */
void print_annotation(annotation a) {
}
