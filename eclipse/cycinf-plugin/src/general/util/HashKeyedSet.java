package general.util;

import java.util.*;

/**
 * A simple implementation of {@link KeyedSet} backed by a
 * {@link java.util.HashMap} and its
 * {@link java.util.HashMap#values value collection}.
 */
public class HashKeyedSet<K, V> implements KeyedSet<K, V> {
	private final Keyer<K, V> keyer;
	private final Map<K, V> theMap = new HashMap<K, V>();
	final Collection<V> theValues = theMap.values();

	public HashKeyedSet(Keyer<K, V> keyer) {
		this.keyer = Validator.nonnull(keyer, "keyer");
	}

	public int size() {
		return theValues.size();
	}

	public boolean isEmpty() {
		return theValues.isEmpty();
	}

	public boolean contains(Object o) {
		return theValues.contains(o);
	}

	private class KeyedSetIterator implements Iterator<V> {
		private Iterator<V> itr = theValues.iterator();
		public boolean hasNext() {
			return itr.hasNext();
		}
		public V next() {
			return itr.next();
		}
		public void remove() {
			itr.remove();
		}
	}

	public Iterator<V> iterator() {
		return new KeyedSetIterator();
	}

	public Object[] toArray() {
		return theValues.toArray();
	}

	public <T> T[] toArray(T[] a) {
		return theValues.toArray(a);
	}

	private boolean checkAdd(int behavior, V old) {
		switch (behavior) {
		case REPLACE:
			remove(old);
			return true;
		case IGNORE:
			return false;
		case THROW_EXCEPTION:
			throw new IllegalStateException();
		default:
			throw new IllegalArgumentException();
		}
	}

	private static boolean eq(Object x, Object y) {
		return (x == y) ? true : (x == null) ? false : x.equals(y);
	}

	public V add(V o, int conflictBehavior, int equalBehavior) {
		K key = keyer.getKeyFor(o);
		V old = theMap.get(key);
		if (old == null
			|| (eq(o, old) ? checkAdd(equalBehavior, old)
				: checkAdd(conflictBehavior, old)))
			theMap.put(key, o);
		return old;
	}

	public boolean add(V o) {
		return add(o, THROW_EXCEPTION, IGNORE) == null;
	}

	public boolean remove(Object o) {
		return theValues.remove(o);
	}

	public boolean containsAll(Collection<?> c) {
		return theValues.containsAll(c);
	}

	public boolean addAll(Collection<? extends V> c) {
		boolean changed = false;
		for (V o : c)
			changed |= add(o);
		return changed;
	}

	public boolean retainAll(Collection<?> c) {
		return theValues.retainAll(c);
	}

	public boolean removeAll(Collection<?> c) {
		return theValues.removeAll(c);
	}

	public void clear() {
		theValues.clear();
	}

	public Keyer<K, V> getKeyer() {
		return keyer;
	}

	public V replace(V v) {
		return theMap.put(keyer.getKeyFor(v), v);
	}

	public V lookup(K k) {
		return theMap.get(k);
	}

	@Override
	public boolean equals(Object o) {
		return theValues.equals(o);
	}

	@Override
	public int hashCode() {
		return theValues.hashCode();
	}

	@Override
	public String toString() {
		return theValues.toString();
	}
}
