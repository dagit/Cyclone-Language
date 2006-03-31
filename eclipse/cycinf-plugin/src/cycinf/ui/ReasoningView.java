package cycinf.ui;

import general.util.GenericNonsense;

import java.util.*;

import org.eclipse.jface.viewers.*;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.ui.part.ViewPart;

import cycinf.cvars.*;
import cycinf.infstats.DirInfstats;
import cycinf.reasoning.*;
import cycinf.util.*;

public class ReasoningView extends ViewPart {
	public static final String PART_ID = "cycinf.ReasoningView";
	
	Decision mainDecision;
	
	Composite viewArea;
	private Layout viewAreaLayout;
	private Label label;
	
	TreeViewer reasoningViewer;
	//Action goToAction;
	
	abstract class ReasoningNode<P extends ReasoningNode<?, ?>, C extends ReasoningNode<?, ?>>
		extends TreelikeAdapter<ReasoningNode<?, ?>, P, C> {
		ReasoningNode(P parent) {
			super(parent);
		}
	}
	// public for Interchange's sake
	public class DecisionNode extends ReasoningNode<ConstraintNode, ConstraintNode> {
		DecisionNode(ConstraintNode parent, Decision theDecision) {
			super(parent);
			this.decision = theDecision;
		}

		Decision decision;
		public Decision getDecision() { return decision; }

		@Override
		public String getTreeLabel() {
			return decision.decisionName() + ": " + decision.getCvar().targetPrettyString();
		}

		@Override
		protected ConstraintNode[] computeTreeChildren() {
			Set<RConstraint<?>> cs = decision.getOwningInfstats().getConstraintsDetermining(decision.getCvar());
			ConstraintNode[] dcns = new ConstraintNode[cs.size()];
			int i = 0;
			for (RConstraint<?> c : cs)
				dcns[i++] = new ConstraintNode(this, c);
			return dcns;
		}
	}
	// public for Interchange's sake
	public class ConstraintNode extends ReasoningNode<DecisionNode, DecisionNode> {
		ConstraintNode(DecisionNode parent, RConstraint<?> theConstraint) {
			super(parent);
			this.constraint = theConstraint;
		}

		RConstraint<?> constraint;
		public RConstraint<?> getConstraint() { return constraint; }

		@Override
		public String getTreeLabel() {
			return constraint.describeEffectOnVar();
		}
		
		private void scanConstraint(RNode<?> n, ArrayList<Decision> ds) {
			if (n instanceof RCvarRef) {
				RCvarRef n2 = GenericNonsense.workaroundCast(RCvarRef.class, n);
				Decision d = n2.getDecisionOnTheCvar();
				if (d != parent.decision && !ds.contains(d))
					ds.add(d);
			}
			RNode<?>[] nchildren = n.getTreeChildren();
			for (RNode<?> n2 : nchildren)
				scanConstraint(n2, ds);
		}

		@Override
		protected DecisionNode[] computeTreeChildren() {
			// We'll put the children in traversal order...
			ArrayList<Decision> ds = new ArrayList<Decision>();
			// ...but in the case of an RAssignmentConstraint we put the (important) source first!  Hahaha!
			if (constraint instanceof RAssignmentConstraint)
				scanConstraint(constraint, ds);
			scanConstraint(constraint.findConstraintTop().getNode(), ds);
			DecisionNode[] children1 = new DecisionNode[ds.size()];
			int i = 0;
			for (Decision d : ds)
				children1[i++] = new DecisionNode(this, d);
			return children1;
		}
	}
	
	class ViewContentProvider extends TreelikeContentProvider {
		DecisionNode[] rootA;
		
		public void inputChanged(Viewer v, Object oldInput,
			Object newInput) {
			/*
			 * When the ReasoningView wants to change decisions,
			 * it calls this so that the reasoningViewer mechanism
			 * understands. We save the change into
			 * "decision" during this call. Seems strange, but
			 * this way no one gets confused.
			 */
			ReasoningView.this.mainDecision = (Decision) newInput;
			rootA = new DecisionNode[] { new DecisionNode(null, mainDecision) };
		}
		public void dispose() {}
		public Treelike<?>[] getElements(Object parent) {
			if (parent == mainDecision)
				return rootA;
			else
				return getChildren(parent);
		}
	}
	
