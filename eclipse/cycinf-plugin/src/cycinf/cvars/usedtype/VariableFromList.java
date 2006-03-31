package cycinf.cvars.usedtype;

import general.util.Validator;
import cycinf.util.*;

public class VariableFromList extends UsedType {
	private final UsedTypeList list;
	public final UsedTypeList getList() {
		return list;
	}
	private final int index;
	public final int getIndex() {
		return index;
	}

	public VariableFromList(UsedTypeList list, int index) {
		this.list = Validator.nonnull(list, "list");
		this.index = Validator.nonnegative(index, "index");
	}

	@Override
	public String toString() {
		return getList().fieldLocatorStringBase()
			+ Integer.toString(getIndex());
	}

	@Override
	public String prettyString() {
		return getList().prettyStringForVariable(getIndex());
	}

	public static final VariableFromList parse1(String x)
		throws ParseException {
		String listStr = ParsingUtils.chopNumberSuffix(x);
		if (listStr == null)
			return null;
		int index = ParsingUtils.nval;
		UsedTypeList list = UsedTypeList.parseBase(listStr);
		if (list == null)
			return null;
		return new VariableFromList(list, index);
	}
}
