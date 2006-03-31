package cycinf.util;

import org.eclipse.core.resources.IMarker;

import cycinf.cvars.Decision;
import cycinf.reasoning.*;
import cycinf.ui.ReasoningView;

public abstract class Interchange {
	private Interchange() {}
	
	public static Decision toDecision(Object o) {
		return (o instanceof Decision) ? (Decision) o
			: (o instanceof RCvarRef) ? ((RCvarRef) o).getDecisionOnTheCvar()
			: (o instanceof ReasoningView.DecisionNode) ? ((ReasoningView.DecisionNode) o).getDecision()
			: (o instanceof IMarker) ? Decision.lookupMarker((IMarker) o)
			: null;
	}
	
	public static RNode<?> toRNode(Object o) {
		return (o instanceof RNode) ? (RNode<?>) o 
			: (o instanceof RConstraintTop) ? ((RConstraintTop) o).getNode()
			: (o instanceof ReasoningView.ConstraintNode) ? ((ReasoningView.ConstraintNode) o).getConstraint()
			: (o instanceof IMarker) ? RLocationNode.lookupMarker((IMarker) o)
			: null;
	}
	
	public static RConstraintTop toRConstraintTop(Object o) {
		RNode<?> n = toRNode(o);
		return n == null ? null : n.findConstraintTop();
	}
}
