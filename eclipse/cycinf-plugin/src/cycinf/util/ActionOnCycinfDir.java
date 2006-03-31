package cycinf.util;

import org.eclipse.core.resources.IContainer;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.*;
import org.eclipse.ui.*;

import cycinf.infstats.*;

public abstract class ActionOnCycinfDir implements IObjectActionDelegate {
	protected InfstatsManager manager = InfstatsManager.getManager();
	// private IWorkbenchPart navigator;
	protected DirTracker dir;
	public abstract void run(IAction a);
	public void selectionChanged(IAction a, ISelection s) {
		Object o;
		if (!s.isEmpty()
			&& (o = ((StructuredSelection) s).getFirstElement()) instanceof IContainer)
			dir = manager.getTrackerFor((IContainer) o);
	}
	public void setActivePart(IAction a, IWorkbenchPart wp) {
	// navigator = wp;
	}
}
