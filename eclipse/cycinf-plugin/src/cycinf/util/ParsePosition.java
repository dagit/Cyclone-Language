package cycinf.util;

public class ParsePosition extends java.text.ParsePosition implements Cloneable {
	public ParsePosition() {
		super(0);
	}
	public ParsePosition(int index) {
		super(index);
	}
	public void advance(int howMuch) {
		setIndex(getIndex() + howMuch);
	}
	@Override
	public ParsePosition clone() {
		try {
			return (ParsePosition) super.clone();
		} catch (CloneNotSupportedException e) {
			throw new Error(e);
		}
	}
}
