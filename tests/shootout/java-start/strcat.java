// $Id: strcat.java,v 1.2 2004-09-07 20:33:28 mwh Exp $
// http://www.bagley.org/~doug/shootout/
// Pelle Nilsson suggested we also illustrate StringBuffer
// since it is the preferred method for concatenating 
// strings in Java

import java.io.*;
import java.util.*;

public class strcat {
    public static void main(String args[]) throws IOException {
	int n = Integer.parseInt(args[0]);
	//@START
	String hello = "hello\n";
	StringBuffer stringBuffer = new StringBuffer(32);

        for (int i=0; i<n; i++) {
            stringBuffer.append(hello);
	}

	System.out.println(stringBuffer.length());
	//@END
    }
}
