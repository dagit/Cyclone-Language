package cycinf.reasoning;

import cycinf.constraints.*;

class RCNodeFactory {
	static RConstraint<?> wrap(RContext<RConstraint<?>> role, Constraint c, boolean effective) {
		if (c instanceof AssignmentConstraint)
			return new RAssignmentConstraint(role, (AssignmentConstraint) c, effective);
		else if (c instanceof GuardedConstraint)
			return new RGuardedConstraint(role, (GuardedConstraint) c, effective);
		else
			throw new IllegalArgumentException(
				"I don't know how to wrap this kind of Constraint: "
					+ c.getClass());
	}
	static RGuard<?> wrap(RContext<RGuard<?>> role, Guard g) {
		if (g instanceof ComparisonGuard)
			return new RComparisonGuard(role, (ComparisonGuard) g);
		else
			throw new IllegalArgumentException(
				"I don't know how to wrap this kind of Guard: "
					+ g.getClass());
	}
	static RCvalueExpression<?> wrap(RContext<RCvalueExpression<?>> role, CvalueExpression e) {
		if (e instanceof CvalueRef)
			return new RCvalueRef(role, (CvalueRef) e);
		else if (e instanceof CvarRef)
			return new RCvarRef(role, (CvarRef) e);
		else
			throw new IllegalArgumentException(
				"I don't know how to wrap this kind of CvalueExpression: "
				+ e.getClass());
	}
}
