package cycinf.constraints;

import cycinf.infstats.DirInfstats;
import cycinf.util.*;

public abstract class Guard extends LocatableCNode {
	protected Guard(SourceLocation location) {
		super(location);
	}

	@Override
	public abstract String toString1();

	public static final Guard parse(String s, ParsePosition used)
		throws ParseException {
		// "used" rule: on success, carries the amount of input used; on
		// failure, contents undefined
		used.setIndex(0);
		String s1;
		ParsePosition used2 = new ParsePosition();
		String locStr = ParsingUtils.unescape(s, used2);
		SourceLocation loc = locStr.equals("") ? null : SourceLocation.parseIfGood(locStr);
		used.advance(used2.getIndex());
		s1 = s.substring(used2.getIndex());
		Guard c;
		if ((c = ComparisonGuard.parse1(s1, used2, loc)) != null) {
			used.advance(used2.getIndex());
			return c;
		} else
			throw new ParseException("Unrecognized Guard: " + s);
	}

	public abstract boolean evaluate(DirInfstats reference);
}
