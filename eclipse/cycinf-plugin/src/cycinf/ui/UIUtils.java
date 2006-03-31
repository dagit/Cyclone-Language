package cycinf.ui;

import java.util.*;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.*;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.texteditor.ITextEditor;

import cycinf.cvars.Decision;
import cycinf.reasoning.*;

public abstract class UIUtils {
	private UIUtils() {}

	private static final Set<ReasoningView> listeningReasoningViews = new HashSet<ReasoningView>();
	public static void register(ReasoningView v) { listeningReasoningViews.add(v); }
	public static void unregister(ReasoningView v) { listeningReasoningViews.remove(v); }
	public static void notifyReasoningView(Decision d) {
		for (ReasoningView v : listeningReasoningViews)
			v.setMainDecision(d);
	}
	
	public static void revealDecision(Decision d) {
		try {
			IDE.openEditor(PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage(), d.getMarker());
		} catch (PartInitException e) {
			MessageDialog
				.openError(SWTUtils.currentShell(),
					"Cyclone with Inference",
					"The editor could not be opened to show the location of a decision.");
		}
	}

	public static void revealRLocationNode(RLocationNode n) {
		try {
			IEditorInput input = new FileEditorInput(n.getFile());
			IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
			IEditorPart editor = page.openEditor(input, "cycinf.CycloneEditor");
			((ITextEditor) editor).selectAndReveal(n.getLoc2().getStartPos(), 1);
		} catch (PartInitException e) {
			MessageDialog
			.openError(SWTUtils.currentShell(),
				"Cyclone with Inference",
				"The editor could not be opened to show the location of a constraint.");
		}
	}
	
	public static void revealConstraint(RConstraintTop ctop) {
		RLocationNode n = ctop.getNode().getLocationNode();
		if (n != null) revealRLocationNode(n);
	}
}
