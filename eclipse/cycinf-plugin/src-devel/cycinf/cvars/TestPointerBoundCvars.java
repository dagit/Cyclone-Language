package cycinf.cvars;

import java.util.*;

import junit.framework.TestCase;
import cycinf.util.ParseException;

public class TestPointerBoundCvars extends TestCase {

	static class TestCvar {
		final String locator, pretty;
		TestCvar(String locator, String pretty) {
			this.locator = locator;
			this.pretty = pretty;
		}
		void test() throws ParseException {
			Cvar cvar = Cvar.parse(locator);
			assertNotNull("cvar recognition", cvar);
			String regurgitatedCvarname = cvar.toString();
			assertEquals("regurgitated name of cvar", locator,
				regurgitatedCvarname);
			String prettyString = cvar.prettyString();
			assertEquals("interpretation of cvar", pretty,
				prettyString);
		}
	}

	List<TestCvar> testVars;

	public TestPointerBoundCvars() {
		testVars = new ArrayList<TestCvar>();
		testVars.add(new TestCvar("C_PTRBND_8Iringring_0",
			"pointer-bound cvar for ringring"));
		testVars.add(new TestCvar("C_PTRBND_8Idingdong_1",
			"pointer-bound cvar for 1 layer deep in dingdong"));
		testVars.add(new TestCvar("C_PTRBND_10Iblingbling_5",
			"pointer-bound cvar for 5 layers deep in blingbling"));
		testVars
			.add(new TestCvar("C_PTRBND_3Ifoo_3Ibar_3_2",
				"pointer-bound cvar for 2 layers deep in bar in function foo"));
		testVars
			.add(new TestCvar("C_PTRBND_3Ifoo_0_ARG2_1",
				"pointer-bound cvar for 1 layer deep in argument #3 to function foo"));
		testVars
			.add(new TestCvar("C_PTRBND_3Ifoo_0_FR_1",
				"pointer-bound cvar for 1 layer deep in return type of function foo"));
		testVars
			.add(new TestCvar(
				"C_PTRBND_8Idrawfunc_1_ARG0_2",
				"pointer-bound cvar for 2 layers deep in argument #1 to function 1 layer deep in drawfunc"));
		testVars
			.add(new TestCvar("C_PTRBND_AD7Ihandler_AF2_0",
				"pointer-bound cvar for field #3 of aggregate handler"));
		testVars
			.add(new TestCvar(
				"C_PTRBND_5Iqsort_0_ARG3_1_ARG0_0",
				"pointer-bound cvar for argument #1 to function 1 layer deep in argument #4 to function qsort"));
		testVars
			.add(new TestCvar(
				"C_PTRBND_DD15Iused_type_layer_DF3_5_1",
				"pointer-bound cvar for 1 layer deep in argument #6 to constructor #4 of datatype used_type_layer"));
		testVars
			.add(new TestCvar(
				"C_PTRBND_AD7Ihandler_AF2_1_ARG0_0",
				"pointer-bound cvar for argument #1 to function 1 layer deep in field #3 of aggregate handler"));
		testVars
			.add(new TestCvar("C_PTRBND_TD11Imagicstring_1",
				"pointer-bound cvar for 1 layer deep in typedef magicstring"));
	}

	public void testParseAndInterpret() throws ParseException {
		for (TestCvar t : testVars)
			t.test();
	}
}
