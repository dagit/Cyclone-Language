package cycinf.util;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.CoreException;

public interface Markable {
	public abstract IMarker getMarker();
	public abstract void mark() throws CoreException;
	public abstract void unmark() throws CoreException;
}
