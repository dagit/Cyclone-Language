package cycinf.cvars.usedtype;

import general.util.Validator;

import java.text.MessageFormat;

import cycinf.util.*;

public class ConstructorArgumentList extends UsedTypeList {
	private final Identifier datatype;
	public final Identifier getDatatype() {
		return datatype;
	}
	private final Identifier constructor;
	public final Identifier getConstructor() {
		return constructor;
	}

	public ConstructorArgumentList(Identifier datatype, Identifier constructor) {
		this.datatype = Validator.nonnull(datatype, "datatype");
		this.constructor = Validator.nonnull(constructor, "constructor");
	}

	private static final String l1 = "DD", l2 = "_DF";

	@Override
	public final String fieldLocatorStringBase() {
		return l1 + getDatatype().bundle() + l2
			+ getConstructor().bundle() + "_";
	}

	private static final MessageFormat prettyFormat = new MessageFormat(
		"arg. #{0} to {1}.{2}");
	@Override
	public final String prettyStringForVariable(int varIndex) {
		return prettyFormat.format(new Object[] {varIndex + 1,
			getDatatype().getName(), getConstructor().getName()});
	}

	public static final ConstructorArgumentList parseBase1(String x)
		throws ParseException {
		String x1 = ParsingUtils.chopSuffix(x, "_");
		if (x1 == null)
			return null;
		String x2 = ParsingUtils.chopIdentifierSuffix(x1);
		if (x2 == null)
			return null;
		Identifier constructor = ParsingUtils.ival;
		String x3 = ParsingUtils.chopSuffix(x2, l2);
		if (x3 == null)
			return null;
		String x4 = ParsingUtils.chopIdentifierSuffix(x3);
		if (x4 == null)
			throw new ParseException("ConstructorArgumentList missing datatype name: " + x);
		Identifier datatype = ParsingUtils.ival;
		if (!x4.equals(l1))
			throw new ParseException("ConstructorArgumentList should have exactly " + l1 + " in front: " + x);
		return new ConstructorArgumentList(datatype, constructor);
	}
}
