#@if cyclone
/*
 * 80x86-specific header file for setjmp() and longjmp().
 * Copyright (c) 2004, Michael Hicks <mwh@cs.umd.edu>
 * $Revision: 1.1 $
 * 
 * This file was based on a file setjmp.s and __longjmp.S in 
 * the GNU C Library, which have the following license:
 *
 * Copyright (C) 1995,1996,1997,1998,2000,2002 Free Software Foundation, Inc.
 * This file is part of the GNU C Library.
 *
 * The GNU C Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.  
 */

.globl setjmp
.align 4
setjmp:       
        movl 4(%esp), %eax

        movl %ebx, (0*4)(%eax)
        movl %esi, (1*4)(%eax)
        movl %edi, (2*4)(%eax)
        leal 4(%esp), %ecx
        movl %ecx, (4*4)(%eax)
        movl 0(%esp), %ecx
        movl %ecx, (5*4)(%eax)
        movl %ebp, (3*4)(%eax)

        pushl   %ebp
        movl    %esp, %ebp
        movl    $0, %eax
        leave
        ret

.globl __sigsetjmp
.align 4
__sigsetjmp:       
        movl    $0, %eax
        leave
        ret	

.globl longjmp
.align 4
longjmp:
	movl 4(%esp), %ecx
        movl 8(%esp), %eax
        movl (5*4)(%ecx), %edx
        movl (0*4)(%ecx), %ebx
        movl (1*4)(%ecx), %esi
        movl (2*4)(%ecx), %edi
        movl (3*4)(%ecx), %ebp
        movl (4*4)(%ecx), %esp

        jmp *%edx

#@endif
