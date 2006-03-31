package cycinf.infstats;

import java.util.Observable;

import org.eclipse.core.resources.*;
import org.eclipse.core.runtime.*;

import cycinf.cvars.Decision;
import cycinf.reasoning.RLocationNode;

public class DirTracker extends Observable {
	final IContainer dir;
	public IContainer getDir() {
		return dir;
	}
	final IFile cvarsFile;
	DirInfstats curInfstats = null;
	public synchronized DirInfstats getCurInfstats() {
		return curInfstats;
	}
	boolean watched = false;
	public synchronized boolean isWatched() {
		return watched;
	}
	boolean loadInProgress = false;
	public synchronized boolean isLoadInProgress() {
		return loadInProgress;
	}

	// Called by InfstatsManager
	DirTracker(IContainer dir) {
		this.dir = dir;
		cvarsFile = dir.getFile(Path.fromPortableString(".cych/cvars"));
		curInfstats = null;
	}

	public String getPath() {
		return dir.getFullPath().toPortableString();
	}
	public boolean infstatsPresent() {
		return cvarsFile.exists();
	}
	public long infstatsStamp() {
		return cvarsFile.getModificationStamp();
	}
	public synchronized void asyncLoadInfstatsNow(boolean automatic) {
		if (loadInProgress)
			return;
		loadInProgress = true;
		DirInfstats is = new DirInfstats(this);
		is.scheduleLoading(automatic);
	}
	public void syncLoadInfstatsNow(boolean automatic) {
		DirInfstats is;
		synchronized (this) {
			if (loadInProgress)
				return;
			loadInProgress = true;
			is = new DirInfstats(this);
			is.scheduleLoading(automatic);
		}
		is.waitUntilLoaded();
	}
	public synchronized boolean infstatsUpToDate() {
		long stamp = infstatsStamp();
		return (getCurInfstats() == null)
			? stamp == IResource.NULL_STAMP
			: stamp == getCurInfstats().getStamp();
	}
	public synchronized void asyncUpdateInfstats(boolean automatic) {
		if (!infstatsUpToDate())
			asyncLoadInfstatsNow(automatic);
	}
	public synchronized DirInfstats syncUpdateInfstats(boolean automatic) {
		// The loop should rarely run more than once, but strange
		// things could happen if cvars is being updated repeatedly as
		// we load.
		while (!infstatsUpToDate())
			syncLoadInfstatsNow(automatic);
		return getCurInfstats();
	}

	synchronized void apply(DirInfstats is, IProgressMonitor pm)
		throws CoreException {
		CoreException ce = null;
		pm.beginTask("Applying the new inference data to the workspace",
			is.getDecisions().size() + is.getConstraintLocations().size() + 1);
		DirInfstats oldIS = getCurInfstats();
		if (oldIS != null) {
			pm.subTask("Unmarking old decision locations");
			for (Decision d : oldIS.getDecisions())
				d.unmark();
			pm.subTask("Unmarking old constraint locations");
			for (RLocationNode n : oldIS.getConstraintLocations())
				n.unmark();
		}
		pm.subTask("Registering the new inference data");
		curInfstats = is;
		setChanged();
		pm.worked(1);
		pm.subTask("Marking new decision locations");
		for (Decision d : is.getDecisions()) {
			try {
				d.mark();
			} catch (CoreException ce1) {
				ce = ce1;
			}
			pm.worked(1);
		}
		pm.subTask("Marking new constraint locations");
		for (RLocationNode n : is.getConstraintLocations()) {
			try {
				n.mark();
			} catch (CoreException ce1) {
				ce = ce1;
			}
			pm.worked(1);
		}
		pm.done();
		notifyObservers(is);
		if (ce != null)
			throw ce;
	}

	IResourceChangeListener watcher = new IResourceChangeListener() {
		public void resourceChanged(IResourceChangeEvent event) {
			asyncUpdateInfstats(true);
		}
	};

	public synchronized void setWatched(boolean watched) {
		if (watched != this.watched) {
			this.watched = watched;
			if (watched) {
				ResourcesPlugin.getWorkspace()
					.addResourceChangeListener(watcher);
				asyncUpdateInfstats(false); // the _first_ load is immediate and interactive
			} else {
				ResourcesPlugin.getWorkspace()
					.removeResourceChangeListener(watcher);
			}
		}
	}

}
