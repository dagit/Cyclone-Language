/* An Earley parser written in haxe. */

typedef Parseforest = Array<Parsetree>;
typedef Parsetree = {
  a:Int,               // number of nonterminal
  l:Int,               // left position
  r:Int,               // right position
  c:Array<Parseforest> // children
};

typedef Eitem = {
  dstate:Int, // dfa state
  back:Int    // back pointer -- where parse of non-terminal started
};

typedef Render = {
  s:String,l:Int,r:Int,c:Array<Render>
};

class Earley {

  /*** State of the Earley parser ***/
  /* Earley states are stored in consecutive elements of
     the array eitems[] of Earley items.

     The stateIndexes[] array indicates where each Earley
     state begins.  eitems[stateIndexes[i]] is the first
     Earley item of Earley state i, and the rest of the
     items of state i follow, up to eitems[stateIndexes[i+1]],
     or until then end of eitems[] if stateIndexes[i+1] does not
     exist.
  */
  static var eitems:Array<Eitem> = new Array();
  static var stateIndexes:Array<Int> = new Array();

  /* If pred[i] == [..j..], eitems[j] is a predecessor of eitems[i].
     If pred[i] == null, eitems[i] has no predecessor.
  */
  static var pred:Array<Array<Int>> = new Array();
  static function addPred(n:Int,p:Null<Int>) {
    if (p == null) return;
    var old = pred[n];
    if (old == null) {
//      flash.Lib.trace("pred["+n+"] += "+ p);
      pred[n] = [p];
      return;
    }
    for (p2 in old) {
      if (p == p2) return;
    }
//    flash.Lib.trace("pred["+n+"] += "+ p);
    pred[n].push(p);
  }

  /* If retn[i] == [..j..], eitems[j] is a return predecessor of eitems[i].
     If retn[i] == null, eitems[i] has no return predecessor.
  */
  static function debugRetn(n:Int,o:{pred:Int,retn:Int,a:Int,r:Int}) {
    flash.Lib.trace("retn["+n+"] += {pred:"+ o.pred
          +",retn:"+o.retn
          +",a:"+o.a
          +",r:"+o.r
          +"}");
  }
  static var retn:Array<Array<{pred:Int,retn:Int,a:Int,r:Int}>> = new Array();
  static function addRetn(n:Int,o:{pred:Int,retn:Int,a:Int,r:Int}) {
    if (o == null) return;
    var old = retn[n];
    if (old == null) {
//      debugRetn(n,o);
      retn[n] = [o];
      return;
    }
    for (o2 in old) {
      if (o.pred == o2.pred && o.retn == o2.retn && o.a == o2.a) return;
    }
//    debugRetn(n,o);
    retn[n].push(o);
  }

