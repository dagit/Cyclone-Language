package cycinf.reasoning;

import static general.util.Validator.nonnull;
import general.util.GenericNonsense;

import java.util.*;

import cycinf.infstats.*;
import cycinf.util.Treelike;

//Any node in a wrapped constraint: mainly RCNodes, also locations and actual
//values
public abstract class RNode<RoleThis extends RNode<? extends RoleThis>> implements
Treelike<RNode<?>>, BelongsToInfstats {
	
	final RContext<RoleThis> context;
	public final RContext<RoleThis> getContext() {
		return context;
	}
	
	// Convenience
	public final DirInfstats getOwningInfstats() {
		return context.getOwningInfstats();
	}
	
	// concrete subclasses shall set this during construction
	String nodeDesc; 
	public final String getNodeDesc() {
		return nodeDesc;
	}
	
	// this gets filled automatically as roles are created
	final List<RContext1<RoleThis, ?>> childRoles = new ArrayList<RContext1<RoleThis, ?>>();
	// we calculate this the first time tree children are requested
	RNode<?>[] treeChildren = null;
	
	RNode(RContext<RoleThis> context) {
		this.context = nonnull(context);
		// All role-concrete subclasses shall run: context.node = this;
	}
	
	// concrete subclasses shall call this during construction
	void registerThis(RoleThis this0) {
		if (this == this0)
			getContext().node = this0;
		else
			throw new IllegalArgumentException("You're supposed to pass this, silly!  (But as a RoleThis.)");
	}
	
	public final String getTreeLabel() {
		String prefix;
		if (getContext() instanceof RContext1) {
			RContext1<?, RoleThis> context1 = (RContext1<?, RoleThis>) getContext();
			prefix = (context1.getName() == null ? null : context1.getName()) + ": ";
		} else
			prefix = "";
		return prefix + getNodeDesc();
	}
	public final RNode<?> getTreeParent() {
		return (getContext() instanceof RContext1)
		? ((RContext1<?, RoleThis>) getContext()).getParent()
			: null;
	}
	public final RNode<?>[] getTreeChildren() {
		if (treeChildren == null) {
			treeChildren = new RNode<?>[childRoles.size()];
			for (int i = 0; i < childRoles.size(); i++) {
				treeChildren[i] = childRoles.get(i).getNode();
			}
		}
		return treeChildren;
	}
	
	public final RConstraintTop findConstraintTop() {
		RNode<?> n = this;
		while (!(n.getContext() instanceof RConstraintTop))
			n = n.getTreeParent();
		return GenericNonsense.workaroundCast(RConstraintTop.class, n.getContext());
	}
}
