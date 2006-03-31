package cycinf.util;

import org.eclipse.core.runtime.IStatus;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Shell;

public abstract class JobUtils {
	private JobUtils() {}

	public static void showJobResult(final Shell shell,
		final IStatus status, final String title) {
		shell.getDisplay().syncExec(new Runnable() {
			public void run() {
				int dlgSeverity;
				switch (status.getSeverity()) {
				case IStatus.OK:
					return;
				case IStatus.INFO:
					dlgSeverity = MessageDialog.INFORMATION;
					break;
				case IStatus.WARNING:
					dlgSeverity = MessageDialog.WARNING;
					break;
				case IStatus.ERROR:
					dlgSeverity = MessageDialog.ERROR;
					break;
				default:
					dlgSeverity = MessageDialog.NONE;
				}
				Throwable ex = status.getException();
				String msg = status.getMessage();
				if (ex != null)
					msg += "\n\n" + ex.toString();
				MessageDialog d = new MessageDialog(shell,
					title, null, msg, dlgSeverity,
					new String[] {"OK"}, 0);
				d.setBlockOnOpen(true);
				d.open();
			}
		});
	}
}
