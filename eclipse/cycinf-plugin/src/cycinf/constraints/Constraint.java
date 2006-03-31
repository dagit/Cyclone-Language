package cycinf.constraints;

import cycinf.util.*;

/*
 * The model of constraints in package cycinf.constraints handles parsing but
 * creates functional, "unaffiliated" constraints. A
 * cycinf.reasoning.RConstraint is affiliated with a DirInfstats and has many
 * additional reasoning and display features.
 */

public abstract class Constraint extends LocatableCNode {
	protected Constraint(SourceLocation location) {
		super(location);
	}

	@Override
	public abstract String toString1();

	public static final Constraint parse(String s, ParsePosition used)
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
		Constraint d;
		if ((d = AssignmentConstraint.parse1(s1, used2, loc)) != null
			|| (d = GuardedConstraint.parse1(s1, used2, loc)) != null) {
			used.advance(used2.getIndex());
			return d;
		} else
			throw new ParseException("Unrecognized Constraint: "
				+ s);
	}
}
