package cycinf.ui;

import general.util.GenericNonsense;

import java.util.*;
import java.util.List;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.*;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.ui.part.ViewPart;

import cycinf.cvars.Decision;
import cycinf.reasoning.*;
import cycinf.util.*;

// Single constraint, tree structure

public class ConstraintView extends ViewPart {
	public static final String PART_ID = "cycinf.ConstraintView";

	RConstraintTop constraint;

	Composite viewArea;
	private Layout viewAreaLayout;
	private Label label;

	TreeViewer ceStructureViewer;
	Action goToDecisionAction;

	class ConstraintWrapper {
		ConstraintWrapper(RConstraintTop c1) {
			ctop = c1;
			c = new RConstraint<?>[] { c1.getNode() };
		}
		RConstraintTop ctop;
		RConstraint<?>[] c; // should be just one
	}

	class ViewContentProvider extends TreelikeContentProvider {

		public void inputChanged(Viewer v, Object oldInput,
			Object newInput) {
			/*
			 * When the ConstraintView wants to change constraints,
			 * it calls this so that the constraintViewer mechanism
			 * understands. We save the change into
			 * "constraint" during this call. Seems strange, but
			 * this way no one gets confused.
			 */
			ConstraintView.this.constraint = newInput == null ? null : ((ConstraintWrapper) newInput).ctop;
		}
		public void dispose() {}
		public Treelike<?>[] getElements(Object parent) {
			if (parent instanceof ConstraintWrapper)
				return ((ConstraintWrapper) parent).c;
			else
				return getChildren(parent);
		}
	}

	class ViewLabelProvider extends TreelikeLabelProvider {}

	// Turns CvarRefs into Decisions and adds the selected RConstraintTop.
	/*class OurSelectionProvider implements ISelectionProvider,
		ISelectionChangedListener {
		ArrayList<ISelectionChangedListener> listeners = new ArrayList<ISelectionChangedListener>();

		public OurSelectionProvider() {
			ceStructureViewer.addSelectionChangedListener(this);
		}

		public void addSelectionChangedListener(
			ISelectionChangedListener listener) {
			listeners.add(listener);
		}

		public ISelection getSelection() {
			IStructuredSelection cvss = (IStructuredSelection) ceStructureViewer
				.getSelection();
			ArrayList<Object> sl = new ArrayList<Object>();
			sl.addAll(cvss.toList());
			for (Object o : cvss.toList()) {
				RNode<?> n = (RNode<?>) o;
				if (n instanceof RCvarRef)
					sl.add(GenericNonsense.workaroundCast(RCvarRef.class, n).getDecisionOnTheCvar());
			}
			sl.add(constraint);
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
	}*/

	private Observer infstatsObserver = new Observer() {
		public void update(Observable changed, Object change) {
			viewArea.getDisplay().asyncExec(new Runnable() {
				public void run() {
					refresh();
				}
			});
		}
	};

	public ConstraintView() {}

	GlobalSelectionListener<RNode<?>> globalSelectionListener;

	void doRevealSomething() {
		// event.getSelection() comes to the same
		ISelection selection = ceStructureViewer.getSelection();
		if (!selection.isEmpty()) {
			RNode<?> n = (RNode<?>) ((IStructuredSelection) selection)
				.getFirstElement();
			if (n instanceof RLocationNode)
				UIUtils.revealRLocationNode(GenericNonsense.workaroundCast(RLocationNode.class, n));
			else if (n instanceof RCvarRef) {
				Decision d = GenericNonsense.workaroundCast(RCvarRef.class, n).getDecisionOnTheCvar();
				UIUtils.revealDecision(d);
				UIUtils.notifyReasoningView(d);
			}
		}
	}

	@Override
	public void createPartControl(Composite parent) {
		viewArea = parent;
		viewAreaLayout = new FormLayout();
		viewArea.setLayout(viewAreaLayout);

		ceStructureViewer = new TreeViewer(parent, SWT.MULTI
			| SWT.H_SCROLL | SWT.V_SCROLL);
		ceStructureViewer.setContentProvider(new ViewContentProvider());
		ceStructureViewer.setLabelProvider(new ViewLabelProvider());
		ceStructureViewer
			.setAutoExpandLevel(AbstractTreeViewer.ALL_LEVELS);

		label = new Label(viewArea, SWT.LEFT | SWT.WRAP);
		setConstraint(null);

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
		ceStructureViewer.getControl().setLayoutData(ifd);

		goToDecisionAction = new Action() {
			@Override
			public void run() {
				doRevealSomething();
			}
		};
		ceStructureViewer
			.addDoubleClickListener(new IDoubleClickListener() {
				public void doubleClick(DoubleClickEvent event) {
					goToDecisionAction.run();
				}
			});

		globalSelectionListener = new GlobalSelectionListener<RNode<?>>(this) {
			@Override
			protected RNode<?> wantObject(Object o) {
				return Interchange.toRNode(o);
			}
			@Override
			protected void selectObjects(List<RNode<?>> objects) {
				RNode<?> n = objects.get(0);
				setConstraint(n.findConstraintTop());
				ceStructureViewer.setSelection(new StructuredSelection(Collections.singletonList(n)));
			}
		};
		getSite().setSelectionProvider(ceStructureViewer);
		getSite().getPage().addPostSelectionListener(
			globalSelectionListener);
	}

	void setConstraint(RConstraintTop c) {
		if (constraint != null)
			constraint.getOwningInfstats().getDir().deleteObserver(
				infstatsObserver);
		ceStructureViewer.setInput(c == null ? null : new ConstraintWrapper(c));
		if (c == null)
			label.setText("Empty.  Select a constraint.");
		else
			label.setText("Constraint structure:");
		viewArea.layout();
		if (constraint != null)
			constraint.getOwningInfstats().getDir().addObserver(
				infstatsObserver);
	}

	void refresh() {
		setConstraint(null); // no reliable way to match constraints upon reload
	}

	/**
	 * Passing the focus request to the constraintViewer's control.
	 */
	@Override
	public void setFocus() {
		ceStructureViewer.getControl().setFocus();
	}

	@Override
	public void dispose() {
		super.dispose();
		globalSelectionListener.unregister();
	}
}