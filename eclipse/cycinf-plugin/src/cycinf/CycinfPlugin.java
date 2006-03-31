package cycinf;

import java.util.*;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

import cycinf.ui.SWTUtils;

/**
 * The main class of the Cyclone with Inference Plugin.
 * @author Matt McCutchen &lt;hashproduct@verizon.net&gt;
 */
public final class CycinfPlugin extends AbstractUIPlugin {
	public static final String id = "cycinf";

	/** The single instance of the plugin */
	private static CycinfPlugin plugin;
	public static CycinfPlugin getPlugin() {
		return plugin;
	}

	/** The plugin's resource bundle */
	private static ResourceBundle resourceBundle;

	public static ResourceBundle getResourceBundle() {
		if (resourceBundle == null)
			try {
				resourceBundle = ResourceBundle
					.getBundle("cycinf.CycinfResources");
			} catch (MissingResourceException x) {}
		return resourceBundle;
	}

	public CycinfPlugin() {
		plugin = this;
	}

	/** Starts the plugin. */
	@Override
	public void start(BundleContext context) throws Exception {
		super.start(context);
	}

	/** Stops the plugin. */
	@Override
	public void stop(BundleContext context) throws Exception {
		super.stop(context);
		plugin = null;
		resourceBundle = null;
	}

	/**
	 * Looks up and returns the resource string with the given key; returns
	 * the key if the resource bundle isn't available.
	 */
	public static String getResourceString(String key) {
		String resourceString = key;
		ResourceBundle bundle = CycinfPlugin.getResourceBundle();
		if (bundle != null)
			try {
				resourceString = bundle.getString(key);
			} catch (MissingResourceException e) {}
		return resourceString;
	}

	/**
	 * Returns an image descriptor for the image file at the given plug-in
	 * relative path.
	 * @param path the path
	 * @return the image descriptor
	 */
	public static ImageDescriptor getImageDescriptor(String path) {
		return AbstractUIPlugin.imageDescriptorFromPlugin("cycinf",
			path);
	}

	public static void message(String text, int kind) {
		message(text, kind, new String[] {"OK"}, 0);
	}
	public static int message(final String text, final int kind,
		final String[] buttons, final int defButton) {
		final Shell shell = SWTUtils.currentShell();

		class MessageShower implements Runnable {
			int choice;
			public void run() {
				MessageDialog d = new MessageDialog(shell,
					"Cyclone with Inference Plugin", null,
					text, kind, buttons, defButton);
				d.setBlockOnOpen(true);
				int ch = d.open();
				synchronized (this) {
					choice = ch;
				}
			}
		}
		MessageShower ms = new MessageShower();
		shell.getDisplay().syncExec(ms);
		synchronized (ms) {
			return ms.choice;
		}
	}

	public static void sayUnimplemented(final String what) {
		message(
			"The feature \""
				+ what
				+ "\" is not yet implemented, even though the means to activate it are in place.",
			MessageDialog.ERROR);
		/*
		 * final Shell shell = SWTUtils.currentShell();
		 * shell.getDisplay().syncExec(new Runnable() { public void
		 * run() { MessageDialog d = new MessageDialog(shell, "Cyclone
		 * with Inference Plugin", null, "The feature \"" + what + "\"
		 * is not yet implemented, even though the means to activate it
		 * are in place.", MessageDialog.INFORMATION, new String[] {
		 * "OK" }, 0); d.setBlockOnOpen(true); d.open(); } });
		 */
	}
}
