#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

namespace Mman {

  // In C, MAP_FAILED is (void*)-1, but I have no idea why.  The user
  // can never request a map at (void*)0, since that is defined to
  // mean "I don't care where you map it".  And mmap() will never
  // decide to map anything there.  So it probably should have been
  // NULL all along.

#define MAP_FAILED NULL

  // Of course, there are safety problems with mmap'ing: don't try to
  // access the memory in a way that violates the protection you
  // requested.
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define PROT_NONE 0

  // MAP_FIXED is unsupported, so we don't define it.
#define MAP_SHARED 1
#define MAP_PRIVATE 2

  // The traditional first arg to mmap should be 0 anyway, so don't
  // even bother to pass it to us.
  extern char ?mmap(size_t length, int prot, int flags, int fd, off_t offset);

  // Is it unsafe to ask the user for the size?  I'd like to take it
  // from the char? type, but that could have been cast around...
  extern int munmap(char ?start, size_t length);
}

#endif
