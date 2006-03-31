package cycinf.cvars.usedtype;

import general.util.Validator;

import java.text.MessageFormat;

import cycinf.util.*;

public class FunctionArgumentList extends UsedTypeList {
	private final UsedTypeLayer function;
	public final UsedTypeLayer getFunction() {
		return function;
	}

	public FunctionArgumentList(UsedTypeLayer function) {
		this.function = Validator.nonnull(function, "function");
	}

	private static final String l1 = "_ARG";

	@Override
	public final String fieldLocatorStringBase() {
		return getFunction().toString() + l1;
	}

	private static final MessageFormat prettyFormat = new MessageFormat(
		"arg. #{0} to {1}");
	@Override
	public final String prettyStringForVariable(int index) {
		return prettyFormat.format(new Object[] {index + 1,
			getFunction().prettyString()});
	}

	public static final FunctionArgumentList parseBase1(String x)
		throws ParseException {
		String functionStr = ParsingUtils.chopSuffix(x, l1);
		if (functionStr == null)
			return null;
		UsedTypeLayer function = UsedTypeLayer.parse(functionStr);
		return new FunctionArgumentList(function);
	}
}
