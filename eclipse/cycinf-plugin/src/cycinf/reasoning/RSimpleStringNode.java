package cycinf.reasoning;

public class RSimpleStringNode extends RNode<RSimpleStringNode> {
	public RSimpleStringNode(RContext<RSimpleStringNode> context, String nodeDesc) {
		super(context);
		registerThis(this);
		this.nodeDesc = nodeDesc;
	}
}
