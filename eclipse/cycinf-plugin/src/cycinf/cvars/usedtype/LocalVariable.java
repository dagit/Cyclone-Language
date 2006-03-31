package cycinf.cvars.usedtype;

import general.util.Validator;

import java.text.MessageFormat;

import cycinf.util.*;

public class LocalVariable extends UsedType {
	private final UsedType function;
	public final UsedType getFunction() {
		return function;
	}
	private final Identifier localName;
	public final Identifier getLocalName() {
		return localName;
	}
	private final int index;
	public final int getIndex() {
		return index;
	}

	public LocalVariable(UsedType function, Identifier localName,
		int index) {
		this.function = Validator.nonnull(function, "function");
		this.localName = Validator.nonnull(localName, "localName");
		this.index = Validator.nonnegative(index, "index");
	}

	@Override
	public final String toString() {
		return function.toString() + "_" + localName.toString() + "_"
			+ Integer.toString(index);
	}

	private static final MessageFormat prettyFormat = new MessageFormat(
		"{0} in {1}");
	@Override
	public final String prettyString() {
		return prettyFormat
			.format(new Object[] {getLocalName().getName(),
				getFunction().prettyString()});
	}

	public static final LocalVariable parse1(String x)
		throws ParseException {
		String x1 = ParsingUtils.chopNumberSuffix(x);
		if (x1 == null)
			return null;
		int index = ParsingUtils.nval;
		String x2 = ParsingUtils.chopSuffix(x1, "_");
		if (x2 == null)
			return null;
		String x3 = ParsingUtils.chopIdentifierSuffix(x2);
		if (x3 == null)
			return null;
		String x4 = ParsingUtils.chopSuffix(x3, "_");
		if (x4 == null)
			return null;
		Identifier localName = ParsingUtils.ival;
		UsedType function = UsedType.parse(x4);
		return new LocalVariable(function, localName, index);
	}

}
