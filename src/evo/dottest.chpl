use Dot;
use IO;

config var filename = "out.dot";

proc main() {

  var g = new Graph();

  var v1 = g.addOrGetVertex(1);
  v1.setAttribute("label", "v1");
  var v2 = g.addOrGetVertex(2);
  v2.setAttribute("label", "v2")
    .setAttribute("fillcolor", "red");
  v2.setAttribute("style", "filled");
  g.addOrGetEdge(v1, v2).setAttribute("color", "green");
  var e = g.getEdge(v1, v2);
  if e then
    e!.setAttribute("label", "edge");
  


  var w = openWriter(filename);
  w.write(g.toString());

}
