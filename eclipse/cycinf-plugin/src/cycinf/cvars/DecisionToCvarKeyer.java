package cycinf.cvars;

import general.util.Keyer;

public class DecisionToCvarKeyer implements Keyer<Cvar, Decision> {
	public final Cvar getKeyFor(Decision d) {
		return d.getCvar();
	}
}
