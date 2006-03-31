package cycinf.reasoning;

import cycinf.infstats.*;
import cycinf.infstats.DirInfstats;

public class RContext<RoleNode extends RNode<? extends RoleNode>> implements BelongsToInfstats {
	final DirInfstats owningInfstats;
	public final DirInfstats getOwningInfstats() {
		return owningInfstats;
	}

	RoleNode node;
	public final RoleNode getNode() {
		return node;
	}

	RContext(DirInfstats owningInfstats) {
		this.owningInfstats = owningInfstats;
	}
}
