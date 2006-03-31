package cycinf.cvars.usedtype;

import general.util.Validator;

import java.text.MessageFormat;

import cycinf.util.*;

public class NamedAggregate extends UsedTypeList {
	private final Identifier name;
	public final Identifier getName() {
		return name;
	}
	private final Typedef typedef;
	public final Typedef getTypedef() {
		return typedef;
	}

	public NamedAggregate(Identifier name, Typedef typedef) {
		this.name = Validator.nonnull(name, "name");
		this.typedef = typedef; // null allowed
	}

	private static final String l1 = "AD", l2 = "_AF";

	@Override
	public final String fieldLocatorStringBase() {
		return l1 + getName().bundle() + l2;
	}

	private static final MessageFormat prettyFormat = new MessageFormat(
		"field #{0} of {1}");
	private static final MessageFormat prettyFormat2 = new MessageFormat(
		"field #{0} of {1} typedef-ed as {2}");
	@Override
	public final String prettyStringForVariable(int index) {
		return typedef == null ? prettyFormat.format(new Object[] {
			index + 1, getName().getName()}) : prettyFormat2
			.format(new Object[] {index + 1, getName().getName(),
				typedef.getName().getName()});
	}

	public static final NamedAggregate parseBase1(String x)
		throws ParseException {
		String x1 = ParsingUtils.chopSuffix(x, l2);
		if (x1 == null)
			return null;
		String x2 = ParsingUtils.chopIdentifierSuffix(x1);
		if (x2 == null)
			throw new ParseException(
				"NamedAggregate expected identifier: " + x);
		Identifier name = ParsingUtils.ival;
		String x3 = ParsingUtils.chopSuffix(x2, l1);
		if (x3 == null)
			throw new ParseException("NamedAggregate expected "
				+ l1 + ": " + x);
		Typedef typedef;
		if (x3.equals(""))
			typedef = null;
		else {
			typedef = Typedef.parse1(x3);
			if (typedef == null)
				throw new ParseException(
					"NamedAggregate expected a typedef: "
						+ x);
		}
		return new NamedAggregate(name, typedef);
	}
}
