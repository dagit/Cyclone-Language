package cycinf.ui;

import java.text.DateFormat;
import java.util.*;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.viewers.*;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.ui.part.ViewPart;

import cycinf.infstats.*;

public abstract class InfstatsPartView<Element extends BelongsToInfstats> extends ViewPart {
	final Class<Element> clazzElement;
	Element asElement(Object o) {
		return clazzElement.cast(o);
	}
	boolean isElement(Object o) {
		return clazzElement.isInstance(o);
	}
	
	DirTracker dir = null;
	DirInfstats is = null;
	
	Composite viewArea;
	Layout viewAreaLayout;
	Label label;
	
	TableViewer theTableViewer;
	Action goToAction;
	
	// add column declarations
	
	abstract String getColumnText(Element e, int index);
	
	abstract Collection<Element> getElements(); // from is
	
	class ViewContentProvider implements IStructuredContentProvider {
		public void inputChanged(Viewer v, Object oldInput,
			Object newInput) {
			/*
			 * When the view wants to change folders, it
			 * calls this so that the viewer mechanism understands.
			 * We save the change into the DirInfstats during this
			 * call. Seems strange, but this way no one gets
			 * confused.
			 */
			is = (DirInfstats) newInput;
		}
		public void dispose() {}
		// Not Element[] because of generic-array nastiness
		public Object[] getElements(Object parent) {
			if (parent == is) {
				Collection<Element> els = InfstatsPartView.this.getElements();
				return els.toArray(); // uses is
			} else
				throw new IllegalArgumentException("The input should be the current infstats");
		}
	}
	class ViewLabelProvider extends LabelProvider implements
	ITableLabelProvider {
		public String getColumnText(Object obj, int index) {
			return InfstatsPartView.this.getColumnText(asElement(obj), index);
		}
		public Image getColumnImage(Object obj, int index) {
			return null;
		}
	}
	
	abstract void doRevealElement(Element e);
	
	void doRevealElement1() {
		ISelection selection = theTableViewer.getSelection();
		// event.getSelection() comes to the same
		if (!selection.isEmpty()) {
			Element e = asElement(((IStructuredSelection) selection).getFirstElement());
			doRevealElement(e);
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
	
	public InfstatsPartView(Class<Element> clazzElement) {
		this.clazzElement = clazzElement;
	}
	
	// override me!
	Element marshallElementFrom(Object o) {
		return isElement(o) 	? asElement(o) : null;
	}
	
	abstract void setUpTableColumns(Table theTable);
	
	GlobalSelectionListener<Object> globalSelectionListener;
	
	private void setUpViewer() {
		theTableViewer = new TableViewer(viewArea, SWT.MULTI
			| SWT.H_SCROLL | SWT.V_SCROLL);
		theTableViewer.setContentProvider(new ViewContentProvider());
		theTableViewer.setLabelProvider(new ViewLabelProvider());
		getSite().setSelectionProvider(theTableViewer);
		Table theTable = theTableViewer.getTable();
		
		setUpTableColumns(theTable);
		theTable.setHeaderVisible(true); // Helpful [Let's try disabling this to get more screen space]
		
		goToAction = new Action() {
			@Override
			public void run() {
				doRevealElement1();
			}
		};
		theTableViewer
		.addDoubleClickListener(new IDoubleClickListener() {
			public void doubleClick(DoubleClickEvent event) {
				goToAction.run();
			}
		});
	}
	
	@Override
	public void createPartControl(Composite parent) {
		viewArea = parent;
		viewAreaLayout = new FormLayout();
		viewArea.setLayout(viewAreaLayout);
		
		label = new Label(viewArea, SWT.LEFT | SWT.WRAP);
		
		setUpViewer();
		
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
		theTableViewer.getControl().setLayoutData(ifd);
		
		label.setText("Empty.  Click a folder in the Navigator.");
		
		/*
		 * Now that we're initialized, we can start listening for folder.
		 * selections.  If a folder is already selected, the
		 * GlobalSelectionListener will notify us as it is being constructed.
		 */
		globalSelectionListener = new GlobalSelectionListener<Object>(this) {
			@Override
			protected Object wantObject(Object o) {
				return (o instanceof IContainer) ? o : marshallElementFrom(o);
			}
			@Override
			protected void selectObjects(List<Object> objects) {
				/* We could have a lot of IContainers and a lot of Elements here...
				 * We show the infstats of the first IContainer or Element and then
				 * select all Elements in that infstats. */
				Object first = objects.get(0);
				DirTracker dt = (first instanceof IContainer)
					? InfstatsManager.getManager().getTrackerFor((IContainer) first)
					: ((BelongsToInfstats) first).getOwningInfstats().getDir();
				useDirectory(dt);
				theTableViewer.setSelection(new StructuredSelection(objects), true);
			}
		};
		getSite().getPage().addPostSelectionListener(globalSelectionListener);
	}
	
	void useDirectory(DirTracker newDir) {
		if (dir != newDir) {
			if (dir != null)
				dir.deleteObserver(infstatsObserver);
			this.dir = newDir;
			if (dir != null)
				newDir.addObserver(infstatsObserver);
			refresh();
		}
	}
	
	abstract String getInfstatsPartName();
	
	void refresh() {
		theTableViewer.setInput(dir == null ? null : dir.getCurInfstats());
		if (dir == null)
			label.setText("Empty.  Click a folder in the Navigator.");
		else if (is == null)
			label.setText(dir.getPath() /*+ " " + getInfstatsPartName()*/
				+ ": Inference data never loaded");
		else
			label.setText(dir.getPath() /*+ " " + getInfstatsPartName()*/ + " as of "
				+ describeTime(is.getLoadDate()));
		viewArea.layout();
	}
	
	private static final DateFormat df = DateFormat.getDateTimeInstance(
		DateFormat.SHORT, DateFormat.SHORT);
	private static String describeTime(Date date) {
		return df.format(date);
	}
	
	@Override
	public void setFocus() {
		theTableViewer.getControl().setFocus();
	}
	
	@Override
	public void dispose() {
		// We're about to be disposed: we no longer care about new folders/data
		globalSelectionListener.unregister();
		if (dir != null)
			dir.deleteObserver(infstatsObserver);
		super.dispose();
	}
}
