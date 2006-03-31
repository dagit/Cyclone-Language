package cycinf.cvars.ptrbnd;

import general.util.Validator;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.CoreException;

import cycinf.cvars.Decision;
import cycinf.infstats.*;
import cycinf.util.*;

public final class PointerInferenceDecision extends Decision {
	private final PointerKindCvar cvar;
	@Override
	public final PointerKindCvar getCvar() {
		return cvar;
	}

	private final PointerKind origKind, infKind;
	public final PointerKind getOrigKind() {
		return origKind;
	}
	public final PointerKind getInfKind() {
		return infKind;
	}
	@Override
	public final PointerKind getCvalue() {
		return getInfKind();
	}

	private SourceSearch ss;

	public PointerInferenceDecision(DirInfstats owningInfstats,
		SourceLocation loc, PointerKindCvar cvar,
		PointerKind origKind, PointerKind infKind) {
		super(owningInfstats, loc);
		this.cvar = Validator.nonnull(cvar, "cvar");
		this.origKind = Validator.nonnull(origKind, "origKind");
		this.infKind = Validator.nonnull(infKind, "infKind");
		ss = SourceSearch.findUsedTypeLayer(owningInfstats.getDir(),
			getLocation(), getCvar().getPointer());
	}

	@Override
	public String decisionName() {
		/*
		 * return getInfKind() == PointerKind.thin ? "Thin" +
		 * (getOrigKind() == PointerKind.fat ? " despite ? in source" :
		 * "") : "Fat" + (getOrigKind() == PointerKind.thin ? " despite *
		 * in source" : "");
		 */
		return getInfKind() == PointerKind.THIN ? "Thin" : "Fat";
	}

	@Override
	public String description() {
		String pointerDesc = getCvar().getPointer().prettyString();
		/*
		 * String decisionDesc = getInfKind() == PointerKind.thin ?
		 * "Thin" + (getOrigKind() == PointerKind.fat ? " despite ? in
		 * source" : "") : "Fat" + (getOrigKind() == PointerKind.thin ? "
		 * despite * in source" : "");
		 */
		String decisionDesc = getInfKind() == PointerKind.THIN ? "Thin"
			: "Fat";
		String caveat = ss.isConfident() ? ""
			: " (approximate location)";
		return decisionDesc + ": " + pointerDesc + caveat;
	}

	@Override
	public void mark() throws CoreException {
		if (getMarker() == null) {
			super.mark();
			if (ss.isConfident()) {
				getMarker().setAttribute(IMarker.CHAR_START,
					ss.getStartPos());
				getMarker().setAttribute(IMarker.CHAR_END,
					ss.getEndPos());
			}
		}
	}
}
