package cycinf.constraints;

import general.util.Validator;
import cycinf.infstats.DirInfstats;
import cycinf.util.*;

public abstract class ConstraintParser {
	private ConstraintParser() {}

	public static void parse(DirInfstats is, String line) {
		try {
			String[] fields = line.split("\\s+", 4);
			// Don't care about fields 0 through 2: we already have
			// them from --infstats output
			// Cvar cvar = Cvar.parse(fields[0]); -- remove `
			// SourceLocation loc =
			// SourceLocation.parse(is.getDir(), fields[1]);
			String constraintsStr = fields[3];
			constraintsStr = Validator.nonnull(ParsingUtils
				.chopPrefix(constraintsStr, "("),
				"constraintsStr chop (");
			constraintsStr = Validator.nonnull(ParsingUtils
				.chopSuffix(constraintsStr, ")"),
				"constraintsStr chop )");
			for (;;) {
				if (constraintsStr.equals(""))
					break;
				ParsePosition p = new ParsePosition();
				Constraint expr = Constraint.parse(
					constraintsStr, p);
				is.logConstraint(expr);
				constraintsStr = constraintsStr.substring(p
					.getIndex());
				if (constraintsStr.equals(""))
					break;
				constraintsStr = Validator.nonnull(ParsingUtils
					.chopPrefix(constraintsStr, ";"),
					"constraintsStr chop ;");
			}
		} catch (Exception e) {
			e.printStackTrace(System.err);
			is.logParseError();
		}
	}
}
