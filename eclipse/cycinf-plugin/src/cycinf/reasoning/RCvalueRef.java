package cycinf.reasoning;

import cycinf.constraints.CvalueRef;

public class RCvalueRef extends RCvalueExpression<CvalueRef> {
	public RCvalueRef(RContext<RCvalueExpression<?>> context, CvalueRef back) {
		super(context, back);
		registerThis(this);
		registerValue(back.evaluate(context.getOwningInfstats()));
	}
}
