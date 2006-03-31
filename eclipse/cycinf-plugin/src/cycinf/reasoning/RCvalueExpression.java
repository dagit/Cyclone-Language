package cycinf.reasoning;

import cycinf.constraints.*;
import cycinf.cvars.Cvalue;

public abstract class RCvalueExpression<B extends CvalueExpression> extends RCNode<B, RCvalueExpression<?>> {
	Cvalue realValue; // subclasses shall set
	public final Cvalue getRealValue() { return realValue; }
	
	RContext<RSimpleStringNode> valueContext = null;
	RSimpleStringNode valueNode;
	
	public RCvalueExpression(RContext<RCvalueExpression<?>> context, B back) {
		super(context, back);
		nodeDesc = back.prettyString();
		// Get the value context in the list now; go back and add the node later
		if (!(back instanceof CvalueRef))
			valueContext = new RContext1<RCvalueExpression<B>, RSimpleStringNode>(this, "Value");
	}
	
	// concrete subclasses shall call this during construction
	void registerValue(@SuppressWarnings("hiding") Cvalue realValue) {
		this.realValue = realValue;
		if (valueContext != null)
			valueNode = new RSimpleStringNode(valueContext, realValue.prettyString());
	}
}
