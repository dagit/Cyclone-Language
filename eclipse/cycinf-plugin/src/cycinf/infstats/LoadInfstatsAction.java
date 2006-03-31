package cycinf.infstats;

import org.eclipse.jface.action.IAction;

import cycinf.util.ActionOnCycinfDir;

public final class LoadInfstatsAction extends ActionOnCycinfDir {
	@Override
	public void run(IAction a) {
		dir.asyncLoadInfstatsNow(false);
	}
}
