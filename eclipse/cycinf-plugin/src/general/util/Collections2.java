package general.util;

import java.util.*;

public abstract class Collections2 {

	private static class UnmodifiableKeyedSet<K, V> implements
		KeyedSet<K, V> {
		private final KeyedSet<K, V> back;
		private final Set<V> unmod1;
		UnmodifiableKeyedSet(KeyedSet<K, V> back) {
			this.back = Validator.nonnull(back, "back");
			this.unmod1 = Collections.unmodifiableSet(this.back);
		}
		public Keyer<K, V> getKeyer() {
			return back.getKeyer();
		}
		public boolean add(V v) {
			throw new UnsupportedOperationException();
		}
		public V add(V v, int conflictBehavior, int equalBehavior) {
			throw new UnsupportedOperationException();
		}
		public V lookup(K k) {
			return back.lookup(k);
		}
		public int size() {
			return unmod1.size();
		}
		public boolean isEmpty() {
			return unmod1.isEmpty();
		}
		public boolean contains(Object o) {
			return unmod1.contains(o);
		}
		public Iterator<V> iterator() {
			return unmod1.iterator();
		}
		public Object[] toArray() {
			return unmod1.toArray();
		}
		public <T> T[] toArray(T[] a) {
			return unmod1.toArray(a);
		}
		public boolean remove(Object o) {
			return unmod1.remove(o);
		}
		public boolean containsAll(Collection<?> c) {
			return unmod1.containsAll(c);
		}
		public boolean addAll(Collection<? extends V> c) {
			return unmod1.addAll(c);
		}
		public boolean retainAll(Collection<?> c) {
			return unmod1.retainAll(c);
		}
		public boolean removeAll(Collection<?> c) {
			return unmod1.removeAll(c);
		}
		public void clear() {
			unmod1.clear();
		}
	}

	public static final <K, V> KeyedSet<K, V> unmodifiableKeyedSet(
		KeyedSet<K, V> back) {
		return new UnmodifiableKeyedSet<K, V>(back);
	}
}
