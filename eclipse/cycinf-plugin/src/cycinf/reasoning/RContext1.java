package cycinf.reasoning;

import static general.util.Validator.nonnull;

// Just create instances of RRole with appropriate types and string. Don't
// bother to extend.
// And RoleN should be a general type (RConstraint, RCvalueExpression), not an
// alternative (RImplicationConstraint, RCvarRef).
// Eclipse should be warning us right about now that RoleN is unused...
public class RContext1<P extends RNode<?>, RoleN extends RNode<? extends RoleN>> extends
	RContext<RoleN> {
	final P parent;
	public final P getParent() {
		return parent;
	}
	// ...but this name is only legal for a certain node type RoleN
	final String name;
	public final String getName() {
		return name;
	}

	RContext1(P parent, String name) {
		super(nonnull(parent).getContext().getOwningInfstats());
		this.parent = nonnull(parent);
		this.name = nonnull(name);
		this.parent.childRoles.add(this);
	}
}
