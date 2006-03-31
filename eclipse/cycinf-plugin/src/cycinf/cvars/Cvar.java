package cycinf.cvars;

import cycinf.cvars.ptrbnd.PointerKindCvar;
import cycinf.util.ParseException;

public abstract class Cvar extends CvarPiece {
	// Returns just the "target", i.e. enough information to understand what happened given the decision name
	public abstract String targetPrettyString();

	public static final Cvar parse(String x) throws ParseException {
		Cvar v;
		if ((v = PointerKindCvar.parse1(x)) != null)
			return v;
		else
			throw new ParseException("Unrecognized Cvar: " + x);
	}
}
