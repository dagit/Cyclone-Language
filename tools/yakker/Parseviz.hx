import flash.display.Sprite;
import flash.events.Event;
import flash.events.MouseEvent;
import flash.display.DisplayObjectContainer;
import flash.text.TextField;
import flash.text.TextFormat;

class Color {
  public static function
  hsvtorgb(h:Float,s:Float,v:Float):Int {
    // h(ue): 0-360
    // s(aturation): 0-1.0
    // v(alue): 0-1.0
    var r, g, b;
    h %= 360;
    h /= 60;
    var i = Math.floor(h) % 6;
    var f = h-i;
    var p = v*(1-s);
    var q = v*(1-(s*f));
    var t = v*(1-(s*(1-f)));
    switch(i) {
    case 0: r=v; g=t; b=p;
    case 1: r=q; g=v; b=p;
    case 2: r=p; g=v; b=t;
    case 3: r=p; g=q; b=v;
    case 4: r=t; g=p; b=v;
    case 5: r=v; g=p; b=q;
    }
    var R =Math.round(r*255);
    var G =Math.round(g*255);
    var B =Math.round(b*255);
    return ((R<<16) | (G<<8) | B);
  }
  public static function random() {
    return hsvtorgb(Math.random()*360,1.0,1.0);
  }

  static var onethird = (1.0/3);
  static var twothirds = (2.0/3);
  static var onesixth = (1.0/6);
  public static function
  hsltorgb(h:Float,s:Float,l:Float):Int {
    // h(ue): 0-360
    // s(aturation): 0-1.0
    // l(uminance): 0-1.0
    if (s == 0) {
      var rgb = Math.round(l * 255) % 255;
      return ((rgb << 16) | (rgb << 8) | rgb);
    }
    var q;
    if (l < 0.5) q = l * (1.0 + s);
    else q = l + s - (l * s);
    var p = 2.0 * l - q;
    var k = h/360;
    var r = k + onethird;
    var g = k;
    var b = k - onethird;
    var normalize = function (c:Float):Int {
      if (c < 0) c += 1;
      else if (c > 1) c -= 1;
      if (c < onesixth)
        c = p + ((q-p) * 6 * c);
      else if (c < 0.5)
        c = q;
      else if (c < twothirds)
        c = p + ((q-p) * (twothirds - c) * 6);
      else
        c = p;
      return Math.round(c * 255);
    }
    var R = normalize(r);
    var G = normalize(g);
    var B = normalize(b);
    return ((R<<16) | (G<<8) | B);
  }
  private static var colorMap = new Hash();
  public static function
  colorFor(s:String):Int {
    if (colorMap.exists(s))
      return colorMap.get(s);
    var c = random();
    colorMap.set(s,c);
    return c;
  }
}

