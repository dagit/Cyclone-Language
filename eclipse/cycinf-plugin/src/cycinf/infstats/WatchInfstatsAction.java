package cycinf.infstats;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.*;
import org.eclipse.jface.viewers.ISelection;

import cycinf.CycinfPlugin;
import cycinf.util.ActionOnCycinfDir;

public final class WatchInfstatsAction extends ActionOnCycinfDir {
	@Override
	public void run(IAction a) {
		boolean newWatched = !dir.isWatched();
		dir.setWatched(newWatched);
		check(a);
		if (newWatched)
			CycinfPlugin.message("Please note that this setting " +
				"is not yet kept between runs of Eclipse, so " +
				"if you wish to continue watching inference " +
				"data after restarting Eclipse, you must " +
				"make the setting again.",
				MessageDialog.INFORMATION);
	}
	private void check(IAction a) {
		if (dir != null)
			a.setChecked(dir.isWatched());
	}
	@Override
	public void selectionChanged(IAction a, ISelection s) {
		super.selectionChanged(a, s);
		check(a);
	}
}
