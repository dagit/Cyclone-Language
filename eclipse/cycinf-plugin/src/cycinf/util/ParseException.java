package cycinf.util;

public class ParseException extends Exception {
	static final long serialVersionUID = 3141592653589793238L;
	public ParseException() {
		super();
	}
	public ParseException(String message, Throwable cause) {
		super(message, cause);
	}
	public ParseException(String message) {
		super(message);
	}
	public ParseException(Throwable cause) {
		super(cause);
	}
}
