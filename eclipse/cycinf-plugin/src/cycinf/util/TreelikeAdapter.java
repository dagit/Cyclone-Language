package cycinf.util;

public abstract class TreelikeAdapter<T extends Treelike<T>, P extends T, C extends T> implements Treelike<T> {
	protected P parent;
	protected C[] children;
	
	protected TreelikeAdapter(P parent) {
		this.parent = parent;
	}

	public abstract String getTreeLabel();

	public P getTreeParent() {
		return parent;
	}

	protected abstract C[] computeTreeChildren();

	public C[] getTreeChildren() {
		if (children == null)
			children = computeTreeChildren();
		return children;
	}
}
