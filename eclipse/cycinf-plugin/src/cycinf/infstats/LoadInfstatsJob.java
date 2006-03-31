package cycinf.infstats;

import general.util.Validator;

import java.io.*;
import java.util.regex.*;

import org.eclipse.core.resources.*;
import org.eclipse.core.runtime.*;
import org.eclipse.core.runtime.jobs.Job;

import cycinf.CycinfPlugin;

final class LoadInfstatsJob extends Job {
	final DirTracker dir;
	final DirInfstats is;
	final boolean automatic;

	LoadInfstatsJob(DirInfstats is, boolean automatic) {
		super("[Load inference data]");
		this.is = is;
		this.dir = is.getDir();
		this.automatic = automatic;
		setUser(!automatic);
		setName("Load inference data in " + dir.getPath());
	}

	Process compilerProcess;
	BufferedReader decisionsIn;
	BufferedReader constraintsIn;

	private void setUpInfstatsProcess() throws IOException {
		ProcessBuilder pb = new ProcessBuilder(InfstatsManager
			.getManager().getCompilerArgs());
		pb.redirectErrorStream(true);
		pb.directory(dir.getDir().getLocation().toFile());
		compilerProcess = pb.start();
		InputStream in1 = compilerProcess.getInputStream();
		decisionsIn = new BufferedReader(new InputStreamReader(in1));
	}

	private void setUpCvarsFile() throws CoreException {
		//FileReader in1 = new FileReader(
			//;
			//.getLocation()
			//.toFile());
		constraintsIn = new BufferedReader(new InputStreamReader(dir.cvarsFile.getContents()));
	}

	private static final String expectedGreeting = "Looking for constraint variables...";
	private static final Pattern amountLinePattern = Pattern
		.compile("Found (\\d+) of them.");

	IStatus makeStatus(int code, String message, Throwable e) {
		if (e != null) {
			StringWriter sw = new StringWriter();
			e.printStackTrace(new PrintWriter(sw));
			message += "\n\n" + sw.toString();
		}
		return new Status(code, CycinfPlugin.id, code, message, e);
	}

	@Override
	public IStatus run(IProgressMonitor pm) {
		try {
			// Monolith ... oh well
			pm.beginTask("Load inference data in " + dir.getPath(), 100);

			if (!dir.infstatsPresent())
				return makeStatus(IStatus.ERROR, dir.getPath() +
					" does not seem to contain inference information: " +
					"it has no \".cych/cvars\" file.  Make sure the project is built; if you " +
					"still get this error, refresh the folder to make sure Eclipse notices " +
					"that the cvars file exists.", null);

			if (automatic) {
				pm.subTask("Waiting for inference data to stabilize");
				do {
					is.stamp = dir.infstatsStamp();
					try {
						Thread.sleep(2000);
					} catch (InterruptedException e) {
						throw new RuntimeException(e);
					}
				} while (is.stamp != dir.infstatsStamp());
			} else
				is.stamp = dir.infstatsStamp();

			// Fire up the compiler and read the header. [8]
			try {
				pm.subTask("Launching Cyclone compiler");
				setUpInfstatsProcess();
				pm.worked(2);
			} catch (Exception e) {
				return makeStatus(IStatus.ERROR,
					"An exception occurred while launching the Cyclone compiler.  "
						+ "No changes have been made to the workspace.", e);
			}
			int numDecisions;
			try {
				pm.subTask("Waiting for Cyclone compiler to start up");
				String greeting = Validator.nonnull(decisionsIn.readLine(), "greeting");
				if (!greeting.equals(expectedGreeting))
					throw new RuntimeException("Bad greeting from the compiler: " + greeting);
				pm.worked(2);
				pm.subTask("Waiting for Cyclone compiler to collect decisions");
				String amountLine = Validator.nonnull(decisionsIn.readLine(), "amount line");
				Matcher m = amountLinePattern.matcher(amountLine);
				if (m.matches()) {
					numDecisions = Integer.parseInt(m.group(1));
				} else
					throw new RuntimeException("Bad amount line from the compiler: " + amountLine);
				pm.worked(2);
				pm.subTask("Waiting for Cyclone compiler to prepare " + numDecisions + " decisions");
				Validator.nonnull(decisionsIn.readLine(), "heading line 1");
				Validator.nonnull(decisionsIn.readLine(), "heading line 2");
				pm.worked(2);
			} catch (Exception e) {
				return makeStatus(IStatus.ERROR,
					"An exception occurred while the Cyclone compiler collected and prepared inference decisions.  "
						+ "No changes have been made to the workspace.", e);
			}

			// Read the bulk of the inference decisions and clean up. [37]
			try {
				pm.subTask("Beginning to read decisions from Cyclone compiler");
				SubProgressMonitor pm2 = new SubProgressMonitor(pm, 35);
				is.loadDecisions(decisionsIn, numDecisions, pm2);
				pm.subTask("Waiting for Cyclone compiler to quit");
				compilerProcess.waitFor();
				decisionsIn.close();
				pm.worked(2);
			} catch (Exception e) {
				return makeStatus(IStatus.ERROR,
					"An exception occurred while reading inference decisions from the Cyclone compiler.  "
						+ "No changes have been made to the workspace.", e);
			}

			try {
				pm.subTask("Opening the \"cvars\" file to load constraints");
				setUpCvarsFile();
				pm.worked(2);
				/* String fileDirective = */Validator.nonnull(constraintsIn.readLine(), "fileDirective");
				pm.worked(2);
			} catch (Exception e) {
				return makeStatus(IStatus.ERROR,
					"An exception occurred while opening the \"cvars\" file.  "
						+ "No changes have been made to the workspace.", e);
			}

			// Read the constraints
			try {
				pm.subTask("Beginning to read constraints from the \"cvars\" file");
				SubProgressMonitor pm2 = new SubProgressMonitor(pm, 39);
				is.loadConstraints(constraintsIn, numDecisions, pm2);
				pm.subTask("Closing the \"cvars\" file");
				constraintsIn.close();
				pm.worked(2);
			} catch (Exception e) {
				return makeStatus(IStatus.ERROR,
					"An exception occurred while reading constraints from the \"cvars\" file.  "
						+ "No changes have been made to the workspace.", e);
			}

			is.markLoaded();

			// Make the decision and constraint markers [10]
			try {
				pm.subTask("Applying inference data to the workspace");
				ResourcesPlugin.getWorkspace().run(new IWorkspaceRunnable() {
					public void run(IProgressMonitor pm2) throws CoreException {
						SubProgressMonitor pm3 = new SubProgressMonitor(pm2, 10);
						is.getDir().apply(is, pm3);
					}
				}, pm);
			} catch (Exception e) {
				return makeStatus(IStatus.ERROR,
					"An exception occurred while applying the inference data to the workspace.  "
						+ "Some markers might appear strangely until you reload successfully.",
					e);
			}
			int errors = is.getNumParseErrors();
			if (errors == 0)
				return makeStatus(IStatus.INFO, is.getDecisions().size()
						+ " inference decision(s) were loaded and marked for source files in "
						+ dir.getPath() + ".", null);
			else
				return makeStatus(IStatus.ERROR /* should be WARNING but only ERROR notifies the user */, is.getDecisions().size()
						+ " inference decision(s) were loaded and marked for source files in "
						+ dir.getPath() + ".  However, " + errors
						+ " parse errors occurred; offending parts of the compiler's output were ignored.",
					null);

		} finally {
			try { decisionsIn.close(); } catch (Exception e) {}
			try { constraintsIn.close(); } catch (Exception e) {}
			synchronized (dir) { dir.loadInProgress = false; }
		}
	}
}
