package cycinf.ui;

import java.util.Collection;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.*;

import cycinf.cvars.Decision;
import cycinf.util.Interchange;

// Table view of all decisions
// Decision name; cvar prettyString; location

public class DecisionsView extends InfstatsPartView<Decision> {
	public static final String PART_ID = "cycinf.DecisionsView";

	private static final int DECISION_NAME_COLUMN = 0;
	private static final int CVAR_PRETTY_COLUMN = 1;
	private static final int CVAR_NAME_COLUMN = 2;
	private static final int LOCATION_COLUMN = 3;

	@Override
	public String getColumnText(Decision id, int index) {
		switch (index) {
		case DECISION_NAME_COLUMN:
			return id.decisionName();
		case CVAR_PRETTY_COLUMN:
			return id.getCvar().prettyString();
		case CVAR_NAME_COLUMN:
			return id.getCvar().toString();
		case LOCATION_COLUMN:
			return id.getLocation().toString();
		default:
			throw new IllegalArgumentException();
		}
	}

	@Override
	void doRevealElement(Decision id) {
		UIUtils.notifyReasoningView(id);
		UIUtils.revealDecision(id);
	}

	public DecisionsView() {
		super(Decision.class);
	}

	@Override
	void setUpTableColumns(Table decisionsTable) {
		TableColumn dNameCol = new TableColumn(decisionsTable, SWT.LEFT, DECISION_NAME_COLUMN);
		dNameCol.setText("Value");
		dNameCol.setWidth(50);
		TableColumn tPrettyCol = new TableColumn(decisionsTable, SWT.LEFT, CVAR_PRETTY_COLUMN);
		tPrettyCol.setText("Cvar description");
		tPrettyCol.setWidth(400);
		TableColumn tNameCol = new TableColumn(decisionsTable, SWT.LEFT, CVAR_NAME_COLUMN);
		tNameCol.setText("Cvar name");
		tNameCol.setWidth(200);
		TableColumn locationCol = new TableColumn(decisionsTable, SWT.LEFT, LOCATION_COLUMN);
		locationCol.setText("Location");
		locationCol.setWidth(200);
	}

	@Override
	Collection<Decision> getElements() {
		return is.getDecisions();
	}

	@Override
	Decision marshallElementFrom(Object o) {
		return Interchange.toDecision(o);
	}

	@Override
	String getInfstatsPartName() {
		return "decisions";
	}
}