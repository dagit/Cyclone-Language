// $Id: ary2.java,v 1.2 2004-09-07 20:33:26 mwh Exp $
// http://www.bagley.org/~doug/shootout/

import java.io.*;
import java.util.*;

public class ary2 {
    public static void main(String args[]) {
	int i, n = Integer.parseInt(args[0]);
	//@START
	int x[] = new int[n];
	int y[] = new int[n];
	for (i = 0; i < n;) {
	    x[i] = i; ++i;
	    x[i] = i; ++i;
	    x[i] = i; ++i;
	    x[i] = i; ++i;
	    x[i] = i; ++i;

	    x[i] = i; ++i;
	    x[i] = i; ++i;
	    x[i] = i; ++i;
	    x[i] = i; ++i;
	    x[i] = i; ++i;
	}
	for (i = n-1; i >= 0;) {
	    y[i] = x[i]; --i;
	    y[i] = x[i]; --i;
	    y[i] = x[i]; --i;
	    y[i] = x[i]; --i;
	    y[i] = x[i]; --i;

	    y[i] = x[i]; --i;
	    y[i] = x[i]; --i;
	    y[i] = x[i]; --i;
	    y[i] = x[i]; --i;
	    y[i] = x[i]; --i;
	}

	System.out.println(y[n-1]);
	//@END
    }
}
