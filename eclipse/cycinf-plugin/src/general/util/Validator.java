package general.util;

public abstract class Validator {
	private Validator() {}

	private static final String something = "<something--see stack trace>";

	public static <X> X nonnull(X x, String what) {
		if (x != null)
			return x;
		else
			throw new NullPointerException("`" + what
				+ "\' must be nonnull; got null");
	}

	public static <X> X nonnull(X x) {
		return nonnull(x, something);
	}

	public static int nonnegative(int x, String what) {
		if (x >= 0)
			return x;
		else
			throw new IllegalArgumentException("`" + what
				+ "\' must be nonnegative; got "
				+ Integer.toString(x));
	}

	public static int nonnegative(int x) {
		return nonnegative(x, something);
	}
}
