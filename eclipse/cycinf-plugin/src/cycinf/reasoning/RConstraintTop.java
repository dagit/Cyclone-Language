package cycinf.reasoning;

import cycinf.constraints.Constraint;
import cycinf.infstats.DirInfstats;

// Ugly workaround for not being able to runtime cast RNode<?> to RConstraint<?>
public class RConstraintTop extends RContext<RConstraint<?>> {
	RConstraintTop(DirInfstats owningInfstats) {
		super(owningInfstats);
	}
	
	public RConstraintTop(DirInfstats owningInfstats, Constraint c) {
		this(owningInfstats);
		RCNodeFactory.wrap(this, c, true);
		// ignore the result: it goes into this.node
	}
}
