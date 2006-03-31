package cycinf.cvars.usedtype;

import general.util.Validator;

import java.util.Arrays;

import cycinf.cvars.CvarPiece;
import cycinf.util.*;


public class UsedTypeLayer extends CvarPiece {
	private final UsedType type;
	public final UsedType getType() {
		return type;
	}
	private final int depth;
	public final int getDepth() {
		return depth;
	}

	public UsedTypeLayer(UsedType type, int depth) {
		this.type = Validator.nonnull(type, "type");
		this.depth = Validator.nonnegative(depth, "depth");
	}

	@Override
	public final String toString() {
		return getType().toString() + "_"
			+ Integer.toString(getDepth());
	}

	@Override
	public final String prettyString() {
		if (depth == 0)
			return type.prettyString();
		else {
			char[] stars = new char[depth];
			Arrays.fill(stars, '*');
			return String.valueOf(stars) + "(" +
				type.prettyString() + ")";
		}
	}

	public static UsedTypeLayer parse(String x) throws ParseException {
		String x1 = ParsingUtils.chopNumberSuffix(x);
		if (x1 == null)
			throw new ParseException(
				"UsedTypeLayer expecting a number: " + x);
		int depth = ParsingUtils.nval;
		String typeStr = ParsingUtils.chopSuffix(x1, "_");
		if (typeStr == null)
			throw new ParseException(
				"UsedTypeLayer expecting an underscore: " + x);
		UsedType type = UsedType.parse(typeStr);
		return new UsedTypeLayer(type, depth);
	}
}