  static var calloutAction = 256;
  static function pushClosure(ei:Eitem,p:Null<Int>,o:{pred:Int,retn:Int,a:Int,r:Int}) {
    var begin = stateIndexes[stateIndexes.length - 1];
    var dstate = ei.dstate;
    var back = ei.back;
    var elen = eitems.length;
    for (n in begin...elen) {
      var ei2 = eitems[n];
      if (ei2.dstate == dstate && ei2.back == back) {
        addPred(n,p);
        addRetn(n,o);
        return;
      }
    }
    eitems.push(ei);
//    flash.Lib.trace("eitems["+(eitems.length-1)+"]=("+dstate+","+back+")");
    pred.push(null);
    addPred(elen,p);
    retn.push(null);
    addRetn(elen,o);
    var i = stateIndexes.length - 1;
    /* close under calls */
    var trans = Dfa.transitions(dstate);
    if (trans != null) {
      var t = trans(calloutAction);
      if (t != 0) {
        pushClosure({dstate:t,back:i},null,null);
      }
    }
    /* close under returns... */
    if (back == i) return; /* ...unless call parsed empty */
    /* for every completed symbol... */
    var attrs = Dfa.attributes(dstate);
    if (attrs == null) return;
    for (a in attrs) {
//      flash.Lib.trace("Returning on "+a+" to "+back);
      /* ... look at every DFA state in the back Earley state... */
      for (m in stateIndexes[back]...stateIndexes[back+1]) {
        var backItem = eitems[m];
        var backtrans = Dfa.transitions(backItem.dstate);
        if (backtrans == null) continue;
        var returnTarget = backtrans(a);
        if (returnTarget == 0) continue;
        pushClosure({dstate:returnTarget,back:backItem.back},null,{pred:m,retn:elen,a:a,r:i});
      }
    }
  }
  /* Try to parse the input.  Return an array of indices of successful
     eitems; failure is indicated by the empty array.
  */
  static function init() {
    eitems = [];
    stateIndexes = [];
    pred = [];
    retn = [];
  }
  static function parse(input:String) {
    init();
    stateIndexes.push(0);
//    flash.Lib.trace("stateIndexes["+(stateIndexes.length-1)+"]="+stateIndexes[stateIndexes.length-1]);
    pushClosure({dstate:1,back:0},null,null);
    // loop invariant: eitems[stateIndexes[i]] to end is Earley state i
    // we are about to scan
    var ilen = input.length;
    for (i in 0...ilen) {
//      flash.Lib.trace("input["+i+"]=="+input.charCodeAt(i));
      var len = eitems.length;
      stateIndexes.push(len);
      for (j in stateIndexes[i]...len) {
        var eitem = eitems[j];
        var dstate = eitem.dstate;
        var back = eitem.back;
//        flash.Lib.trace("Looking at ("+dstate+","+back+")");
        var trans = Dfa.transitions(dstate);
        if (trans == null) continue;
        var k = trans(input.charCodeAt(i));
        if (k != 0) {
          pushClosure({dstate:k,back:back},j,null);
        }
      }
      // if (eitems.length == len) break; // no eitems added, we are done
    }
    var result = new Array();
    for (j in stateIndexes[ilen]...eitems.length) {
      var eitem = eitems[j];
      if (eitem.back == 0 &&
          Dfa.attributes(eitem.dstate) != null &&
          Dfa.attributes(eitem.dstate).length == 0) {
        result.push(j);
      }
    }
    return result;
  }
  static function prefixAllImp(t:Parsetree,fs:Array<Parseforest>) {
    for (f in fs)
      f.unshift(t);
  }
  static function dr(o:{pred:Int,retn:Int,a:Int,r:Int}):Parsetree {
    /* Parsed an o.a starting at eitem o.pred ending at eitem o.retn. */
    var hDerivations = dh(o.retn);
    var a = o.a;
    var left = eitems[o.retn].back;
    var right = o.r;
    return {a:a,l:left,r:right,c:hDerivations};
  }
  static function dh(i:Int):Array<Parseforest> {
    var derivations = [];
    if (i < retn.length && retn[i] != null) {
      for (o in retn[i]) {
        if (o == null) break;
        var predDerivations = dh(o.pred);
        var childTree = dr(o);
        for (d in predDerivations)
          d.push(childTree);
        derivations = derivations.concat(predDerivations);
      }
    }
    if (i < pred.length && pred[i] != null) {
      for (pr in pred[i]) {
        var predDerivations = dh(pr);
        derivations = derivations.concat(predDerivations);
      }
    }
    if (derivations.length == 0)
      return [[]];
    else
      return derivations;
  }
  /* general */
  static function renderForest(f:Parseforest,choices:Array<Int>):Array<Render> {
    var result = [];
    for (t in f)
      result.push(renderL(t));
    return result;
  }
  static public function render(t:Parsetree,choices:Array<Int>):Render {
    var c:Array<Render> = [];
    if (t.c != null && t.c.length > 0) {
      var choice = choices.shift();
      if (choice >= t.c.length) choice = 0;
      c = renderForestL(t.c[choice]);
    }
    return {s:Dfa.act2symb(t.a),
            l:t.l,r:t.r,c:c};
  }
  /* left */
  static function renderForestL(f:Parseforest):Array<Render> {
    var result = [];
    for (t in f)
      result.push(renderL(t));
    return result;
  }
  static public function renderL(t:Parsetree):Render {
    var c:Array<Render> = [];
    if (t.c != null && t.c.length > 0)
      c = renderForestL(t.c[0]); // see here
    return {s:Dfa.act2symb(t.a),
            l:t.l,r:t.r,c:c};
  }
  /* right */
  static function renderForestR(f:Parseforest):Array<Render> {
    var result = [];
    for (t in f)
      result.push(renderR(t));
    return result;
  }
  static public function renderR(t:Parsetree):Render {
    var c:Array<Render> = [];
    if (t.c != null && t.c.length > 0)
      c = renderForestR(t.c[t.c.length-1]); // see here
    return {s:Dfa.act2symb(t.a),
            l:t.l,r:t.r,c:c};
  }
  static public function parseToRender(input:String):{left:Array<Render>,right:Array<Render>} {
    var r = parse(input);
    if (r == null || r.length == 0) return {left:[],right:[]};
    var apf = dh(r[0]);
    var left = renderForestL(apf[0]);
    var right = renderForestR(apf[apf.length-1]);
    return {left:left,right:right};
  }
  /* DEBUGGING */
  static function trParseforest(indent:String,f:Parseforest) {
    for (t in f) trParsetree(indent,t);
  }
  static function trParsetree(indent:String,t:Parsetree) {
    flash.Lib.trace(indent+"a:"+Dfa.act2symb(t.a)+", l:"+t.l+", r:"+t.r);
    for (f in t.c) trParseforest(indent+" ",f);
  }
  static function trRenderA(indent:String,a:Array<Render>) {
    for (r in a) trRender(indent,r);
  }
  static function trRender(indent:String,r:Render) {
    flash.Lib.trace(indent+"s:"+(r.s)+", l:"+r.l+", r:"+r.r);
    trRenderA(indent+" ",r.c);
  }
  static public function main() {
    flash.Lib.trace("Starting");
    var input = "a logout\r\n";
    var result = parse(input);
    if (result.length > 0) {
      flash.Lib.trace("YES! "+result[0]);
      for (f in dh(result[0])) trParseforest(" ",f);
      flash.Lib.trace("huhn");
      for (r in renderForestL(dh(result[0])[0])) trRender(" ",r);
      flash.Lib.trace("huhn");
    }
    else flash.Lib.trace("no.");
  }
}
