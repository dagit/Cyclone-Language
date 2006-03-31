package cycinf.reasoning;

import cycinf.constraints.GuardedConstraint;

public class RGuardedConstraint extends RConstraint<GuardedConstraint> {
	final RGuard<?> ifPart;
	public final RGuard<?> getIfPart() {
		return ifPart;
	}
	final RConstraint<?> thenPart;
	public final RConstraint<?> getThenPart() {
		return thenPart;
	}

	public RGuardedConstraint(RContext<RConstraint<?>> context, GuardedConstraint back, boolean effective) {
		super(context, back, effective);
		registerThis(this);
		nodeDesc = "Guarded constraint";
		ifPart = RCNodeFactory.wrap(new RContext1<RGuardedConstraint, RGuard<?>>(this, "If"), back.getIfPart());
		thenPart = RCNodeFactory.wrap(new RContext1<RGuardedConstraint, RConstraint<?>>(this, "Then"), back.getThenPart(), getIfPart().getRealValue());
	}
}
