package cycinf.cvars.ptrbnd;

import general.util.Validator;

import java.text.MessageFormat;

import cycinf.cvars.Cvar;
import cycinf.cvars.usedtype.UsedTypeLayer;
import cycinf.util.*;

public class PointerKindCvar extends Cvar {
	private final UsedTypeLayer pointer;
	public final UsedTypeLayer getPointer() {
		return pointer;
	}

	public PointerKindCvar(UsedTypeLayer pointer) {
		this.pointer = Validator.nonnull(pointer, "pointer");
	}

	private static final String l1 = "C_PTRBND_";

	@Override
	public final String toString() {
		return l1 + getPointer().toString();
	}

	private static final MessageFormat prettyFormat = new MessageFormat(
		"pointer kind of {0}");
	@Override
	public final String prettyString() {
		return prettyFormat.format(new Object[] {getPointer()
			.prettyString()});
	}

	public static final PointerKindCvar parse1(String x)
		throws ParseException {
		if (x.startsWith(l1)) {
			String preprocessed = ParsingUtils.preprocess(x
				.substring(l1.length()));
			UsedTypeLayer pointer = UsedTypeLayer
				.parse(preprocessed);
			return new PointerKindCvar(pointer);
		} else
			return null;
	}

	@Override
	public String targetPrettyString() {
		return pointer.prettyString();
	}
}
