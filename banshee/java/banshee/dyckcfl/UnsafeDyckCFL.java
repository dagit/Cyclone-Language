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
package banshee.dyckcfl;

// A wrapper around the DyckCFL Banshee API. For efficiency, this code
// assumes that longs are big enough to store C pointers. Then each
// long (really, a C address), is a unique ID for a node. This
// minimizes native->java conversion, but is hardly safe.

/**
 * A wrapper around the Banshee DyckCFL API. This code assumes that
 * longs are big enough to store C pointers. Each long is a valid C
 * address for a node. While this scheme is efficient, it is not
 * safe.
 *
 * @author John Kodumal
 */

public class UnsafeDyckCFL {
    private static UnsafeDyckCFL instance = new UnsafeDyckCFL();

    static {
	System.loadLibrary("jdyckcfl");
	initialize();
    }

    private UnsafeDyckCFL() { }

    public static UnsafeDyckCFL v() { return instance; }

    private static native void initialize();

    public native void printDyckConstraints(boolean value);

    public native long makeTaggedNode(String name);

    public native long makeUntaggedNode(String name);

    public native void markNodeGlobal(long node);

    public native void makeSubtypeEdge(long node1, long node2);

    public native void makeOpenEdge(long node1, long node2, int index);

    public native void makeCloseEdge(long node1, long node2, int index);

    public native boolean checkReaches(long node1, long node2);
    
    public native boolean checkPNReaches(long node1, long node2);

    public static void main(String args[]) {
	UnsafeDyckCFL cflEngine = UnsafeDyckCFL.v();;

	long n0 = cflEngine.makeTaggedNode("foo");
	long n1 = cflEngine.makeTaggedNode("bar");
	long n2 = cflEngine.makeTaggedNode("baz");
	long n3 = cflEngine.makeTaggedNode("bum");
	long n4 = cflEngine.makeTaggedNode("blog");

	cflEngine.makeOpenEdge(n0,n1,1);
	cflEngine.makeSubtypeEdge(n1,n2);
	cflEngine.makeCloseEdge(n2,n3,1);
	cflEngine.makeCloseEdge(n2,n4,2);

	System.out.println("Checking reachability (should be true): " 
			   + cflEngine.checkReaches(n0,n3));
	System.out.println("Checking reachability (should be false): " 
			   + cflEngine.checkReaches(n0,n4));
    }

}