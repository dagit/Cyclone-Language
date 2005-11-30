/*
 * Copyright (c) 2000-2004
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
package banshee.engine;


public class UnsafeNonspecEngine {
    static UnsafeErrorHandler errorHandler;

    static {
	System.loadLibrary("jnsengine");
	initialize();
    }

    private static native void initialize();
    public static native void reset();


    // Flags

    public static native void mergeProjections(boolean value);
    public static native void eliminateCycles(boolean value);
    public static native void occursCheck(boolean value);


    // Error handling

    public static void setErrorHandler(UnsafeErrorHandler h) {
	errorHandler = h;
    }

    // Constructors
    public static native long makeConstructor(String name, SortKind kind,
					      SignatureElement[] sig);
    public static native long makeConstructorExpr(long constructor, 
						  long[] exps);

    // Constants
    public static native long makeSetIFConstant(String name);
    public static native long makeSetSTConstant(String name);
    public static native long makeTermConstant(String name);


    // Projection patterns
    public static native long makeSetIFProjPat(long constructor,
					       int index,
					       long expr);
    public static native long makeSetSTProjPat(long constructor, 
					       int index,
					       long expr);

    // Projections
    public static native long makeSetIFProj(long constructor,
					    int index,
					    long expr);
    public static native long makeSetSTProj(long constructor,
					    int index,
					    long expr);

    // Variables
    public static native long makeSetIFFresh(String name);
    public static native long makeSetSTFresh(String name);
    public static native long makeTermFresh(String name);
    public static native long makeFlowrowFresh(String name, SortKind base);

    // Unions
    public static native long makeSetIFUnion(long[] exps);
    public static native long makeSetSTUnion(long[] exps);

    // Intersections
    public static native long makeSetIFInter(long[] exps);
    public static native long makeSetSTInter(long[] exps);
}