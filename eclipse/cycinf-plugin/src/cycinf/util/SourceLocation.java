package cycinf.util;

import general.util.EqualByStringRepresentation;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.*;

import cycinf.infstats.DirTracker;

public class SourceLocation extends EqualByStringRepresentation {
	public static final int UNKNOWN = -1;

	private final String filename;
	public final String getFilename() {
		return filename;
	}
	private final int line, charPos;
	public final int getLine() {
		return line;
	}
	public final int getCharPos() {
		return charPos;
	}

	public SourceLocation(String filename, int line, int charPos) {
		this.filename = filename;
		this.line = line;
		this.charPos = charPos;
	}

	public IFile findFileInDir(DirTracker dir) {
		IPath path = Path.fromPortableString(filename);
		if (path.isAbsolute())
			return dir.getDir().getWorkspace().getRoot().getFileForLocation(path);
		else
			return dir.getDir().getFile(Path.fromPortableString(filename));
	}
	
	// Parse the location, but if it's bad ("xx.cyp:(1:0)"), return null.
	// Temporary kludge until the bad location bug is fixed.
	public static SourceLocation parseIfGood(String str) throws ParseException {
		SourceLocation l = parse(str);
		if (l.getFilename().endsWith(".cyp")
			&& l.getLine() == 1 && l.getCharPos() == 0)
			return null;
		else
			return l;
	}

	public static SourceLocation parse(String str) throws ParseException {
		int firstColon = str.indexOf(':');
		if (firstColon == -1)
			throw new ParseException(
				"Can't parse as SourceLocation, no colon: "
					+ str);
		String filename = str.substring(0, firstColon);
		/*
		 * IFile file = (IFile)
		 * base.findMember(Path.fromOSString(filePathStr)); if (file ==
		 * null) throw new ParseException("Can't parse as
		 * SourceLocation, file doesn't exist: " + filePathStr);
		 */
		int line, charPos;
		String posStr = str.substring(firstColon + 1);
		if (posStr.charAt(0) == '(') {
			int secondColon = posStr.indexOf(':');
			if (secondColon == -1)
				throw new ParseException(
					"Can't parse as SourceLocation, bad position: "
						+ posStr);
			if (posStr.charAt(posStr.length() - 1) != ')')
				throw new ParseException(
					"Can't parse as SourceLocation, bad position: "
						+ posStr);
			String lineStr = posStr.substring(1, secondColon);
			String charPosStr = posStr.substring(secondColon + 1,
				posStr.length() - 1);
			try {
				line = Integer.parseInt(lineStr);
				charPos = Integer.parseInt(charPosStr);
			} catch (NumberFormatException e) {
				throw new ParseException(
					"Can't parse as SourceLocation, bad line or character number in position: "
						+ posStr, e);
			}
		} else {
			try {
				line = Integer.parseInt(posStr);
			} catch (NumberFormatException e) {
				throw new ParseException(
					"Can't parse as SourceLocation, bad line number in position: "
						+ posStr, e);
			}
			charPos = UNKNOWN;
		}
		return new SourceLocation(filename, line, charPos);
	}

	public int locateInString(String source) {
		int curLine = 1;
		int i = 0;
		while (curLine < getLine()) {
			i = source.indexOf('\n', i);
			if (i == -1)
				return source.length();
			i++;
			curLine++;
		}
		if (getCharPos() == UNKNOWN)
			return i;
		else {
			int charsLeft = getCharPos();
			int end = source.indexOf('\n', i);
			if (end == -1)
				end = source.length();
			while (charsLeft > 0 && i < end) {
				if (source.charAt(i) == '\t')
					charsLeft -= 8; // account for
				// detailedlocation
				// special treatment of
				// tabs
				else
					charsLeft--;
				i++;
			}
			return i;
		}
	}

	@Override
	public final String toString() {
		return getFilename()
			+ ":"
			+ (getCharPos() == UNKNOWN ? Integer
				.toString(getLine()) : "("
				+ Integer.toString(getLine()) + ":"
				+ Integer.toString(getCharPos()) + ")");
	}
}