	class ViewLabelProvider extends TreelikeLabelProvider {}
	
	private Observer infstatsObserver = new Observer() {
		public void update(Observable changed, Object change) {
			viewArea.getDisplay().asyncExec(new Runnable() {
				public void run() {
					refresh();
				}
			});
		}
	};

	public ReasoningView() {}

	void doRevealSomething() {
		// event.getSelection() comes to the same
		ISelection selection = reasoningViewer.getSelection();
		// Make sure other views know about our selection even though we are only briefly the active part
		GlobalSelectionListener.fireSelection(getSite().getPage(), selection);
		if (!selection.isEmpty()) {
			ReasoningNode n = (ReasoningNode) ((IStructuredSelection) selection).getFirstElement();
			if (n instanceof DecisionNode)
				UIUtils.revealDecision(((DecisionNode) n).decision);
			else if (n instanceof ConstraintNode) {
				ConstraintNode n2 = (ConstraintNode) n;
				RLocationNode ln = n2.constraint.getLocationNode();
				if (ln == null) ln = n2.constraint.findConstraintTop().getNode().getLocationNode();
				if (ln != null)
					UIUtils.revealRLocationNode(ln);
			}
		}
	}
	
	@Override
	public void createPartControl(Composite parent) {
		viewArea = parent;
		viewAreaLayout = new FormLayout();
		viewArea.setLayout(viewAreaLayout);

		reasoningViewer = new TreeViewer(parent, SWT.MULTI
			| SWT.H_SCROLL | SWT.V_SCROLL);
		reasoningViewer.setContentProvider(new ViewContentProvider());
		reasoningViewer.setLabelProvider(new ViewLabelProvider());
		// HMMM What happens if there are cycles in the reasoning?
		// We can simply expand a few levels instead of everything, but what if the user expands all?
		//reasoningViewer.setAutoExpandLevel(AbstractTreeViewer.ALL_LEVELS);
		reasoningViewer.setAutoExpandLevel(5);

		label = new Label(viewArea, SWT.LEFT | SWT.WRAP);
		setMainDecision(null);

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
		reasoningViewer.getControl().setLayoutData(ifd);

		/*goToAction = new Action() {
			@Override
			public void run() {
				doRevealSomething();
			}
		};*/
		/*reasoningViewer
			.addDoubleClickListener(new IDoubleClickListener() {
				public void doubleClick(DoubleClickEvent event) {
					goToAction.run();
				}
			});*/
		// Reveal-on-select makes it quicker to browse
		reasoningViewer.addSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent event) {
				doRevealSomething();
			}
		});
		
		getSite().setSelectionProvider(reasoningViewer);
		UIUtils.register(this);
	}
	
	// We don't use a GlobalSelectionListener; instead we focus on a Decision
	// when it is revealed by UIUtils.revealDecision.
	void setMainDecision(Decision d) {
		if (mainDecision != null)
			mainDecision.getOwningInfstats().getDir().deleteObserver(
				infstatsObserver);
		reasoningViewer.setInput(d);
		if (d == null)
			label.setText("Empty.  Double-click a decision.");
		else
			label.setText("Reasoning behind "
				+ d.getCvar().prettyString());
		viewArea.layout();
		if (mainDecision != null)
			mainDecision.getOwningInfstats().getDir().addObserver(
				infstatsObserver);
	}

	// Try to preserve a decision over updates
	void refresh() {
		Cvar cvar = mainDecision.getCvar();
		DirInfstats newInfstats = mainDecision.getOwningInfstats().getDir()
			.getCurInfstats();
		if (newInfstats == null)
			setMainDecision(null);
		else {
			Decision newDecision = newInfstats
				.getDecisionOnCvar(cvar);
			setMainDecision(newDecision);
		}
	}

	@Override
	public void setFocus() {
		reasoningViewer.getControl().setFocus();
	}

	@Override
	public void dispose() {
		super.dispose();
		UIUtils.unregister(this);
	}
}
