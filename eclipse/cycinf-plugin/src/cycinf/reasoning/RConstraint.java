package cycinf.reasoning;

import cycinf.constraints.Constraint;
import cycinf.cvars.Cvar;

public abstract class RConstraint<B extends Constraint> extends RLocatableCNode<B, RConstraint<?>> {
	final boolean effective;
	public final boolean isEffective() {
		return effective;
	}
	// Returns the cvar constrained by _this_ constraint node (not children), if any.
	// Certain subclasses should override.
	public Cvar getConstrainedVar() {
		return null;
	}
	// When getConstrainedVar returns a Cvar, this must return a meaningful string for the Reasoning view
	public String describeEffectOnVar() {
		return null;
	}
	
	final RSimpleStringNode effectiveNode;
	public final RSimpleStringNode getEffectiveNode() {
		return effectiveNode;
	}

	RConstraint(RContext<RConstraint<?>> role, B back, boolean effective) {
		super(role, back);
		this.effective = effective;
		effectiveNode = new RSimpleStringNode(new RContext1<RConstraint<B>, RSimpleStringNode>(this, "In effect"), effective ? "yes" : "no");
	}
}
