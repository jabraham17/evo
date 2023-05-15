/* very simple wrapper around basic graphviz */
module Dot {
  use List;
  use Map;
  use IO.FormattedIO;
  
  type AttributeSet = map(string, string);

  class Vertex {
    var id: uint;
    var attributes: AttributeSet;


    proc init() {
      this.id = 0;
      this.attributes = new AttributeSet();
    }
    proc init(id: uint, attributes = new AttributeSet()) {
      this.id = id;
      this.attributes = attributes;
    }
    proc setAttribute(key: string, value: string): borrowed Vertex {
      this.attributes.addOrSet(key, value);
      return this.borrow();
    }

    operator ==(a: borrowed Vertex, b: borrowed Vertex) {
      return a.id == b.id ;
    }
  }

  class Edge {
    var v1: borrowed Vertex;
    var v2: borrowed Vertex;
    var attributes: AttributeSet;


    proc init(v1: borrowed Vertex, v2: borrowed Vertex, attributes = new AttributeSet()) {
      this.v1 = v1;
      this.v2 = v2;
      this.attributes = attributes;
    }
    proc setAttribute(key: string, value: string): borrowed Edge {
      this.attributes.addOrSet(key, value);
      return this.borrow();
    }

    operator ==(a: borrowed Edge, b: borrowed Edge) {
      return a.v1 == b.v1 && a.v2 == b.v2;
    }
  }

  class Graph {
    var name: string;
    var isDirected: bool;
    var vertexes: list(owned Vertex?);
    var edges: list(owned Edge?);
    var attributes: AttributeSet;

    var parent: borrowed Graph?;
    var children: list(owned Graph?);

    proc init(name: string = "", isDirected: bool = false) {
      this.name = name;
      this.isDirected = isDirected;
      this.vertexes = new list(owned Vertex?);
      this.edges = new list(owned Edge?);
      this.attributes = new AttributeSet();
      this.parent = nil;
      this.children = new list(owned Graph?);
    }

    proc setAttribute(key: string, value: string): borrowed Graph {
      this.attributes.addOrSet(key, value);
      return this.borrow();
    }
    proc addChild(in child: owned Graph) {
      child.parent = this.borrow():class?;
      this.children.append(child);
    }

    proc addOrGetVertex(id: uint, attrs = new AttributeSet()): borrowed Vertex {
      var found = this.getVertex(id);
      
      if found then return found!;
      else {
        var v = new Vertex(id, attrs):class?;
        this.vertexes.append(v);
        return this.vertexes.last().borrow()!;
      }
    }
    proc getVertex(id: uint): borrowed Vertex? {
      var tmpVertex = new Vertex(id):class?;
      var foundIdx = this.vertexes.find(tmpVertex);

      if foundIdx == -1 then return nil;
      else return this.vertexes[foundIdx];
    }

    proc addOrGetEdge(v1: borrowed Vertex, v2: borrowed Vertex, attrs = new AttributeSet()): borrowed Edge {
      var found = this.getEdge(v1, v2);
      
      if found then return found!;
      else {
        var e = new Edge(v1, v2, attrs):class?;
        this.edges.append(e);
        return this.edges.last().borrow()!;
      }
    }
    proc getEdge(v1: borrowed Vertex, v2: borrowed Vertex): borrowed Edge? {
      var tmpEdge = new Edge(v1, v2):class?;
      var foundIdx = this.edges.find(tmpEdge);

      if foundIdx == -1 then return nil;
      else return this.edges[foundIdx];
    }

    proc isSubGraph {
      return parent != nil;
    }

    operator :(a: this.type, type t: string) {
      return a.toString();
    }
    proc toString() {
    var indent: int = 0;
    return toString(indent);
    }
    proc toString(ref indent: int): string {
      var buff = "";
      proc attrsToString(attrs: AttributeSet, prefix="[", postfix="]") {
        var buff = "";
        for key in attrs.keys() {
          buff += prefix + key + "=" + attrs[key] + postfix;
        }
        return buff;
      }

      if isDirected && !isSubGraph then buff += "di";
      if isSubGraph then buff += "sub";

      var noSpaceName = "_".join(name.split(" "));
      var niceName: string;
      var refName = noSpaceName;
      if(isSubGraph) {
        niceName = "cluster_" + noSpaceName;
        refName = niceName;
      }
      else {
        niceName = "\"%s\"".format(name);
      }
      buff += "graph " + niceName + "{\n";

      indent += 2;

      buff += attrsToString(this.attributes, " "*indent, ";\n");

      for v in this.vertexes {
        buff += " "*indent;
        buff += refName + "_" + v!.id:string;
        buff += attrsToString(v!.attributes);
        buff += ";\n";
      }

      var arrow = if isDirected then "->" else "--";
      for e in this.edges {
        buff += " "*indent;
        buff += refName + "_" + e!.v1.id:string + arrow + refName + "_" + e!.v2.id:string;
        buff += attrsToString(e!.attributes);
        buff += ";\n";
      }

      for c in this.children {
        buff += c!.toString(indent);
      }

      indent -= 2;
      buff += " "*indent + "}\n";
      return buff;
    }

  }


}
