package cycinf.reasoning;

import java.util.*;

import org.eclipse.core.resources.*;
import org.eclipse.core.runtime.CoreException;

import cycinf.util.*;

public class RLocationNode extends RNode<RLocationNode> implements Markable {
	final SourceLocation location;
	public final SourceLocation getLocation() {
		return location;
	}
	// these two for the benefit of UIUtils.revealRLocationNode
	final IFile file;
	public final IFile getFile() { return file; }
	final SourceSearch loc2;
	public final SourceSearch getLoc2() { return loc2; }
	RLocationNode(RContext1<?, RLocationNode> role, SourceLocation location) {
		super(role);
		registerThis(this);
		this.location = location;
		this.nodeDesc = location.toString();
		this.file = getLocation().findFileInDir(getContext().getOwningInfstats().getDir());
		this.loc2 = SourceSearch.justFindLocation(getContext().getOwningInfstats().getDir(), location);
	}
	
	private IMarker marker = null;
	public IMarker getMarker() {
		return marker;
	}

	private static Map<IMarker, RLocationNode> constraintMarkerCache = new HashMap<IMarker, RLocationNode>();

	private static final String markerType = "cycinf.ConstraintMarker";
	public void mark() throws CoreException {
		if (marker == null) {
			marker = getLocation().findFileInDir(
				getOwningInfstats().getDir()).createMarker(
				markerType);
			marker.setAttribute(IMarker.LINE_NUMBER, getLocation()
				.getLine());
			marker.setAttribute(IMarker.MESSAGE, "Mentioned by constraint " + findConstraintTop().getNode().getBack().toString());
			marker.setAttribute(IMarker.PRIORITY,
				IMarker.PRIORITY_NORMAL);
			marker.setAttribute(IMarker.SEVERITY,
				IMarker.SEVERITY_INFO);
			marker.setAttribute(IMarker.CHAR_START, loc2.getStartPos());
			marker.setAttribute(IMarker.CHAR_END, loc2.getEndPos());
			constraintMarkerCache.put(marker, this);
		}
	}
	public void unmark() throws CoreException {
		if (marker != null) {
			constraintMarkerCache.remove(marker);
			marker.delete();
			marker = null;
		}
	}

	public static RLocationNode lookupMarker(IMarker m) {
		return constraintMarkerCache.get(m);
	}
}
