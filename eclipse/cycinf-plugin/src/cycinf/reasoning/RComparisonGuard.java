package cycinf.reasoning;

import cycinf.constraints.*;

public class RComparisonGuard extends RGuard<ComparisonGuard> {
	final RCvalueExpression<?> value1;
	public final RCvalueExpression<?> getValue1() {
		return value1;
	}
	final RCvalueExpression<?> value2;
	public final RCvalueExpression<?> getValue2() {
		return value2;
	}

	public RComparisonGuard(RContext<RGuard<?>> context, ComparisonGuard back) {
		super(context, back);
		registerThis(this);
		ComparisonOperator op = back.getOperator();
		switch (op) {
		case EQUAL:
			nodeDesc = "Equal?";
			break;
		default:
			throw new IllegalArgumentException("Unrecognized ComparisonOperator");
		}
		value1 = RCNodeFactory.wrap(new RContext1<RComparisonGuard, RCvalueExpression<?>>(this, "Expr 1"), back.getValue1());
		value2 = RCNodeFactory.wrap(new RContext1<RComparisonGuard, RCvalueExpression<?>>(this, "Expr 2"), back.getValue2());
		registerValue(getValue1().getRealValue().equals(getValue2().getRealValue()));
	}
}
