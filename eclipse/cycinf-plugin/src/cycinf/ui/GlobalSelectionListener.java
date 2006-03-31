package cycinf.ui;

import java.util.*;

import org.eclipse.jface.viewers.*;
import org.eclipse.ui.*;

public abstract class GlobalSelectionListener<T> implements ISelectionListener,
	ISelectionChangedListener {
	static Set<GlobalSelectionListener<?>> listeners = new HashSet<GlobalSelectionListener<?>>();
	
	public static void fireSelection(IWorkbenchPage page, ISelection selection) {
		for (GlobalSelectionListener<?> l : listeners)
			if (l.myPart.getSite().getPage() == page)
				l.processSelection(selection);
	}
	
	IWorkbenchPart myPart;
	public GlobalSelectionListener(IWorkbenchPart myPart) {
		this.myPart = myPart;
		listeners.add(this);
		applyGlobalSelectionNow();
	}
	
	public void unregister() {
		listeners.remove(this);
	}

	public void applyGlobalSelectionNow() {
		IWorkbenchPage page = myPart.getSite().getPage();
		selectionChanged(page.getActivePart(), page.getSelection());
	}

	public void processSelection(ISelection selection) {
		if (selection != null
			&& selection instanceof IStructuredSelection) {
			IStructuredSelection globalSelection2 = (IStructuredSelection) selection;
			List objects = globalSelection2.toList();
			// Collect the wanted objects
			ArrayList<T> wants = new ArrayList<T>();
			for (Object o : objects) {
				T want = wantObject(o);
				if (want != null)
					wants.add(want);
			}
			// Only set the selection if at least one object is
			// relevant.
			if (!wants.isEmpty())
				selectObjects(Collections
					.unmodifiableList(wants));
		}
	}

	public void selectionChanged(IWorkbenchPart part,
		ISelection globalSelection) {
		// If my part is active, don't recurse changes in its selection!
		if (part != myPart)
			processSelection(globalSelection);
	}

	public void selectionChanged(SelectionChangedEvent event) {
		processSelection(event.getSelection());
	}

	// Return the object if you want it, otherwise null
	protected abstract T wantObject(Object o);

	// Select all the objects
	protected abstract void selectObjects(List<T> objects);
}