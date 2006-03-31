package cycinf.reasoning;

import cycinf.constraints.CvarRef;
import cycinf.cvars.Decision;

public class RCvarRef extends RCvalueExpression<CvarRef> {
	public RCvarRef(RContext<RCvalueExpression<?>> context, CvarRef back) {
		super(context, back);
		registerThis(this);
		registerValue(back.evaluate(context.getOwningInfstats()));
	}
	
	public Decision getDecisionOnTheCvar() {
		return getContext().getOwningInfstats().getDecisionOnCvar(getBack().getCvar());
	}
}
