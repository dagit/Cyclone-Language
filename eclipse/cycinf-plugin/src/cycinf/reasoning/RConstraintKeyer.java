package cycinf.reasoning;

import general.util.Keyer;
import cycinf.constraints.Constraint;

public class RConstraintKeyer implements Keyer<Constraint, RConstraintTop> {
	public final Constraint getKeyFor(RConstraintTop ctop) {
		return ctop.getNode().getBack();
	}
}
