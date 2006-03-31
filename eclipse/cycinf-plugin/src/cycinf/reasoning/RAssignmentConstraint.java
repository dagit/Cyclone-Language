package cycinf.reasoning;

import general.util.GenericNonsense;
import cycinf.constraints.*;
import cycinf.cvars.Cvar;

public class RAssignmentConstraint extends RConstraint<AssignmentConstraint> {
	final RCvalueExpression<?> targetValue;
	public final RCvalueExpression<?> getTargetValue() {
		return targetValue;
	}
	final RCvalueExpression<?> sourceValue;
	public final RCvalueExpression<?> getSourceValue() {
		return sourceValue;
	}

	public RAssignmentConstraint(RContext<RConstraint<?>> context, AssignmentConstraint back, boolean effective) {
		super(context, back, effective);
		registerThis(this);
		nodeDesc = "Assignment constraint";
		targetValue = RCNodeFactory.wrap(new RContext1<RAssignmentConstraint, RCvalueExpression<?>>(this, "Set"), back.getTargetValue());
		sourceValue = RCNodeFactory.wrap(new RContext1<RAssignmentConstraint, RCvalueExpression<?>>(this, "To"), back.getSourceValue());
	}

	@Override
	public Cvar getConstrainedVar() {
		return (getTargetValue() instanceof RCvarRef)
			? GenericNonsense.workaroundCast(RCvarRef.class, getTargetValue()).getBack().getCvar() : null;
	}

	@Override
	public String describeEffectOnVar() {
		CvalueExpression src2 = getSourceValue().getBack();
		if (src2 instanceof CvarRef)
			return "same as " + ((CvarRef) src2).getCvar().targetPrettyString();
		else
			return "= " + getSourceValue().getBack().toString();
	}
	
}
