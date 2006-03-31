package cycinf.cvars;

import general.util.Validator;

import java.util.*;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.CoreException;

import cycinf.infstats.*;
import cycinf.util.*;

public abstract class Decision implements Markable, BelongsToInfstats {
	public abstract Cvar getCvar();
	public abstract Cvalue getCvalue();
	public String decisionName() {
		return getCvalue().prettyString();
	}
	private final DirInfstats owningInfstats;
	public final DirInfstats getOwningInfstats() {
		return owningInfstats;
	}

	public String description() {
		return decisionName() + ": " + getCvar().prettyString();
	}

	private final SourceLocation location;
	public final SourceLocation getLocation() {
		return location;
	}

	private IMarker marker = null;
	public final IMarker getMarker() {
		return marker;
	}

	private static Map<IMarker, Decision> decisionMarkerCache = new HashMap<IMarker, Decision>();

	public Decision(DirInfstats owningInfstats,
		SourceLocation location) {
		this.owningInfstats = Validator.nonnull(owningInfstats,
			"owningInfstats");
		this.location = location;
	}

	private static final String markerType = "cycinf.DecisionMarker";
	public void mark() throws CoreException {
		if (marker == null) {
			marker = getLocation().findFileInDir(
				getOwningInfstats().getDir()).createMarker(
				markerType);
			marker.setAttribute(IMarker.LINE_NUMBER, getLocation()
				.getLine());
			marker.setAttribute(IMarker.MESSAGE, description());
			marker.setAttribute(IMarker.PRIORITY,
				IMarker.PRIORITY_NORMAL);
			marker.setAttribute(IMarker.SEVERITY,
				IMarker.SEVERITY_INFO);
			decisionMarkerCache.put(marker, this);
		}
	}

	public void unmark() throws CoreException {
		if (marker != null) {
			decisionMarkerCache.remove(marker);
			marker.delete();
			marker = null;
		}
	}

	public static Decision lookupMarker(IMarker m) {
		return decisionMarkerCache.get(m);
	}
}
