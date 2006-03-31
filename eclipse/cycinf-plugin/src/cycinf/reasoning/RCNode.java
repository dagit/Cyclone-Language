package cycinf.reasoning;
import cycinf.constraints.CNode;
// An RCNode is backed by a CNode; it isn't something extra like a location or
// actual value
public abstract class RCNode<B extends CNode, RoleThis extends RCNode<?, RoleThis>>
	extends RNode<RoleThis> {
	final B back;
	public final B getBack() {
		return back;
	}
	RCNode(RContext<RoleThis> role, B back) {
		super(role);
		this.back = back;
	}
}
