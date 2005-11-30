/*
  An example demonstrating that naively intersecting a polymorphic
  field insensitive alias analysis with a monomorphic field sensitive
  alias analysis is less precise than a combined polymorphic field
  sensitive analysis.

  The key to this example is the call to ex.f.m(). By inspection, we
  see that ex.f can only contain an F instance, so the m() method
  called is F.m. 

  The field insensitive polymorphic analysis can't resolve this call
  because f and g have the same points-to sets.

  The monomorphic field sensitive analysis can't resolve this call
  because the monomorphic call to id() merges both of id's contexts,
  so f contains a G instance.


*/

class F {
    
    F m() { return new F(); }

}

class G extends F {
    F m() { return new G(); }
}

class Example {
    F f;
    F g;

    static F id(F arg) { return arg; }


    public static void main(String[] args) {
	Example ex = new Example();

	ex.f = id(new F()); 	
	ex.g = id(new G());

	ex.f = ex.f.m();
    }

}