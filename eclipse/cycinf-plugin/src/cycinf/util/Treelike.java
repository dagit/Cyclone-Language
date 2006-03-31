package cycinf.util;

public interface Treelike<N extends Treelike<N>> {
	public abstract String getTreeLabel();
	public abstract N getTreeParent();
	// The caller mustn't modify the returned array
	public abstract N[] getTreeChildren();
}
