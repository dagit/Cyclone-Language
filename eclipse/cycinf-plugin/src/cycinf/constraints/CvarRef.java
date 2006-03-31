package cycinf.constraints;

import general.util.Validator;
import cycinf.cvars.*;
import cycinf.infstats.DirInfstats;
import cycinf.util.*;

public class CvarRef extends CvalueExpression {
	private final Cvar cvar;
	public final Cvar getCvar() {
		return cvar;
	}

	public CvarRef(Cvar cvar) {
		this.cvar = Validator.nonnull(cvar, "cvar");
	}

	@Override
	public final String toString() {
		return "`" + cvar.toString();
	}
	
	@Override
	public final String prettyString() {
		return cvar.prettyString();
	}

	public static final CvarRef parse1(String s, ParsePosition used)
		throws ParseException {
		used.setIndex(0);
		String s1 = ParsingUtils.chopPrefix(s, "`");
		if (s1 == null)
			return null;
		used.advance("`".length());
		int i = 0;
		while (i < s1.length()
			&& Character.isJavaIdentifierPart(s1.charAt(i)))
			i++;
		used.advance(i);
		return new CvarRef(Cvar.parse(s1.substring(0, i)));
	}

	@Override
	public final Cvalue evaluate(DirInfstats reference) {
		return reference.getDecisions().lookup(getCvar()).getCvalue();
	}
}
