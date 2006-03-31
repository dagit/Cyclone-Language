package cycinf.constraints;

import general.util.Validator;
import cycinf.util.*;

public class GuardedConstraint extends Constraint {
	private final Guard ifPart;
	public final Guard getIfPart() {
		return ifPart;
	}
	private final Constraint thenPart;
	public final Constraint getThenPart() {
		return thenPart;
	}

	public GuardedConstraint(SourceLocation location, Guard constraint1,
		Constraint constraint2) {
		super(location);
		this.ifPart = Validator.nonnull(constraint1, "ifPart");
		this.thenPart = Validator.nonnull(constraint2, "thenPart");
	}

	@Override
	public final String toString1() {
		return ">(" + ifPart.toString() + "," + thenPart.toString()
			+ ")";
	}

	public static final GuardedConstraint parse1(String s,
		ParsePosition used, SourceLocation loc) throws ParseException {
		used.setIndex(0);
		String s1 = ParsingUtils.chopPrefix(s, ">");
		if (s1 == null)
			return null;
		used.advance(1);
		String s2 = ParsingUtils.chopPrefix(s1, "(");
		if (s2 == null)
			throw new ParseException(
				"Badly parenthesized GuardedConstraint: " + s1);
		used.advance(1);
		ParsePosition ifUsed = new ParsePosition();
		Guard ifPart = Guard.parse(s2, ifUsed); // conveniently reuse
		// "used"
		String s3 = s2.substring(ifUsed.getIndex());
		used.advance(ifUsed.getIndex());
		String s4 = ParsingUtils.chopPrefix(s3, ",");
		if (s4 == null)
			throw new ParseException(
				"Badly comma-ed GuardedConstraint: " + s3);
		used.advance(1);
		ParsePosition thenUsed = new ParsePosition();
		Constraint thenPart = Constraint.parse(s4, thenUsed);
		String s5 = s4.substring(thenUsed.getIndex());
		used.advance(thenUsed.getIndex());
		String s6 = ParsingUtils.chopPrefix(s5, ")");
		if (s6 == null)
			throw new ParseException(
				"Badly parenthesized GuardedConstraint: " + s5);
		used.advance(1);
		return new GuardedConstraint(loc, ifPart, thenPart);
	}
}
