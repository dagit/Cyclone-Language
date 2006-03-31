package general.util;

public abstract class ObjectUtils {
	private ObjectUtils() {}

	public static boolean equal(Object x, Object y) {
		return x == null ? y == null : x.equals(y);
	}
}
