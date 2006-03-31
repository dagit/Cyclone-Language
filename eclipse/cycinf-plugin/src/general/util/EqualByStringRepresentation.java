package general.util;

public abstract class EqualByStringRepresentation {
	@Override
	public abstract String toString();

	@Override
	public final boolean equals(Object that) {
		return this.getClass() == that.getClass()
			&& this.toString().equals(that.toString());
	}

	@Override
	public final int hashCode() {
		return toString().hashCode();
	}
}
