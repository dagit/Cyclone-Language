package cycinf.build;

import static cycinf.CycinfPlugin.message;

import java.io.InputStream;

import org.eclipse.core.resources.*;
import org.eclipse.core.runtime.*;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.MessageDialog;

import cycinf.util.ActionOnCycinfDir;

public final class SetUpMakeAction extends ActionOnCycinfDir {
	private boolean writeFile(InputStream source, IFile target,
		boolean overwrite) throws CoreException {
		if (target.exists()) {
			if (overwrite)
				target.setContents(source, false, true,
					new NullProgressMonitor());
			else
				return false;
		} else
			target.create(source, false, new NullProgressMonitor());
		return true;
	}

	@Override
	public void run(IAction a) {
		// Right now, we just write the Cyclone make library
		IContainer dir2 = dir.getDir();
		InputStream cycinfmk = SetUpMakeAction.class
			.getResourceAsStream("cycinf.mk");
		InputStream psmk = SetUpMakeAction.class
			.getResourceAsStream("project-sample.mk");
		try {
			boolean wroteCimk = writeFile(cycinfmk, dir2
				.getFile(Path.fromPortableString("cycinf.mk")),
				true);
			if (wroteCimk)
				message(
					"The Cyclone make library has been written to cycinf.mk.",
					MessageDialog.INFORMATION);
			boolean wroteSample = writeFile(psmk, dir2.getFile(Path
				.fromPortableString("Makefile")), false);
			if (wroteSample)
				message(
					"A sample Makefile has been written.  You should customize it.  At the very least, type in the names of your source files so something gets compiled.",
					MessageDialog.INFORMATION);
			else
				message(
					"You already have a Makefile, so a sample Makefile was not written.",
					MessageDialog.INFORMATION);
			message(
				"Done.  If you want automatic building, check \"Build on resource save\" in the project properties, and check \"Project\" -> \"Build Automatically\".  " +
				"The project should then build whenever you save something.  Consider enabling \"Watch Inference Data\" too.",
				MessageDialog.INFORMATION);
		} catch (CoreException e) {
			message("Something went wrong: " + e.toString(),
				MessageDialog.ERROR, new String[] {"OK"}, 0);
		} finally {
			try {
				cycinfmk.close();
			} catch (Exception e) {}
			try {
				psmk.close();
			} catch (Exception e) {}
		}
	}
}
