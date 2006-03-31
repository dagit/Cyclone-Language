package cycinf.ui;

import org.eclipse.ui.*;
import org.eclipse.ui.console.IConsoleConstants;

public class InfBrowsingPerspective implements IPerspectiveFactory {
	public void createInitialLayout(IPageLayout layout) {
		String editorArea = layout.getEditorArea();
		
		/*layout.addView(DecisionsView.PART_ID, IPageLayout.TOP, 1/4f, editorArea);
		layout.addView(ConstraintsView.PART_ID, IPageLayout.RIGHT, 1/2f, DecisionsView.PART_ID);*/
		
		IFolderLayout left = layout.createFolder("left", IPageLayout.LEFT, 1/5f, editorArea);
		left.addView(IPageLayout.ID_RES_NAV);
		
		IFolderLayout topRight = layout.createFolder("topRight", IPageLayout.RIGHT, 5/7f, editorArea);
		topRight.addView(ReasoningView.PART_ID);
		topRight.addView(DecisionView.PART_ID);

		IFolderLayout bottomRight = layout.createFolder("bottomRight", IPageLayout.BOTTOM, 1/2f, "topRight");
		bottomRight.addView(ConstraintView.PART_ID);

		IFolderLayout traditionalBottom = layout.createFolder("bottom", IPageLayout.BOTTOM, 2/3f, editorArea);
		traditionalBottom.addView(DecisionsView.PART_ID);
		traditionalBottom.addView(ConstraintsView.PART_ID);
		traditionalBottom.addView(IPageLayout.ID_PROBLEM_VIEW);
		traditionalBottom.addView(IConsoleConstants.ID_CONSOLE_VIEW);
		
	}
}
