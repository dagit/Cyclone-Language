package cycinf.ui;

import cycinf.CycinfPlugin;

import org.eclipse.jface.text.source.IVerticalRulerInfo;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.texteditor.*;

// Thanks to JavaSelectAnnotationRulerAction!
public class CycinfSelectRulerAction extends SelectMarkerRulerAction {
	ITextEditor editor;

	public CycinfSelectRulerAction(ITextEditor editor, IVerticalRulerInfo rulerInfo) {
		super(CycinfPlugin.getResourceBundle(), "CycinfSelectRulerAction.", editor, rulerInfo);
		this.editor = editor;
	}
	
	@Override
	public void run() {
		super.run();
		GlobalSelectionListener.fireSelection(editor.getSite().getPage(), new StructuredSelection(getMarkers()));
	}
}
