package cycinf.cvars.usedtype;

import general.util.Validator;

import java.text.MessageFormat;

import cycinf.util.*;

public class ConstructorTempArgList extends UsedTypeList {
	private final UsedTypeLayer context;
	public final UsedTypeLayer getContext() {
		return context;
	}
	
	private final Identifier datatypeName;
	public final Identifier getDatatypeName() {
		return datatypeName;
	}

	private final Identifier constructorName;
	public final Identifier getConstructorName() {
		return constructorName;
	}

	public ConstructorTempArgList(UsedTypeLayer context,
		Identifier datatypeName, Identifier constructorName) {
		this.context = Validator.nonnull(context, "context");
		this.datatypeName = Validator.nonnull(datatypeName, "datatypeName");
		this.constructorName = Validator.nonnull(constructorName, "constructorName");
	}

	private static final String l1 = "_DFARG", l2 = "DD", l3 = "DF";

	@Override
	public final String fieldLocatorStringBase() {
		return getContext().toString() + l2 +
			getDatatypeName().toString() + l3 +
			getConstructorName().toString() + l1;
	}

	private static final MessageFormat prettyFormat = new MessageFormat(
		"template arg. #{0} to {1}.{2} in {3}");
	@Override
	public final String prettyStringForVariable(int index) {
		return prettyFormat.format(new Object[] {index + 1,
			getDatatypeName().prettyString(),
			getConstructorName().prettyString(),
			getContext().prettyString()});
	}

	public static final ConstructorTempArgList parseBase1(String x)
		throws ParseException {
		String x1 = ParsingUtils.chopSuffix(x, l1);
		if (x1 == null)
			return null;
		String x2 = ParsingUtils.chopIdentifierSuffix(x1);
		if (x2 == null)
			throw new ParseException("DatatypeTempArgList expecting the name of the constructor: " + x);
		Identifier constructorName = ParsingUtils.ival;
		String x3 = ParsingUtils.chopSuffix(x2, l3);
		if (x3 == null)
			throw new ParseException("DatatypeTempArgList expecting " + l3 + ": " + x);
		String x4 = ParsingUtils.chopIdentifierSuffix(x3);
		if (x4 == null)
			throw new ParseException("DatatypeTempArgList expecting the name of the datatype: " + x);
		Identifier datatypeName = ParsingUtils.ival;
		String x5 = ParsingUtils.chopSuffix(x4, l2);
		if (x5 == null)
			throw new ParseException("DatatypeTempArgList expecting " + l2 + ": " + x);
		UsedTypeLayer context = UsedTypeLayer.parse(x5);
		return new ConstructorTempArgList(context, datatypeName, constructorName);
	}
}
