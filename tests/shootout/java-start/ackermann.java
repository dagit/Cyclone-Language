// $Id: ackermann.java,v 1.2 2004-09-07 20:33:25 mwh Exp $
// http://www.bagley.org/~doug/shootout/

public class ackermann {
    public static void main(String[] args) {
	int num = Integer.parseInt(args[0]);
	//@START
	System.out.println("Ack(3," + num + "): " + Ack(3, num));
	//@END
    }
    public static int Ack(int m, int n) {
	return (m == 0) ? (n + 1) : ((n == 0) ? Ack(m-1, 1) :
				     Ack(m-1, Ack(m, n - 1)));
    }
}
