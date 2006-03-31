package cycinf.cvars.usedtype;

import general.util.Validator;

import java.text.MessageFormat;

import cycinf.util.*;

public class Typedef extends UsedType {
	private final Identifier name;
	public final Identifier getName() {
		return name;
	}

	public Typedef(Identifier name) {
		this.name = Validator.nonnull(name, "name");
	}

	private static final String l1 = "TD";

	@Override
	public String toString() {
		return l1 + getName().bundle();
	}

	private static final MessageFormat prettyFormat = new MessageFormat(
		"{0}");
	@Override
	public final String prettyString() {
		return prettyFormat.format(new Object[] {getName().getName()});
	}

	public static final Typedef parse1(String x) throws ParseException {
		String name = ParsingUtils.chopPrefix(x, l1);
		if (name == null)
			return null;
		return new Typedef(Identifier.unwrap(name));
	}
}
