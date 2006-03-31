package cycinf.infstats;

import general.util.*;

import java.io.*;
import java.util.*;

import org.eclipse.core.runtime.IProgressMonitor;

import cycinf.constraints.*;
import cycinf.cvars.*;
import cycinf.cvars.ptrbnd.PointerKindDecisionParser;
import cycinf.reasoning.*;

public final class DirInfstats {
	private DirTracker dir;
	public DirTracker getDir() {
		return dir;
	}

	private boolean loaded = false;
	public boolean isLoaded() {
		return loaded;
	}
	private void checkLoaded() {
		if (!loaded)
			throw new IllegalStateException();
	}

	private Date loadDate;
	public Date getLoadDate() {
		checkLoaded();
		return loadDate;
	}
	long stamp;
	public long getStamp() {
		checkLoaded();
		return stamp;
	}

	private int numParseErrors;
	public int getNumParseErrors() {
		return numParseErrors;
	}
	public void logParseError() {
		if (loaded)
			throw new IllegalStateException();
		numParseErrors++;
	}

	DirInfstats(DirTracker dir) {
		this.dir = dir;
		numParseErrors = 0;
	}

	// We now allow early access to the decisions and constraints in the
	// hope that this access will not be abused during loading...

	private final KeyedSet<Cvar, Decision> decisions = new HashKeyedSet<Cvar, Decision>(new DecisionToCvarKeyer());
	public KeyedSet<Cvar, Decision> getDecisions() {
		/* checkLoaded(); */return Collections2
			.unmodifiableKeyedSet(decisions);
	}

	public Decision getDecisionOnCvar(Cvar cvar) {
		return decisions.lookup(cvar);
	}

	private final HashKeyedSet<Constraint,RConstraintTop> constraints = new HashKeyedSet<Constraint, RConstraintTop>(new RConstraintKeyer());
	public KeyedSet<Constraint, RConstraintTop> getConstraints() {
		/* checkLoaded(); */return Collections2
			.unmodifiableKeyedSet(constraints);
	}

	private final Map<Cvar, Set<RNode<?>>> cvarsToReferences = new HashMap<Cvar, Set<RNode<?>>>();
	public Set<RNode<?>> getReferencesTo(Cvar cvar) {
		Set<RNode<?>> s1 = cvarsToReferences.get(cvar);
		if (s1 == null)
			return Collections.emptySet();
		else
			return Collections.unmodifiableSet(s1);
	}
	private Set<RNode<?>> getReferencesTo2(Cvar cvar) {
		Set<RNode<?>> s1 = cvarsToReferences.get(cvar);
		if (s1 == null) {
			s1 = new HashSet<RNode<?>>();
			cvarsToReferences.put(cvar, s1);
		}
		return s1;
	}
	private final Set<RLocationNode> constraintLocations = new HashSet<RLocationNode>();
	public Set<RLocationNode> getConstraintLocations() {
		return Collections.unmodifiableSet(constraintLocations);
	}
	private final Map<Cvar, Set<RConstraint<?>>> cvarsToDeterminingConstraints = new HashMap<Cvar, Set<RConstraint<?>>>();
	public Set<RConstraint<?>> getConstraintsDetermining(Cvar cvar) {
		Set<RConstraint<?>> s1 = cvarsToDeterminingConstraints.get(cvar);
		if (s1 == null)
			return Collections.emptySet();
		else
			return Collections.unmodifiableSet(s1);
	}
	private Set<RConstraint<?>> getConstraintsDetermining2(Cvar cvar) {
		Set<RConstraint<?>> s1 = cvarsToDeterminingConstraints.get(cvar);
		if (s1 == null) {
			s1 = new HashSet<RConstraint<?>>();
			cvarsToDeterminingConstraints.put(cvar, s1);
		}
		return s1;
	}
	private void scanConstraint(RNode<?> n) {
		if (n instanceof RCvarRef) {
			RCvarRef n2 = GenericNonsense.workaroundCast(RCvarRef.class, n);
			Set<RNode<?>> s = getReferencesTo2(n2.getBack().getCvar());
			s.add(n);
		}
		if (n instanceof RLocationNode) {
			RLocationNode n2 = GenericNonsense.workaroundCast(RLocationNode.class, n);
			constraintLocations.add(n2);
		}
		if (n instanceof RConstraint) {
			RConstraint<?> n2 = GenericNonsense.workaroundCast(RConstraint.class, n);
			Cvar cvar = n2.getConstrainedVar();
			if (cvar != null && n2.isEffective())
				getConstraintsDetermining2(cvar).add(n2);
		}
		RNode<?>[] children = n.getTreeChildren();
		for (RNode<?> n2 : children)
			scanConstraint(n2);
	}
	
	LoadInfstatsJob job;
	void scheduleLoading(boolean automatic) {
		job = new LoadInfstatsJob(this, automatic);
		job.schedule();
	}
	void waitUntilLoaded() {
		try {
			job.join();
		} catch (InterruptedException e) {
			throw new RuntimeException(e);
		}
	}

	void loadDecisions(BufferedReader in, int nd1, IProgressMonitor pm)
		throws IOException {
		pm
			.beginTask(
				"Reading inference decisions from the Cyclone compiler",
				nd1);
		pm.subTask("Preparing");
		String line;
		lines: for (int i = 0; i < nd1; i++, pm.worked(1)) {
			pm.subTask("Reading inference decision #" + i);
			line = in.readLine();
			if (line == null) {
				System.err.println("Unexpected EOF");
				logParseError();
				break lines;
			}
			Decision id = PointerKindDecisionParser.parse(this,
				line);
			if (id != null)
				decisions.add(id);
		}
		pm.subTask("Finishing");
	}

	void loadConstraints(BufferedReader in, int nd1, IProgressMonitor pm)
		throws IOException {
		pm
			.beginTask(
				"Loading constraints from the \"cvars\" file",
				nd1);
		pm.subTask("Preparing");
		
		String line;
		lines: for (int i = 0; i < nd1; i++, pm.worked(1)) {
			pm.subTask("Reading cvars line #" + i);
			line = in.readLine();
			if (line == null) {
				System.err.println("Unexpected EOF");
				logParseError();
				break lines;
			}
			ConstraintParser.parse(this, line);
		}
		pm.subTask("Finishing");
	}

	public void logConstraint(Constraint c) {
		if (constraints.lookup(c) == null) {
			RConstraintTop rc = new RConstraintTop(this, c);
			constraints.add(rc);
			scanConstraint(rc.getNode());
		}
	}

	// Oh well, this is bad design
	void markLoaded() {
		loadDate = new Date();
		loaded = true;
	}
}
