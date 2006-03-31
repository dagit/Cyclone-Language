package cycinf.cvars.usedtype;

import cycinf.util.ParseException;

public abstract class UsedTypeList {
	public abstract String fieldLocatorStringBase();

	public abstract String prettyStringForVariable(int index);

	public static final UsedTypeList parseBase(String x)
		throws ParseException {
		UsedTypeList l;
		if ((l = NamedAggregate.parseBase1(x)) != null
			|| (l = AnonymousAggregate.parseBase1(x)) != null
			|| (l = FunctionArgumentList.parseBase1(x)) != null
			|| (l = ConstructorArgumentList.parseBase1(x)) != null
			|| (l = TypedefTempArgList.parseBase1(x)) != null
			|| (l = NamedAggregateTempArgList.parseBase1(x)) != null
			|| (l = DatatypeTempArgList.parseBase1(x)) != null
			|| (l = ConstructorTempArgList.parseBase1(x)) != null)
			return l;
		else
			return null;
	}
}
