package cycinf.cvars.ptrbnd;

import cycinf.cvars.Cvalue;

public class PointerKind extends Cvalue {
	private final String valstr, comstr;

	private PointerKind(String valstr, String comstr) {
		this.valstr = valstr;
		this.comstr = comstr;
	}

	@Override
	public String valueString() {
		return valstr;
	}

	public String commonString() {
		return comstr;
	}

	@Override
	public String prettyString() {
		return comstr;
	}

	public static final PointerKind THIN = new PointerKind(
		"@thin @numelts{valueof_t(1U)}", "thin");
	public static final PointerKind FAT = new PointerKind("@fat", "fat");

	public static PointerKind valueParse(String s) {
		if (THIN.valueString().equals(s))
			return THIN;
		else if (FAT.valueString().equals(s))
			return FAT;
		else
			return null;
	}

	public static PointerKind commonParse(String s) {
		if (THIN.commonString().equals(s))
			return THIN;
		else if (FAT.commonString().equals(s))
			return FAT;
		else
			return null;
	}
}
