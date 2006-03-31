package cycinf.constraints;

import general.util.Validator;
import cycinf.util.*;

public class AssignmentConstraint extends Constraint {
	private final CvalueExpression targetValue;
	public final CvalueExpression getTargetValue() {
		return targetValue;
	}
	private final CvalueExpression sourceValue;
	public final CvalueExpression getSourceValue() {
		return sourceValue;
	}

	public AssignmentConstraint(SourceLocation location,
		CvalueExpression targetValue, CvalueExpression sourceValue) {
		super(location);
		this.targetValue = Validator
			.nonnull(targetValue, "targetValue");
		this.sourceValue = Validator
			.nonnull(sourceValue, "sourceValue");
	}

	@Override
	public final String toString1() {
		return "=(" + getTargetValue().toString() + ","
			+ getSourceValue().toString() + ")";
	}

	public static final AssignmentConstraint parse1(String s,
		ParsePosition used, SourceLocation loc) throws ParseException {
		used.setIndex(0);
		String s1 = ParsingUtils.chopPrefix(s, "=");
		if (s1 == null)
			return null;
		used.advance(1);
		String s2 = ParsingUtils.chopPrefix(s1, "(");
		if (s2 == null)
			throw new ParseException(
				"Badly parenthesized AssignmentConstraint: "
					+ s);
		used.advance(1);
		ParsePosition svused = new ParsePosition();
		CvalueExpression sv = CvalueExpression.parse(s2, svused); // conveniently
		// reuse
		// "used"
		String s3 = s2.substring(svused.getIndex());
		used.advance(svused.getIndex());
		String s4 = ParsingUtils.chopPrefix(s3, ",");
		if (s4 == null)
			throw new ParseException(
				"Badly comma-ed AssignmentConstraint: " + s);
		used.advance(1);
		ParsePosition tvused = new ParsePosition();
		CvalueExpression tv = CvalueExpression.parse(s4, tvused);
		String s5 = s4.substring(tvused.getIndex());
		used.advance(tvused.getIndex());
		String s6 = ParsingUtils.chopPrefix(s5, ")");
		if (s6 == null)
			throw new ParseException(
				"Badly parenthesized AssignmentConstraint: "
					+ s);
		used.advance(1);
		return new AssignmentConstraint(loc, sv, tv);
	}
}
