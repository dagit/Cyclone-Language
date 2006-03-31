package cycinf.constraints;

import cycinf.cvars.Cvalue;
import cycinf.infstats.DirInfstats;
import cycinf.util.*;

public abstract class CvalueExpression extends CNode {
	public abstract Cvalue evaluate(DirInfstats reference);
	
	public abstract String prettyString();

	public static final CvalueExpression parse(String s, ParsePosition used)
		throws ParseException {
		CvalueExpression v;
		if ((v = CvarRef.parse1(s, used)) != null
			|| (v = CvalueRef.parse1(s, used)) != null)
			return v;
		else
			throw new ParseException(
				"Unrecognized CvalueExpression: " + s);
	}
}
