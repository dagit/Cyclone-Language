package cycinf.util;

import java.util.regex.*;

import cycinf.cvars.usedtype.*;
import cycinf.infstats.DirTracker;

public final class SourceSearch {
	private int startPos, endPos;
	public final int getStartPos() {
		return startPos;
	}
	public final int getEndPos() {
		return endPos;
	}
	private boolean confident;
	public final boolean isConfident() {
		return confident;
	}

	private SourceSearch() {}

	private SourceSearch(int startPos, int endPos, boolean confident) {
		this.startPos = startPos;
		this.endPos = endPos;
		this.confident = confident;
	}
	
	public static SourceSearch justFindLocation(DirTracker dir, SourceLocation loc) {
		try {
			int pos = loc.locateInString(FileUtils.readFile(loc.findFileInDir(dir)));
			return new SourceSearch(pos, pos + 1, true);
		} catch (Exception e) {
			e.printStackTrace();
			return new SourceSearch(0, 0, false);
		}
	}

	public static SourceSearch findUsedTypeLayer(DirTracker dir,
		SourceLocation sl, UsedTypeLayer utl) {
		try {
			return findPointer(sl, FileUtils.readFile(sl
				.findFileInDir(dir)), utl);
		} catch (Exception e) {
			e.printStackTrace();
			return new SourceSearch(0, 0, false);
		}
	}

	private static String patternForIdentifier(Identifier i) {
		return "(?<!\\w)" + Pattern.quote(i.getName()) + "(?!\\w)";
	}

	private static final String patternForAnyIdentifier = "(?<!\\w)\\w+(?!\\w)";
	private static final String ignorable = "(?:\\s|`"
		+ patternForAnyIdentifier + ")*";

	static SourceSearch findPointer(SourceLocation sl, String source,
		UsedTypeLayer utl) {
		int pos1 = sl.locateInString(source);
		SourceSearch ss = new SourceSearch(pos1, pos1, false);
		UsedType ut = utl.getType();
		if (ut instanceof LocalVariable) {
			LocalVariable lv = (LocalVariable) ut;
			ss = findIdentifierPointerNear(pos1, source, lv
				.getLocalName(), utl.getDepth());
		} else if (ut instanceof GlobalVariable) {
			GlobalVariable gv = (GlobalVariable) ut;
			ss = findIdentifierPointerNear(pos1, source, gv
				.getName(), utl.getDepth());
		}
		if (!ss.isConfident()) {
			// If we don't have a good answer yet, try this!
			if (sl.getCharPos() != SourceLocation.UNKNOWN
				&& (source.charAt(pos1) == '*'
					|| source.charAt(pos1) == '?' || source
					.charAt(pos1) == '@')) {
				ss.startPos = pos1;
				ss.endPos = pos1 + 1;
				ss.confident = true;
			}
		}
		return ss;
	}

	private static SourceSearch findIdentifierPointerNear(int pos1,
		String source, Identifier identifier, int depth) {
		String patternStr = "([\\*\\@\\?])" + ignorable
			+ "(?:[\\*\\@\\?]" + ignorable + "){" + depth + "}"
			+ patternForIdentifier(identifier);
		Pattern thePattern = Pattern.compile(patternStr);
		return findPatternMatchNear(pos1, source, thePattern, 1);
	}

	private static final int CONSIDER_RANGE = 256;

	static SourceSearch findPatternMatchNear(int pos1, String source,
		Pattern pat, int group) {
		int startConsider = Math.max(0, pos1 - CONSIDER_RANGE);
		int endConsider = Math.min(source.length(), pos1
			+ CONSIDER_RANGE);
		Matcher theMatcher = pat.matcher(source);
		theMatcher.region(startConsider, endConsider);
		theMatcher.useTransparentBounds(true);
		int bestMatchStart = -1, bestMatchEnd = -1;
		// System.out.println("Preparing to match against pattern " +
		// pat.pattern());
		while (theMatcher.find()) {
			int matchStart = theMatcher.start(group);
			// System.out.println("Found a match at position " +
			// matchStart);
			if (bestMatchStart == -1
				|| Math.abs(matchStart - pos1) < Math
					.abs(bestMatchStart - pos1)) {
				bestMatchStart = matchStart;
				bestMatchEnd = theMatcher.end(group);
			}
		}
		SourceSearch ss = new SourceSearch();
		if (bestMatchStart == -1) {
			ss.startPos = pos1;
			ss.endPos = pos1;
			ss.confident = false;
		} else {
			ss.startPos = bestMatchStart;
			ss.endPos = bestMatchEnd;
			ss.confident = true;
		}
		return ss;
	}
}
