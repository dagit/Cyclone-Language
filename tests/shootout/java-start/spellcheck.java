// $Id: spellcheck.java,v 1.3 2004-10-09 17:15:47 mwh Exp $
// http://www.bagley.org/~doug/shootout/

import java.io.*;
import java.util.*;

public class spellcheck {
    static void go(BufferedReader in) {
	HashMap dict = new HashMap();
	String word;

	try {
	    BufferedReader din = new BufferedReader(new FileReader("Usr.Dict.Words"));
	    while ((word = din.readLine()) != null) {
		dict.put(word, new Integer(1));
	    }
	    din.close();
        } catch (IOException e) {
            System.err.println(e);
            return;
        }

	try {
	    while ((word = in.readLine()) != null) {
		if (!dict.containsKey(word)) {
		    System.out.println(word);
		}
	    }
        } catch (IOException e) {
            System.err.println(e);
            return;
        }
    }
    public static void main(String args[]) throws IOException {
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
