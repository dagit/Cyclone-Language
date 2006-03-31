package cycinf.util;

import java.io.*;
import java.util.regex.*;

import cycinf.cvars.usedtype.Identifier;

public abstract class ParsingUtils {
	private ParsingUtils() {}

	private static final Pattern identifierPat = Pattern.compile("(\\d+)I");

	public static String preprocess(String x) throws ParseException {
		Matcher idMat = identifierPat.matcher(x);
		StringBuilder sb = new StringBuilder(x.length());
		int pos = 0;
		while (idMat.find(pos)) {
			int chunkStart = idMat.start();
			sb.append(x.substring(pos, chunkStart));
			int idLen = Integer.parseInt(idMat.group(1));
			int idStart = idMat.end();
			int idEnd = idStart + idLen;
			String id;
			try {
				id = x.substring(idStart, idEnd);
			} catch (StringIndexOutOfBoundsException e) {
				throw new ParseException(
					"Failed to preprocess: invalid identifier length",
					e);
			}
			sb.append('(');
			sb.append(id);
			sb.append(')');
			pos = idEnd;
		}
		sb.append(x.substring(pos));
		return sb.toString();
	}

	public static boolean isValidIdentifier(String x) {
		if (x.equals(""))
			return false;
		if (!Character.isJavaIdentifierStart(x.charAt(0)))
			return false;
		for (int i = 1; i < x.length(); i++)
			if (!Character.isJavaIdentifierPart(x.charAt(i)))
				return false;
		return true;
	}

	public static String validIdentifier(String x) throws ParseException {
		if (isValidIdentifier(x))
			return x;
		else
			throw new ParseException("Bad identifier: " + x);
	}

	public static String chopPrefix(String x, String prefix) {
		if (x.startsWith(prefix))
			return x.substring(prefix.length());
		else
			return null;
	}

	public static String chopSuffix(String x, String suffix) {
		if (x.endsWith(suffix))
			return x.substring(0, x.length() - suffix.length());
		else
			return null;
	}

	/*
	 * HMMM The DirTracker mechanism may be threadsafe, but the use of these
	 * static fields to return parsing results is not.
	 */
	public static int nval;
	public static Identifier ival;

	public static String chopNumberSuffix(String x) throws ParseException {
		int i = x.length();
		while (i > 0 && Character.isDigit(x.charAt(i - 1)))
			i--;
		if (i == x.length())
			return null;
		String main = x.substring(0, i);
		try {
			nval = Integer.parseInt(x.substring(i)); // shouldn't
			// get
			// NumberFormatException
		} catch (NumberFormatException e) {
			throw new ParseException(
				"Something wrong with a number", e);
		}
		return main;
	}

	public static String chopIdentifierSuffix(String x)
		throws ParseException {
		if (x.charAt(x.length() - 1) == ')') {
			int b = x.lastIndexOf('(');
			if (b == -1)
				throw new ParseException(
					"Internal: Badly bounded identifier: "
						+ x);
			ival = Identifier.unwrap(x.substring(b, x.length()));
			return x.substring(0, b);
		} else
			return null;
	}

	public static String escape(String in) {
		StringBuffer out = new StringBuffer("\"");
		for (int pos = 0; pos < in.length(); pos++)
			switch (in.charAt(pos)) {
			case '\n':
				out.append("\\n");
				break;
			case '\t':
				out.append("\\t");
				break;
			case '\\':
				out.append("\\\\");
				break;
			case '\'':
				out.append("\\\'");
				break;
			case '\"':
				out.append("\\\"");
				break;
			default:
				out.append(in.charAt(pos));
			}
		out.append('\"');
		return out.toString();
	}

	@SuppressWarnings("unused")
	public static String unescape(String x, ParsePosition used)
		throws ParseException {
		StringReader is = new StringReader(x);
		StreamTokenizer st = new StreamTokenizer(is);
		try {
			st.nextToken();
		} catch (IOException e) {
			throw new RuntimeException(
				"My StringReader should not throw an IOException!",
				e);
		}
		if (st.ttype != '"')
			return null;
		// this is a hack, but I couldn't find a practical better way;
		// assumes no nonstandard escapes
		used.setIndex(escape(st.sval).length());
		return st.sval;
	}
}
