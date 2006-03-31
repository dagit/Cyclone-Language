package cycinf.ui;

import org.eclipse.jface.viewers.*;
import cycinf.util.Treelike;

public class TreelikeLabelProvider extends LabelProvider {
	@Override
	public String getText(Object element) {
		return ((Treelike<?>) element).getTreeLabel();
	}
}
