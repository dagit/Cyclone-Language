// $Id: spellcheck.java,v 1.2 2004-09-07 20:33:26 mwh Exp $
// http://www.bagley.org/~doug/shootout/

import java.io.*;
import java.util.*;

public class spellcheck {
    public static void main(String args[]) throws IOException {
	//@START
	HashMap dict = new HashMap();
	String word;

	try {
	    BufferedReader in = new BufferedReader(new FileReader("Usr.Dict.Words"));
	    while ((word = in.readLine()) != null) {
		dict.put(word, new Integer(1));
	    }
	    in.close();
        } catch (IOException e) {
            System.err.println(e);
            return;
        }
	
	try {
	    BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
	    while ((word = in.readLine()) != null) {
		if (!dict.containsKey(word)) {
		    System.out.println(word);
		}
	    }
        } catch (IOException e) {
            System.err.println(e);
            return;
        }
	//@END
    }
}
