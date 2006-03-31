package cycinf.reasoning;

import cycinf.constraints.Guard;

public abstract class RGuard<B extends Guard> extends RLocatableCNode<B, RGuard<?>> {
	boolean realValue;
	public final boolean getRealValue() { return realValue; }
	
	RContext<RSimpleStringNode> valueContext;
	RSimpleStringNode valueNode;
	
	public RGuard(RContext<RGuard<?>> context, B back) {
		super(context, back);
		// Get the value context in the list now; go back and add the node later 
		valueContext = new RContext1<RGuard<B>, RSimpleStringNode>(this, "Satisfied");
	}
	
	// concrete subclasses shall call this during construction
	void registerValue(@SuppressWarnings("hiding") boolean value) {
		this.realValue = value;
		valueNode = new RSimpleStringNode(valueContext, value ? "yes" : "no");
	}
}
