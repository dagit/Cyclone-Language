// $Id: regexmatch.java,v 1.4 2004-10-09 17:15:47 mwh Exp $
// http://www.bagley.org/~doug/shootout/

import java.io.*;
import java.util.*;
import java.text.*;
import java.lang.reflect.*;
import org.apache.oro.text.regex.*;

public class regexmatch {

    public static void main(String args[]) 
	throws IOException, MalformedPatternException {
	int n = Integer.parseInt(args[0]);
	BufferedReader in;
	if (args.length == 2) {
	    String inFile = args[1];
	    try {
	        in = new BufferedReader(new FileReader(inFile));
	    } catch (IOException e) {
		System.err.println(e);
		return;
	    }
	    go(in,n);
	}
	//@NOWARM
	//@START
	in = new BufferedReader(new InputStreamReader(System.in));
	go(in,n);
	//@END
    }

    static void go(BufferedReader in, int n)
	throws IOException, MalformedPatternException {

	LinkedList lines = new LinkedList();
	try {
	    String line;
	    while ((line = in.readLine()) != null) {
		lines.addLast(line);
	    }
	    in.close();
        } catch (IOException e) {
            System.err.println(e);
            return;
        }

	String regex = 
	    "(?: ^ | [^\\d\\(])		# must be preceeded by non-digit\n" +
	    "(?:			# area code is 3 digits\n" +
	    " \\((\\d\\d\\d)\\)		# match 1: with parens\n" +
	    "|				# or\n" +
	    " (\\d\\d\\d)		# match 2: without parens\n" +
	    ")				# if match1 then match right paren\n" +
	    "[ ]			# area code followed by one space\n" +
	    "(\\d\\d\\d)		# match 3: prefix of 3 digits\n" +
	    "[ -]			# separator is either space or dash\n" +
	    "(\\d\\d\\d\\d)		# match 4: last 4 digits\n" +
	    "(?:\\D|$)			# must be followed by a non-digit\n";

	PatternMatcher matcher = new Perl5Matcher();
	PatternCompiler compiler = new Perl5Compiler();
	Pattern pattern = null;

	try {
	    pattern = compiler.compile(regex, Perl5Compiler.EXTENDED_MASK);
	} catch(MalformedPatternException e) {
	    System.err.println("Bad pattern.");
	    System.err.println(e.getMessage());
	    System.exit(1);
	}

	for (int i = 0; i < n; i++) {
	    LinkedList phones = new LinkedList();
	    for (ListIterator li = lines.listIterator(); li.hasNext();) {
		String line = (String)li.next();
		if (matcher.contains(line, pattern)) {
		    MatchResult match = matcher.getMatch();
		    String num = "";
		    if (match.group(1) != null) {
			num = "(" + match.group(1) + ") " + match.group(3) + "-" + match.group(4);
		    } else {
			num = "(" + match.group(2) + ") " + match.group(3) + "-" + match.group(4);
		    }
		    phones.addLast(num);
		}
	    }
	    if (i == (n - 1)) {
		int count = 1;
		for (ListIterator li = phones.listIterator(); li.hasNext();) {
		    String num = (String)li.next();
		    System.out.println(count++ + ": " + num);
		}
	    }
	}
    }
}
