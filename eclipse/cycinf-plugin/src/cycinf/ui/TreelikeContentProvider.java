package cycinf.ui;

import org.eclipse.jface.viewers.*;

import cycinf.util.Treelike;

public abstract class TreelikeContentProvider implements ITreeContentProvider {
	public TreelikeContentProvider() {}
	
	public Treelike<?> getParent(Object child) {
		return getParent((Treelike<?>) child);
	}
	public Treelike<?> getParent(Treelike<?> child) {
		return child.getTreeParent();
	}
	public Treelike<?>[] getChildren(Object parent) {
		return getChildren((Treelike<?>) parent);
	}
	public Treelike<?>[] getChildren(Treelike<?> parent) {
		return parent.getTreeChildren();
	}
	public boolean hasChildren(Object parent) {
		return hasChildren((Treelike<?>) parent);
	}
	public boolean hasChildren(Treelike<?> parent) {
		return parent.getTreeChildren().length > 0;
	}
}
