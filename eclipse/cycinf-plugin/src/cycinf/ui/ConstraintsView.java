package cycinf.ui;

import java.util.Collection;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.*;

import cycinf.reasoning.*;
import cycinf.util.Interchange;

// Table view of all constraints
// OldConstraint expression string

public class ConstraintsView extends InfstatsPartView<RConstraintTop> {
	public static final String PART_ID = "cycinf.ConstraintsView";

	private static final int CONSTRAINT_STRING_COLUMN = 0;

	@Override
	String getColumnText(RConstraintTop c, int index) {
		switch (index) {
		case CONSTRAINT_STRING_COLUMN:
			return c.getNode().getBack().toString();
		default:
			throw new IllegalArgumentException();
		}
	}

	@Override
	void doRevealElement(RConstraintTop c) {
		UIUtils.revealConstraint(c);
	}

	public ConstraintsView() {
		super(RConstraintTop.class);
	}

	@Override
	void setUpTableColumns(Table constraintsTable) {
		TableColumn dNameCol = new TableColumn(constraintsTable, SWT.LEFT, CONSTRAINT_STRING_COLUMN);
		dNameCol.setText("Constraint expression");
		dNameCol.setWidth(700);
	}

	@Override
	Collection<RConstraintTop> getElements() {
		return is.getConstraints();
	}

	@Override
	RConstraintTop marshallElementFrom(Object o) {
		return Interchange.toRConstraintTop(o);
	}

	@Override
	String getInfstatsPartName() {
		return "constraints";
	}
}