package cycinf.ui;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.text.source.IVerticalRulerInfo;
import org.eclipse.ui.texteditor.*;

public class CycinfSelectRulerActionDelegate extends AbstractRulerActionDelegate {

	@Override
	protected IAction createAction(ITextEditor editor, IVerticalRulerInfo rulerInfo) {
		return new CycinfSelectRulerAction(editor, rulerInfo);
	}

}
