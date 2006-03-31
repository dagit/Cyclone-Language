package cycinf.ui;

import general.util.GenericNonsense;

import java.util.*;
import java.util.List;

import org.eclipse.jface.viewers.*;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.ui.part.ViewPart;

import cycinf.cvars.*;
import cycinf.infstats.DirInfstats;
import cycinf.reasoning.RNode;
import cycinf.util.Interchange;

// Single decision, referring constraints

public class DecisionView extends ViewPart {
	public static final String PART_ID = "cycinf.DecisionView";

	Decision decision;

	Composite viewArea;
	private Layout viewAreaLayout;
	private Label label;

	TableViewer referringConstraintsViewer;

	private static final int CONSTRAINT_STRING_COLUMN = 0;

	class ViewContentProvider implements IStructuredContentProvider {

		public void inputChanged(Viewer v, Object oldInput,
			Object newInput) {
			/*
			 * When the DecisionView wants to change constraints, it
			 * calls this so that the decisionViewer mechanism
			 * understands. We save the change into the Decision
			 * during this call. Seems strange, but this way no one
			 * gets confused.
			 */
			DecisionView.this.decision = (Decision) newInput;
		}
		public void dispose() {}
		public RNode<?>[] getElements(Object parent) {
			Set<RNode<?>> constraints = decision
				.getOwningInfstats().getReferencesTo(
					decision.getCvar());
			return constraints
				.toArray(new RNode<?>[constraints.size()]);
		}
	}

	class ViewLabelProvider extends LabelProvider implements
		ITableLabelProvider {
		public String getColumnText(Object obj, int index) {
			RNode<?> c = (RNode<?>) obj;
			switch (index) {
			case CONSTRAINT_STRING_COLUMN:
				return c.findConstraintTop().getNode().getBack().toString();
			default:
				throw new IllegalArgumentException();
			}
		}
		public Image getColumnImage(Object obj, int index) {
			return null;
		}
	}

	// Turns CvarRefs into Decisions.
	class OurSelectionProvider implements ISelectionProvider,
		ISelectionChangedListener {
		ArrayList<ISelectionChangedListener> listeners = new ArrayList<ISelectionChangedListener>();

		public OurSelectionProvider() {
			referringConstraintsViewer
				.addSelectionChangedListener(this);
		}

		public void addSelectionChangedListener(
			ISelectionChangedListener listener) {
			listeners.add(listener);
		}

		public ISelection getSelection() {
			IStructuredSelection cvss = (IStructuredSelection) referringConstraintsViewer
				.getSelection();
			ArrayList<Object> sl = GenericNonsense.receiveRawCollection(Object.class, cvss.toList());
			sl.add(decision);
			return new StructuredSelection(sl);
		}

		public void removeSelectionChangedListener(
			ISelectionChangedListener listener) {
			listeners.remove(listener);
		}

		public void setSelection(ISelection selection) {
			globalSelectionListener.processSelection(selection);
		}

		public void selectionChanged(SelectionChangedEvent event) {
			ISelection sel = getSelection();
			for (ISelectionChangedListener listener : listeners)
				listener
					.selectionChanged(new SelectionChangedEvent(
						this, sel));
		}
	}

	private Observer infstatsObserver = new Observer() {
		public void update(Observable changed, Object change) {
			viewArea.getDisplay().asyncExec(new Runnable() {
				public void run() {
					refresh();
				}
			});
		}
	};

	public DecisionView() {}

	GlobalSelectionListener<Decision> globalSelectionListener;

	/**
	 * This is a callback that will allow us to create the decisionViewer
	 * and initialize it.
	 */
	@Override
	public void createPartControl(Composite parent) {
		viewArea = parent;
		viewAreaLayout = new FormLayout();
		viewArea.setLayout(viewAreaLayout);

		referringConstraintsViewer = new TableViewer(parent, SWT.MULTI
			| SWT.H_SCROLL | SWT.V_SCROLL);
		referringConstraintsViewer
			.setContentProvider(new ViewContentProvider());
		referringConstraintsViewer
			.setLabelProvider(new ViewLabelProvider());

		label = new Label(viewArea, SWT.LEFT | SWT.WRAP);
		setDecision(null);

		FormData lbd = new FormData();
		lbd.top = new FormAttachment(0, 2);
		lbd.left = new FormAttachment(0, 2);
		lbd.right = new FormAttachment(100, -2);
		label.setLayoutData(lbd);
		FormData ifd = new FormData();
		ifd.top = new FormAttachment(label, 2);
		ifd.bottom = new FormAttachment(100, -2);
		ifd.left = new FormAttachment(0, 2);
		ifd.right = new FormAttachment(100, -2);
		referringConstraintsViewer.getControl().setLayoutData(ifd);

		// FUTURE When possible, select RCvarRefs instead of just opening their decisions
		globalSelectionListener = new GlobalSelectionListener<Decision>(
			this) {
			@Override
			protected Decision wantObject(Object o) {
				return Interchange.toDecision(o);
			}
			@Override
			protected void selectObjects(
				List<Decision> objects) {
				setDecision(objects.get(0));
			}
		};
		getSite().setSelectionProvider(referringConstraintsViewer);
		getSite().getPage().addPostSelectionListener(
			globalSelectionListener);
	}

	void setDecision(Decision d) {
		if (decision != null)
			decision.getOwningInfstats().getDir().deleteObserver(
				infstatsObserver);
		referringConstraintsViewer.setInput(d);
		if (d == null)
			label.setText("Empty.  Select a decision.");
		else
			label.setText("Constraints referring to "
				+ d.getCvar().prettyString());
		viewArea.layout();
		if (decision != null)
			decision.getOwningInfstats().getDir().addObserver(
				infstatsObserver);
	}

	// Try to preserve a decision over updates
	void refresh() {
		Cvar cvar = decision.getCvar();
		DirInfstats newInfstats = decision.getOwningInfstats().getDir()
			.getCurInfstats();
		if (newInfstats == null)
			setDecision(null);
		else {
			Decision newDecision = newInfstats
				.getDecisionOnCvar(cvar);
			setDecision(newDecision);
		}
	}

	/**
	 * Passing the focus request to the constraintViewer's control.
	 */
	@Override
	public void setFocus() {
		referringConstraintsViewer.getControl().setFocus();
	}

	@Override
	public void dispose() {
		super.dispose();
		globalSelectionListener.unregister();
	}
}