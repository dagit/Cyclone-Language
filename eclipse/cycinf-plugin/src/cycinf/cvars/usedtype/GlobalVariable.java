package cycinf.cvars.usedtype;

import general.util.Validator;

import java.text.MessageFormat;

import cycinf.util.*;

public class GlobalVariable extends UsedType {
	private final Identifier name;
	public final Identifier getName() {
		return name;
	}

	public GlobalVariable(Identifier name) {
		this.name = Validator.nonnull(name, "name");
	}

	@Override
	public final String toString() {
		return getName().toString();
	}

	private static final MessageFormat prettyFormat = new MessageFormat(
		"{0}");
	@Override
	public final String prettyString() {
		return prettyFormat.format(new Object[] {getName().getName()});
	}

	public static final GlobalVariable parse1(String x)
		throws ParseException {
		return new GlobalVariable(Identifier.unwrap(x));
	}
}
