package cycinf.cvars.usedtype;

import general.util.Validator;
import cycinf.cvars.CvarPiece;
import cycinf.util.*;

public final class Identifier extends CvarPiece {
	private final String name;
	public final String getName() {
		return name;
	}

	public Identifier(String name) throws ParseException {
		this.name = ParsingUtils.validIdentifier(Validator.nonnull(
			name, "name"));
	}

	@Override
	public final String toString() {
		return bundle();
	}

	@Override
	public final String prettyString() {
		return getName();
	}

	public final String bundle() {
		return Integer.toString(getName().length()) + "I" + getName();
	}

	public static Identifier unwrap(String x) throws ParseException {
		if (x.lastIndexOf('(') == 0 && x.indexOf(')') == x.length() - 1)
			return new Identifier(x.substring(1, x.length() - 1));
		else
			throw new ParseException("Expected identifier: " + x);
	}
}
