// $Id: random.java,v 1.3 2004-10-09 17:15:47 mwh Exp $
// http://www.bagley.org/~doug/shootout/

import java.text.*;

public class random {

    public static final long IM = 139968;
    public static final long IA = 3877;
    public static final long IC = 29573;

    public static void main(String args[]) {
	int x = Integer.parseInt(args[0]) - 1;
	//@START
	int N = x;
	NumberFormat nf = NumberFormat.getInstance();
	nf.setMaximumFractionDigits(9);
	nf.setMinimumFractionDigits(9);
	nf.setGroupingUsed(false);

	while (N-- > 0) {
	    gen_random(100);
	}
	System.out.println(nf.format(gen_random(100)));
	//@END
    }

    public static long last = 42;
    public static double gen_random(double max) {
	return( max * (last = (last * IA + IC) % IM) / IM );
    }
}
