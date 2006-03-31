package cycinf.cvars.ptrbnd;

import cycinf.cvars.*;
import cycinf.infstats.DirInfstats;
import cycinf.util.SourceLocation;

public abstract class PointerKindDecisionParser {
	private PointerKindDecisionParser() {}

	// private static final Pattern decisionLinePattern =
	// Pattern.compile("^\\s*(\\w+)\\s+([^:]+):\\(?(\\d+)\\S*\\s+(\\w+)\\s+(\\w+)\\s*$");
	// target, file:line, orig, inf

	public static Decision parse(DirInfstats is, String line) {
		try {
			String[] fields = line.split("\\s+");
			Cvar cvar = Cvar.parse(fields[0]);
			SourceLocation loc = SourceLocation.parse(fields[1]);
			if (cvar instanceof PointerKindCvar) {
				PointerKindCvar cvar2 = (PointerKindCvar) cvar;
				if (fields.length != 4) {
					System.err
						.println("Wrong number of fields in infstats output");
					is.logParseError();
					return null;
				}
				PointerKind origKind, infKind;
				origKind = PointerKind.commonParse(fields[2]);
				infKind = PointerKind.commonParse(fields[3]);
				PointerInferenceDecision id = new PointerInferenceDecision(
					is, loc, cvar2, origKind, infKind);
				return id;
			} else
				return null; // OK for now
		} catch (Exception e) {
			e.printStackTrace(System.err);
			is.logParseError();
			return null;
		}
	}
}
