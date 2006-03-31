package cycinf;

import org.eclipse.core.resources.*;
import org.eclipse.core.runtime.CoreException;

/*
 * For now this nature just provides a way to tell whether a project is a
 * Cyclone with Inference project; it doesn't actually do anything.
 * And no projects are actually marked with this nature!
 * FUTURE Revisit the project nature.
 */
public class CycinfProjectNature implements IProjectNature {
	IProject theProject;

	public void configure() throws CoreException {}

	public void deconfigure() throws CoreException {}

	public IProject getProject() {
		return theProject;
	}

	public void setProject(IProject project) {
		this.theProject = project;
	}
}
