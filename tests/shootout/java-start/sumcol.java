// $Id: sumcol.java,v 1.2 2004-10-09 17:15:47 mwh Exp $
// http://www.bagley.org/~doug/shootout/

import java.io.*;
import java.util.*;
import java.text.*;

public class sumcol {
    static void go(BufferedReader in) {
	int sum = 0;
	String line;
        try {
            while ((line = in.readLine()) != null) {
		sum = sum + Integer.parseInt(line);
            }
        } catch (IOException e) {
            System.err.println(e);
            return;
        }
	System.out.println(Integer.toString(sum));
    }
    public static void main(String[] args) {
	BufferedReader in;
	if (args.length == 2) {
	    String inFile = args[1];
	    try {
	        in = new BufferedReader(new FileReader(inFile));
	    } catch (IOException e) {
		System.err.println(e);
		return;
	    }
	    go(in);
	}
	//@NOWARM
	//@START
	in = new BufferedReader(new InputStreamReader(System.in));
	go(in);
	//@END
    }
}
