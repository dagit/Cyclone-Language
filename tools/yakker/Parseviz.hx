/**********************************************************************/
/**********************************************************************/

import flash.display.Sprite;
import mx.controls.TextArea;
import flash.events.Event;
import mx.events.ResizeEvent;
import mx.events.NumericStepperEvent;
import mx.events.FlexEvent;
import mx.controls.ToolTip;
import mx.managers.ToolTipManager;
import flash.events.MouseEvent;
import flash.events.IOErrorEvent;
import flash.display.DisplayObjectContainer;
import flash.text.TextField;
import flash.text.TextFormat;
import flash.geom.Rectangle;
import flash.geom.Point;

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
  static var zoom = 1.0;
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
    if (s.length > w)
      t.text = s.substr(0,w);
    else
      t.text = s;
    t.x = i * gridWidth;
    t.y = z * gridHeight;

    c.addChild(t);
    c.graphics.lineStyle(1,0x0,0.5);
    c.graphics.beginFill(Color.colorFor(s));
    c.graphics.drawRect(i*gridWidth,gridHeight,w*gridWidth,z*gridHeight);
    c.graphics.endFill();
    c.x = x;
    c.y = y;
    spr.addChild(c);

    var mouseOut = function (e:MouseEvent) {};
    var mouseOver = function (e:MouseEvent) {
      var pt = c.localToGlobal(new Point(i*gridWidth+c.width,c.y+c.height));
      var tt = ToolTipManager.createToolTip(s,pt.x,pt.y);
      mouseOut = function (e:MouseEvent) {
        ToolTipManager.destroyToolTip(tt);
        c.removeEventListener(MouseEvent.ROLL_OUT,mouseOut);
      };
      c.addEventListener(MouseEvent.ROLL_OUT,mouseOut);
    };
    c.addEventListener(MouseEvent.ROLL_OVER,mouseOver);

    return c;
  }
  static function drawSpansBelow(spr:DisplayObjectContainer,a:Array<Dynamic>) {
    for (x in a) {
      drawSpanBelow(spr,font,0,0,x.l,x.r-x.l,x.h,x.s);
      if (x.h == 1 || !Reflect.hasField(x,"c")) continue;
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
    if (s.length > w)
      t.text = s.substr(0,w);
    else
      t.text = s;
    t.x = i * gridWidth;
    t.y = -z * gridHeight;

    c.addChild(t);
    c.graphics.lineStyle(1,0x0,0.5);
    c.graphics.beginFill(Color.colorFor(s));
    c.graphics.drawRect(i*gridWidth,0,w*gridWidth,-z*gridHeight);
    c.graphics.endFill();
    c.x = x;
    c.y = y;
    spr.addChild(c);

    var mouseOut = function (e:MouseEvent) {};
    var mouseOver = function (e:MouseEvent) {
      var pt = c.localToGlobal(new Point(i*gridWidth+c.width,c.y-c.height));
      var tt = ToolTipManager.createToolTip(s,pt.x,pt.y);
      mouseOut = function (e:MouseEvent) {
        ToolTipManager.destroyToolTip(tt);
        c.removeEventListener(MouseEvent.ROLL_OUT,mouseOut);
      };
      c.addEventListener(MouseEvent.ROLL_OUT,mouseOut);
    };
    c.addEventListener(MouseEvent.ROLL_OVER,mouseOver);

    return c;
  }
  static function drawSpansAbove(spr:DisplayObjectContainer,a:Array<Dynamic>) {
    for (x in a) {
      drawSpanAbove(spr,font,0,0,x.l,x.r-x.l,x.h,x.s);
      if (x.h == 1 || !Reflect.hasField(x,"c")) continue;
      var a:Array<Dynamic> = x.c;
      drawSpansAbove(spr,a);
    }
  }
  static function markHeight(x:Dynamic,limit:Int):Int {
    if (limit == 1 || !Reflect.hasField(x,"c")) {
      Reflect.setField(x,"h",1);
      return 1;
    }
    var h = 0;
    var a:Array<Dynamic> = x.c;
    for (y in a) {
      var yh = markHeight(y,limit-1);
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
  static var maxHeightAbove = 0;
  static var limit = 0;
  static function drawTrees(spr:DisplayObjectContainer) {
    maxHeightAbove = 0; // needed if limit can change
    for (x in parseAbove) {
      var h = markHeight(x,limit);
      if (h > maxHeightAbove) maxHeightAbove = h;
    }
    var maxHeightBelow = 0;
    for (x in parseBelow) {
      var h = markHeight(x,limit);
      if (h > maxHeightBelow) maxHeightBelow = h;
    }
    var bg = new Sprite();
    spr.addChild(bg);
    bg.graphics.beginFill(0xffffff);
    bg.graphics.drawRect(0,
                         -maxHeightAbove * gridHeight,
                         parseInput.length * gridWidth,
                         (maxHeightAbove + maxHeightBelow + 1) * gridHeight);
    bg.graphics.endFill();
    var tf = drawText(spr,font,0,0,parseInput);
    drawSpansBelow(spr,parseBelow);
    drawSpansAbove(spr,parseAbove);
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
  static function loadFile(file:String,t:TextArea) {
    var ul:flash.net.URLLoader = new flash.net.URLLoader();
    ul.addEventListener
    (Event.COMPLETE,
     function (e:Event) {
       t.text = ul.data;
     });
    ul.addEventListener
    (IOErrorEvent.IO_ERROR,
     function (e:Event) {
       /* if file does not exist just ignore it */
     });
    ul.load(new flash.net.URLRequest(file));
  }
  /* Main entry point */
  public function start(app:Dynamic) {
    // In fact app:Application, but it is convenient to use Dynamic to
    // more easily access UI components

    loadFile("input",app.parseInput);
/*
    loadFile("left.xml",app.parseAbove);
    loadFile("right.xml",app.parseBelow);
*/


    /* mainCanvas is a mx.containers.Canvas, and its addChild method
       requires that the argument implement IUIComponent, which most
       flash DisplayObjects do not.  Therefore we add a regular
       DisplayObject as a child using rawChildren, and the rest of the
       program will add children to c as usual. */
    var c = new Sprite();
    app.mainCanvas.rawChildren.addChild(c);
//    c.opaqueBackground = 0xffffff;  // useless b/c doesn't get mouse
    var bg = new Sprite();
    c.addChild(bg);
    bg.graphics.beginFill(0xffffff,0.5);
    bg.graphics.drawRect(0,0,1,1);
    bg.graphics.endFill();

    c.scrollRect = new Rectangle(0,0,app.mainCanvas.width,app.mainCanvas.height);
    var resizeC = function () {
      var r = c.scrollRect;
      r.width = app.mainCanvas.width;
      r.height = app.mainCanvas.height;
      c.scrollRect = r;
      bg.scaleX = app.mainCanvas.width;
      bg.scaleY = app.mainCanvas.height;
    }
    resizeC();
    app.mainCanvas.addEventListener(ResizeEvent.RESIZE,
                                    function (e:Event) {
                                      resizeC();
                                    });
    var scrollX = 0.0;
    var scrollY = 0.0;
    var inScroll = function (e:MouseEvent) {
      var dX = e.stageX - scrollX;
      var dY = e.stageY - scrollY;
      scrollX = e.stageX;
      scrollY = e.stageY;
      var r = c.scrollRect;
      r.x -= dX;
      r.y -= dY;
      c.scrollRect = r;
      bg.x = r.x;
      bg.y = r.y;
    }
    var spr:Sprite = null;
    var stopScroll = function (e:MouseEvent) {}
    stopScroll = function (e:MouseEvent) {
      bg.removeEventListener(MouseEvent.MOUSE_MOVE,inScroll);
      bg.removeEventListener(MouseEvent.MOUSE_UP,stopScroll);
      bg.removeEventListener(MouseEvent.MOUSE_OUT,stopScroll);
      if (spr != null) {
        spr.removeEventListener(MouseEvent.MOUSE_MOVE,inScroll);
        spr.removeEventListener(MouseEvent.MOUSE_UP,stopScroll);
      }
    }
    var startScroll = function (e:MouseEvent) {
      bg.addEventListener(MouseEvent.MOUSE_MOVE,inScroll);
      bg.addEventListener(MouseEvent.MOUSE_UP,stopScroll);
      bg.addEventListener(MouseEvent.MOUSE_OUT,stopScroll);
      if (spr != null) {
        spr.addEventListener(MouseEvent.MOUSE_MOVE,inScroll);
        spr.addEventListener(MouseEvent.MOUSE_UP,stopScroll);
      }
      scrollX = e.stageX;
      scrollY = e.stageY;
    }
    bg.addEventListener(MouseEvent.MOUSE_DOWN,startScroll);

    var render = function () {
      if (spr != null)
        c.removeChild(spr);

      spr = new Sprite();
      c.addChild(spr);
      spr.addEventListener(MouseEvent.MOUSE_DOWN,startScroll);

      limit = app.limit.value;

      drawTrees(spr);

//      zoom = 1;
      spr.scaleX = spr.scaleY = zoom;

      var r = c.scrollRect;
      bg.x = r.x = 0;
      bg.y = r.y = -maxHeightAbove * gridHeight;
      c.scrollRect = r;
    }
/*
    app.renderButton.addEventListener
    (MouseEvent.CLICK,
     function (e:Event) {
       try {
         parseInput = app.parseInput.text;
         parseAbove = decode(app.parseAbove.text);
         parseBelow = decode(app.parseBelow.text);
         render();
       }
       catch (e:Dynamic) {
         drawText(c,font,0,0,"Error: uncaught exception");
       }
     });
*/
    app.minusButton.addEventListener
    (FlexEvent.BUTTON_DOWN,
     function (e:Event) {
       if (spr == null || zoom <= 0.6) return;
       zoom -= 0.1;
       spr.scaleX = spr.scaleY = zoom;
     });
    app.plusButton.addEventListener
    (FlexEvent.BUTTON_DOWN,
     function (e:Event) {
       if (spr == null || zoom >= 4) return;
       zoom += 0.1;
       spr.scaleX = spr.scaleY = zoom;
     });

    limit = app.limit.value;
    app.limit.addEventListener
    (NumericStepperEvent.CHANGE,
     function (e:Event) {
       limit = app.limit.value;
       render();
     });
    app.parseButton.addEventListener
    (MouseEvent.CLICK,
     function (e:Event) {
       try {
         parseInput = app.parseInput.text + "\r\n";
         var parses = Earley.parseToRender(parseInput);
         parseAbove = parses.left;
         parseBelow = parses.right;
         render();
       }
       catch (e:Dynamic) {
         drawText(c,font,0,0,"Uncaught exn " + e.toString());
       }
     });
  }
}


/*
foo login bar "baz"  
*/
/*
<t s="TOP" l="0" r="21"><t s="command" l="0" r="21"><t s="CRLF" l="19" r="21"><t s="LF" l="20" r="21"></t><t s="CR" l="19" r="20"></t></t><t s="command-nonauth" l="4" r="19"><t s="login" l="4" r="19"><t s="password" l="14" r="19"><t s="astring" l="14" r="19"><t s="string" l="14" r="19"><t s="quoted" l="14" r="19"><t s="DQUOTE" l="18" r="19"></t><t s="QUOTED-CHAR" l="17" r="18"></t><t s="QUOTED-CHAR" l="16" r="17"></t><t s="QUOTED-CHAR" l="15" r="16"></t><t s="DQUOTE" l="14" r="15"></t></t></t></t></t><t s="SP" l="13" r="14"></t><t s="userid" l="10" r="13"><t s="astring" l="10" r="13"><t s="ASTRING-CHAR" l="12" r="13"></t><t s="ASTRING-CHAR" l="11" r="12"></t><t s="ASTRING-CHAR" l="10" r="11"></t></t></t><t s="SP" l="9" r="10"></t></t></t><t s="SP" l="3" r="4"></t><t s="tag" l="0" r="3"></t></t></t>
*/
