package cycinf.cvars.usedtype;

import cycinf.cvars.CvarPiece;
import cycinf.util.ParseException;

public abstract class UsedType extends CvarPiece {
	public static final UsedType parse(String x) throws ParseException {
		UsedType t;
		// This seems to be a reasonable order in which to try the
		// possibilities.
		if ((t = FunctionReturnType.parse1(x)) != null
			|| (t = VariableFromList.parse1(x)) != null
			|| (t = Typedef.parse1(x)) != null
			|| (t = LocalVariable.parse1(x)) != null
			|| (t = GlobalVariable.parse1(x)) != null) // Should
			// be
			// last
			return t;
		else
			throw new ParseException("Unrecognized UsedType: " + x);
	}
}
