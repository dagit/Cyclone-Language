package cycinf.cvars;

import general.util.EqualByStringRepresentation;

public abstract class CvarPiece extends EqualByStringRepresentation implements
	Pretty {
	@Override
	public abstract String toString();
	public abstract String prettyString();
}
