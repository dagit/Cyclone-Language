package cycinf.constraints;

import general.util.Validator;
import cycinf.cvars.Cvalue;
import cycinf.infstats.DirInfstats;
import cycinf.util.*;

public class CvalueRef extends CvalueExpression {
	private final Cvalue cvalue;
	public final Cvalue getCvalue() {
		return cvalue;
	}

	public CvalueRef(Cvalue cvalue) {
		this.cvalue = Validator.nonnull(cvalue, "cvalue");
	}

	@Override
	public final String toString() {
		return ParsingUtils.escape(cvalue.toString());
	}
	
	@Override
	public final String prettyString() {
		return cvalue.prettyString();
	}

	public static final CvalueRef parse1(String s, ParsePosition used)
		throws ParseException {
		String s1 = ParsingUtils.unescape(s, used);
		if (s1 == null)
			return null;
		Cvalue v = Cvalue.fromString(s1);
		if (v == null)
			throw new ParseException("Unrecognized CvalueRef: "
				+ s1);
		else
			return new CvalueRef(v);
	}

	@Override
	public final Cvalue evaluate(DirInfstats reference) {
		return getCvalue();
	}
}
