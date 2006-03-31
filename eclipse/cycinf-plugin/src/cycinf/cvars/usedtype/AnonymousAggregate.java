package cycinf.cvars.usedtype;

import general.util.Validator;

import java.text.MessageFormat;

import cycinf.util.*;

public class AnonymousAggregate extends UsedTypeList {
	private final UsedTypeLayer context;
	public final UsedTypeLayer getContext() {
		return context;
	}

	public AnonymousAggregate(UsedTypeLayer context) {
		this.context = Validator.nonnull(context, "context");
	}

	@Override
	public final String fieldLocatorStringBase() {
		return context.toString() + "_AA";
	}

	private static final MessageFormat prettyFormat = new MessageFormat(
		"field #{0} of {1}");
	@Override
	public final String prettyStringForVariable(int index) {
		return prettyFormat.format(new Object[] {index + 1,
			context.prettyString()});
	}

	public static final AnonymousAggregate parseBase1(String x)
		throws ParseException {
		String contextStr = ParsingUtils.chopSuffix(x, "_AA");
		if (contextStr == null)
			return null;
		UsedTypeLayer context = UsedTypeLayer.parse(contextStr);
		return new AnonymousAggregate(context);
	}
}
