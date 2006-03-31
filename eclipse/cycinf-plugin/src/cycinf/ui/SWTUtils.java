package cycinf.ui;

import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;

public abstract class SWTUtils {
	private SWTUtils() {}

	public static Shell currentShell() {
		return PlatformUI.getWorkbench().getActiveWorkbenchWindow()
			.getShell();
	}
}
