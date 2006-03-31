package cycinf.constraints;

import cycinf.util.*;

public abstract class LocatableCNode extends CNode {
	private final SourceLocation location;
	public final SourceLocation getLocationStr() {
		return location;
	}
	protected LocatableCNode(SourceLocation location) {
		this.location = location; // nullable
	}

	public abstract String toString1();

	@Override
	public final String toString() {
		return ParsingUtils.escape(location == null ? "" : location.toString()) + toString1();
	}
}
