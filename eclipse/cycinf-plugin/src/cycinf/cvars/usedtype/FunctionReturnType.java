package cycinf.cvars.usedtype;

import general.util.Validator;

import java.text.MessageFormat;

import cycinf.util.*;

public class FunctionReturnType extends UsedType {
	private final UsedTypeLayer function;
	public final UsedTypeLayer getFunction() {
		return function;
	}

	public FunctionReturnType(UsedTypeLayer function) {
		this.function = Validator.nonnull(function, "function");
	}

	private static final String l1 = "_FR";

	@Override
	public final String toString() {
		return getFunction().toString() + l1;
	}

	private static final MessageFormat prettyFormat = new MessageFormat(
		"return type of function {0}");
	@Override
	public final String prettyString() {
		return prettyFormat.format(new Object[] {getFunction()
			.prettyString()});
	}

	public static FunctionReturnType parse1(String x) throws ParseException {
		String functionStr = ParsingUtils.chopSuffix(x, l1);
		if (functionStr == null)
			return null;
		UsedTypeLayer function = UsedTypeLayer.parse(functionStr);
		return new FunctionReturnType(function);
	}
}
