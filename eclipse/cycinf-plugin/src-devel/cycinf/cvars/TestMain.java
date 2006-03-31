package cycinf.cvars;

import java.io.*;

import cycinf.util.ParseException;

public abstract class TestMain {
	private TestMain() {}

	public static void main(String[] args) throws IOException {
		BufferedReader in = new BufferedReader(new InputStreamReader(
			System.in));
		for (;;) {
			System.out
				.print("Enter a cvar name and I'll interpret it: ");
			String cvarname = in.readLine();
			if (cvarname == null)
				break;
			try {
				Cvar cvar = Cvar.parse(cvarname);
				System.out.println("That is the "
					+ cvar.prettyString() + ".");
			} catch (ParseException pe) {
				System.out
					.println("I don't understand that cvar:");
				pe.printStackTrace(System.out);
			}
		}
		System.out.println("\nGoodbye.");
	}
}
