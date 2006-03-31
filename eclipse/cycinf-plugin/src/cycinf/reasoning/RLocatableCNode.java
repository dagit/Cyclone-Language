package cycinf.reasoning;

import cycinf.constraints.LocatableCNode;
import cycinf.util.SourceLocation;

public abstract class RLocatableCNode<B extends LocatableCNode, RoleThis extends RLocatableCNode<?, RoleThis>>
	extends RCNode<B, RoleThis> {
	final RLocationNode locationNode;
	public final RLocationNode getLocationNode() {
		return locationNode;
	}

	RLocatableCNode(RContext<RoleThis> role, B back) {
		super(role, back);
		SourceLocation loc = back.getLocationStr();
		if (loc == null)
			locationNode = null;
		else
			locationNode = new RLocationNode(
				new RContext1<RLocatableCNode<B, RoleThis>, RLocationNode>(this, "Location"),
				loc);
	}
}
