package general.util;

public interface Keyer<K, V> {
	public abstract K getKeyFor(V v);
}
