package cycinf.constraints;

import general.util.Validator;
import cycinf.infstats.DirInfstats;
import cycinf.util.*;

// Right now, ComparisonGuard objects are not used, but this
// class knows how to parse ^(=,foo,bar) into an AssignmentConstraint.
public class ComparisonGuard extends Guard {
	private final ComparisonOperator operator;
	public final ComparisonOperator getOperator() {
		return operator;
	}
	private final CvalueExpression value1;
	public final CvalueExpression getValue1() {
		return value1;
	}
	private final CvalueExpression value2;
	public final CvalueExpression getValue2() {
		return value2;
	}

	public ComparisonGuard(SourceLocation loc, ComparisonOperator operator,
		CvalueExpression value1, CvalueExpression value2) {
		super(loc);
		this.operator = Validator.nonnull(operator, "operator");
		this.value1 = Validator.nonnull(value1, "value1");
		this.value2 = Validator.nonnull(value2, "value2");
	}

	public static final ComparisonGuard parse1(String s,
		ParsePosition used, SourceLocation loc) throws ParseException {
		used.setIndex(0);
		String s1 = ParsingUtils.chopPrefix(s, "^(");
		if (s1 == null)
			return null;
		used.advance(2);
		String s2eq;
		ComparisonOperator op = null;
		if ((s2eq = ParsingUtils.chopPrefix(s1, "=,")) != null) {
			op = ComparisonOperator.EQUAL;
			used.advance(2);
		} else
			throw new ParseException(
				"Badly operator-ed ComparisonGuard");
		ParsePosition v1used = new ParsePosition();
		CvalueExpression val1 = CvalueExpression.parse(s2eq, v1used); // conveniently
		// reuse
		// "used"
		String s3 = s2eq.substring(v1used.getIndex());
		used.advance(v1used.getIndex());
		String s4 = ParsingUtils.chopPrefix(s3, ",");
		if (s4 == null)
			throw new ParseException(
				"Badly comma-ed ComparisonGuard: " + s);
		used.advance(1);
		ParsePosition v2used = new ParsePosition();
		CvalueExpression val2 = CvalueExpression.parse(s4, v2used);
		String s5 = s4.substring(v2used.getIndex());
		used.advance(v2used.getIndex());
		String s6 = ParsingUtils.chopPrefix(s5, ")");
		if (s6 == null)
			throw new ParseException(
				"Badly parenthesized ComparisonGuard: " + s);
		used.advance(1);
		return new ComparisonGuard(loc, op, val1, val2);
	}

	@Override
	public String toString1() {
		return "^(" + getOperator().symbol + ","
			+ getValue1().toString() + "," + getValue2().toString()
			+ ")";
	}

	@Override
	public boolean evaluate(DirInfstats reference) {
		return operator.evaluate(getValue1().evaluate(reference),
			getValue2().evaluate(reference));
	}
}
