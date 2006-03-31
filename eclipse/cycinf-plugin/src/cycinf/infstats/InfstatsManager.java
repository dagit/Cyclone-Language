package cycinf.infstats;

import java.util.*;

import org.eclipse.core.resources.IContainer;

public final class InfstatsManager {
	private static InfstatsManager manager = new InfstatsManager();
	public static InfstatsManager getManager() {
		return manager;
	}

	Map<IContainer, DirTracker> dirTrackers = new HashMap<IContainer, DirTracker>();
	public synchronized DirTracker getTrackerFor(IContainer dir) {
		// For convenience, map a .cych work area to the actual project folder
		if (dir.getName().equals(".cych"))
			dir = dir.getParent();
		DirTracker t = dirTrackers.get(dir);
		if (t == null) {
			t = new DirTracker(dir);
			dirTrackers.put(dir, t);
		}
		return t;
	}
	// Map<IContainer, DirInfstats> infstatsCache = new HashMap<IContainer,
	// DirInfstats>();
	public DirInfstats getCurInfstatsFor(IContainer dir) {
		return getTrackerFor(dir).getCurInfstats();
	}

	InfstatsManager() {}

	String[] getCompilerArgs() {
		// FUTURE Observe the user's preferences
		return new String[] {"cyclone-inf", "--infstats"};
	}

}
