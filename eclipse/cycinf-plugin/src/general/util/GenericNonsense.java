package general.util;

import java.util.*;

public abstract class GenericNonsense {
	private GenericNonsense() {}

	// Since some Eclipse libraries have not yet been generified, they provide raw collections;
	// this method copies a raw collection into a generic one.
	public static final <X> ArrayList<X> receiveRawCollection(Class<X> clazzX, Collection raw) {
		ArrayList<X> cooked = new ArrayList<X>();
		for (Object o : raw)
			cooked.add(clazzX.cast(o));
		return cooked;
	}
	
	/* This works around the annoying, pointless unchecked warning
	 * "The cast to X is actually checking against the erased type X".
	 * See https://bugs.eclipse.org/bugs/show_bug.cgi?id=115691 */
	public static final <X> X workaroundCast(Class<X> clazzX, Object o) {
		return clazzX.cast(o);
	}
}
