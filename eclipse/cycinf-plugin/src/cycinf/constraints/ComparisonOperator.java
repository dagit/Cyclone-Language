package cycinf.constraints;

import cycinf.cvars.Cvalue;

public enum ComparisonOperator {
	EQUAL("=") {
		@Override
		public boolean evaluate(Cvalue value1, Cvalue value2) {
			return value1.equals(value2);
		}
	}/*
		 * , UNEQUAL { public final String symbol = "!"; @Override
		 * public boolean evaluate(Cvalue value1, Cvalue value2) {
		 * return !value1.equals(value2); } }
		 */;
	public final String symbol;
	ComparisonOperator(String symbol) {
		this.symbol = symbol;
	}
	public abstract boolean evaluate(Cvalue value1, Cvalue value2);
}
