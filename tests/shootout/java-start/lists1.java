// $Id: lists1.java,v 1.1 2004-09-06 19:33:45 mwh Exp $
// http://www.bagley.org/~doug/shootout/

import java.io.*;
import java.util.*;
import java.text.*;
import java.lang.reflect.*;

public class lists1 {
    public static void main(String args[]) {
	int n = Integer.parseInt(args[0]);

	// create empty list B
	LinkedList B = new LinkedList();

	// create list (A) of integers from 1 through N
	LinkedList A = new LinkedList();
	for (int i=n; i>0; i--) {
	    A.addFirst(new Integer(i));
	}

	// move each individual item from A to B, in a loop, reversing order
	while (! A.isEmpty()) {
	    B.addFirst(A.removeFirst());
	}

	// print first 2 elements of B
	System.out.println(B.get(0) + " " + B.get(1));

	// reverse B (can be done in place)
	LinkedList tmp = new LinkedList();
	while (! B.isEmpty()) {
	    tmp.addFirst(B.removeFirst());
	}
	B = tmp;
	// java.util.Collections.reverse(B);


	// is 0 a member of B?
	System.out.println((B.contains(new Integer(0))) ? "true" : "false");

	// is N a member of B?
	System.out.println((B.contains(new Integer(n))) ? "true" : "false");

	// filter values from B to A that are less than N/2, preserving order
	int mid = n/2;
	for (ListIterator li = B.listIterator(); li.hasNext();) {
	    int val = ((Integer)li.next()).intValue();
	    if (val < mid) A.addLast(new Integer(val));
	}

	// print first ten items of A
	int i = 0;
	for (ListIterator li = A.listIterator(); i<10 && li.hasNext(); i++) {
	    System.out.print(((Integer)li.next()).intValue() + ((i<9) ? " " : "\n"));
	}

	// print sum of items in A that are less than 1000
	int sum = 0;
	for (ListIterator li = A.listIterator(); li.hasNext();) {
	    int val = ((Integer)li.next()).intValue();
	    if (val < 1000) sum += val;
	}
	System.out.println(sum);

	// append B to end of A
	A.addAll(B);

	// print length and last element of A
	System.out.println(A.size() + " " + ((Integer)A.getLast()).intValue());
    }
}