class Parseviz {
  static var gridHeight = 20;
  static var gridWidth = 12;
  public static function
  drawText(spr:DisplayObjectContainer,tf:TextFormat,
           x:Float,y:Float,s:String):Sprite
  {
    var c = new Sprite();
    for (i in 0...s.length) {
      var t:TextField = new TextField();
      t.defaultTextFormat = tf;
      t.autoSize = flash.text.TextFieldAutoSize.LEFT;
      t.selectable = false;
      t.text = s.charAt(i);
      t.x = i*gridWidth;
      t.y = 0;
      c.addChild(t);
    }
    c.x = x;
    c.y = y;
    spr.addChild(c);
    return c;
  }
  public static function
  drawSpanBelow(spr:DisplayObjectContainer,tf:TextFormat,
                x:Float,y:Float,i:Int,w:Int,z:Int,s:String):Sprite
  {
    var c = new Sprite();
    var t:TextField = new TextField();
    t.defaultTextFormat = tf;
    t.autoSize = flash.text.TextFieldAutoSize.LEFT;
    t.selectable = false;
    var color = Color.colorFor(s); // BEFORE chopping s
    if (s.length > w) {
      s = s.substr(0,w);
    }
    t.text = s;
    t.x = i * gridWidth;
    t.y = z * gridHeight;

    c.addChild(t);
    c.graphics.lineStyle(1,0x0,0.5);
    c.graphics.beginFill(color);
    c.graphics.drawRect(i*gridWidth,gridHeight,w*gridWidth,z*gridHeight);
    c.graphics.endFill();
    c.x = x;
    c.y = y;
    spr.addChild(c);
    return c;
  }
  static function drawSpansBelow(spr:DisplayObjectContainer,a:Array<Dynamic>) {
    for (x in a) {
      drawSpanBelow(spr,font,0,0,x.l,x.r-x.l,x.h,x.s);
      if (!Reflect.hasField(x,"c")) return;
      var a:Array<Dynamic> = x.c;
      drawSpansBelow(spr,a);
    }
  }
  public static function
  drawSpanAbove(spr:DisplayObjectContainer,tf:TextFormat,
                x:Float,y:Float,i:Int,w:Int,z:Int,s:String):Sprite
  {
    var c = new Sprite();
    var t:TextField = new TextField();
    t.defaultTextFormat = tf;
    t.autoSize = flash.text.TextFieldAutoSize.LEFT;
    t.selectable = false;
    var color = Color.colorFor(s); // BEFORE chopping s
    if (s.length > w) {
      s = s.substr(0,w);
    }
    t.text = s;
    t.x = i * gridWidth;
    t.y = -z * gridHeight;

    c.addChild(t);
    c.graphics.lineStyle(1,0x0,0.5);
    c.graphics.beginFill(color);
    c.graphics.drawRect(i*gridWidth,0,w*gridWidth,-z*gridHeight);
    c.graphics.endFill();
    c.x = x;
    c.y = y;
    spr.addChild(c);
    return c;
  }
  static function drawSpansAbove(spr:DisplayObjectContainer,a:Array<Dynamic>) {
    for (x in a) {
      drawSpanAbove(spr,font,0,0,x.l,x.r-x.l,x.h,x.s);
      if (!Reflect.hasField(x,"c")) return;
      var a:Array<Dynamic> = x.c;
      drawSpansAbove(spr,a);
    }
  }
  static function markHeight(x:Dynamic):Int {
    if (!Reflect.hasField(x,"c")) {
      Reflect.setField(x,"h",1);
      return 1;
    }
    var h = 0;
    var a:Array<Dynamic> = x.c;
    for (y in a) {
      var yh = markHeight(y);
      if (yh > h) h = yh;
    }
    h++;
    Reflect.setField(x,"h",h);
    return h;
  }
  static var font = new TextFormat("Courier",16,0x0);
  static var parseInput:String = "";
  static var parseAbove:Array<Dynamic> = [];
  static var parseBelow:Array<Dynamic> = [];
  static function drawTrees(spr:DisplayObjectContainer) {
    var tf = drawText(spr,font,0,0,parseInput);
    var maxHeightAbove = 0;
    for (x in parseAbove) {
      var h = markHeight(x);
      if (h > maxHeightAbove) maxHeightAbove = h;
    }
    var maxHeightBelow = 0;
    for (x in parseBelow) {
      var h = markHeight(x);
      if (h > maxHeightBelow) maxHeightBelow = h;
    }
    drawSpansBelow(spr,parseBelow);
    drawSpansAbove(spr,parseAbove);
    spr.y = maxHeightAbove * gridHeight; // puts top-left at origin
  }
  static function decode(t:String):Array<Dynamic> {
    var c:Array<Dynamic> = [];
    try {
      var xml = Xml.parse(t);
      if (xml == null) return c;
      for (x in xml.elements()) {
        c.push(xml2obj(x));
      }
    }
    catch (e:Dynamic) {
    }
    return c;
// <t s="lkjfdsfds" l="1" r="9"></t>
  }
  static function xml2obj(xml:Xml):Dynamic {
    if (xml == null) return null;

    var s = xml.get('s');
    var l = Std.parseInt(xml.get('l'));
    var r = Std.parseInt(xml.get('r'));
    var c:Array<Dynamic> = [];
    for (x in xml.elements()) {
      c.push(xml2obj(x));
    }
    
    return {s:s,l:l,r:r,c:c};
  }
  /* Main entry point */
  public function start(app:Dynamic) {
    // In fact app:Application, but it is convenient to use Dynamic to
    // more easily access UI components


    /* mainCanvas is a mx.containers.Canvas, and its addChild method
       requires that the argument implement IUIComponent, which most
       flash DisplayObjects do not.  Therefore we add a regular
       DisplayObject as a child using rawChildren, and the rest of the
       program will add children to c as usual. */
    var c = new Sprite();
    app.mainCanvas.rawChildren.addChild(c);

    app.renderButton.addEventListener
    (MouseEvent.CLICK,
     function (e:Event) {
       while (c.numChildren > 0)
         c.removeChildAt(0);

       try {
         parseInput = app.parseInput.text;
         parseAbove = decode(app.parseAbove.text);
         parseBelow = decode(app.parseBelow.text);
         drawTrees(c);
       }
       catch (e:Dynamic) {
         drawText(c,font,0,0,"Exception!!");
       }
     });
  }
}

//         [{c:[{c:[{c:[],s:"LF",l:10,r:11}],s:"ws",l:10,r:11},{c:[{c:[{c:[{c:[],s:"DIGIT",l:8,r:9}],s:"int",l:8,r:9}],s:"v",l:8,r:9},{c:[{c:[],s:"ALPHA",l:6,r:7}],s:"id",l:6,r:7},{c:[{c:[],s:"SP",l:5,r:6}],s:"ws",l:5,r:6},{c:[{c:[{c:[],s:"DIGIT",l:3,r:4}],s:"int",l:3,r:4}],s:"v",l:3,r:4},{c:[{c:[],s:"ALPHA",l:1,r:2}],s:"id",l:1,r:2}],s:"v",l:0,r:10}],s:"wv",l:0,r:11}];
