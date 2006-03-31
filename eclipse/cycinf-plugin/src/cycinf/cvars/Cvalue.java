package cycinf.cvars;

import cycinf.cvars.ptrbnd.PointerKind;
import cycinf.util.ParseException;

public abstract class Cvalue implements Pretty {
	public abstract String valueString();
	@Override
	public final String toString() {
		return valueString();
	}
	public abstract String prettyString();

	public static Cvalue fromString(String x) throws ParseException {
		Cvalue v;
		if ((v = PointerKind.valueParse(x)) != null)
			return v;
		else
			throw new ParseException("Unrecognized Cvalue: " + x);
	}
}
