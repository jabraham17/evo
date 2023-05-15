module Creature {
  use CTypes;
  use Random;
  use MyBitOps;
  import Dot;
  use IO.FormattedIO;

  config param genomeMaxConnections: uint = 16;

  class creature {
    var dna: genome;
    proc init(numConnections: uint) {
      this.dna = new genome(numConnections);
    }
  }


  proc scale(value: ?valueType, resultMin: ?resultType, resultMax: resultType, oldMin = min(valueType), oldMax = max(valueType)): resultType {
    var result: resultType = (resultMax - resultMin) * 
                              (value:resultType - oldMin:resultType) /
                              (oldMax:resultType - oldMin:resultType) +
                              resultMin;
    return result;
  }

  enum geneField {
    source,
    sink,
    weight
  }
  record geneConnection {
    var gene: uint(32);
    proc init() {
      this.gene = 0;
    }
    proc init(gene: uint(32)) {
      this.gene = gene;
    }
    proc init(sourceSink: uint(16), weight: uint(16)) {
      this.gene = (sourceSink << 16) | weight;
    }
    proc init(source: uint(8), sink: uint(8), weight: uint(16)) {
      this.gene = (source << 24) | (sink << 16) | weight;
    }
    proc set(which: geneField, val) {
      use geneField;
      select(which) {
        when source do this.gene = setBitRange(31, 24, this.gene, val):this.gene.type;
        when sink do this.gene = setBitRange(23, 16, this.gene, val):this.gene.type;
        when weight do this.gene = setBitRange(15, 0, this.gene, val):this.gene.type;
      }
    }
    proc this(which: geneField) do return this.get(which);
    proc get(which: geneField) {
      use geneField;
      select(which) {
        when source do return getBitRange(31, 24, this.gene);
        when sink do return getBitRange(23, 16, this.gene);
        when weight do return getBitRange(15, 0, this.gene);
      }
      halt("Should not get here");
    }

    proc isInternal(param which: geneField) 
      where which == geneField.source || which == geneField.sink {
      var v = this(which);
      return v >= min(geneTypeInternal):int && v <= max(geneTypeInternal):int;
    }

    proc toString() {
      use geneField;
      var s: string;
      s += "Source: %20s".format(geneToString(this(source), geneTypeSource));
      s += " Sink: %20s".format(geneToString(this(sink), geneTypeSink));
      s += " Weight: %n".format(this(weight):int(16));
      return s;
    }
    operator :(a: this.type, type t: string) {
      return a.toString();
    }

    proc toDot(d: borrowed Dot.Graph) {
      use geneField;
      var sourceVertexID = this(source) | 
                            (if !this.isInternal(source) then 1 << 9 else 0);
      var sinkVertexID =   this(sink)   |
                            (if !this.isInternal(sink)   then 1 << 10 else 0);

      var sourceVectex = d.addOrGetVertex(sourceVertexID);
      var sinkVectex = d.addOrGetVertex(sinkVertexID);
      var geneEdge = d.addOrGetEdge(sourceVectex, sinkVectex);

      sourceVectex.setAttribute("label", geneToString(this(source), geneTypeSource));
      sinkVectex.setAttribute("label", geneToString(this(sink), geneTypeSink));

      var weight_i = this(weight):int(16);
      var weight_f = scale(weight_i, -4.0, 4.0);
      geneEdge.setAttribute("xlabel", "%4.3r".format(weight_f));

      var h = if weight_i >= 0 then 0.333 else 0.0;
      var s = scale(abs(weight_i), 0.5, 1.0, 0, max(int(16)));
      var v = 1.0;
      geneEdge.setAttribute("color", '"%4.3r,%4.3r,%4.3r"'.format(h, s, v));

      var penwidth = scale(abs(weight_i), 1.0, 8.0, 0, max(int(16)));
      geneEdge.setAttribute("penwdith", "%4.3r".format(penwidth));
    }
  }
  enum geneTypeInternal {
    INTERNAL_N0=0, INTERNAL_N1, INTERNAL_N2
  }
  proc min(type t) param
    where t == geneTypeInternal do return geneTypeInternal.INTERNAL_N0;
  proc max(type t) param
    where t == geneTypeInternal do return geneTypeInternal.INTERNAL_N2;
  enum geneTypeSource {
    SENSE_WALL_LEFT=max(geneTypeInternal):int+1,
    SENSE_WALL_RIGHT,
    SENSE_WALL_UP,
    SENSE_WALL_DOWN,
    SENSE_CREATURE_LEFT,
    SENSE_CREATURE_RIGHT,
    SENSE_CREATURE_UP,
    SENSE_CREATURE_DOWN,
  }
  proc min(type t) param
    where t == geneTypeSource do return geneTypeSource.SENSE_WALL_LEFT;
  proc max(type t) param
    where t == geneTypeSource do return geneTypeSource.SENSE_CREATURE_DOWN;
  enum geneTypeSink {
    ACTION_MOVE_LEFT=max(geneTypeInternal):int+1,
    ACTION_MOVE_RIGHT,
    ACTION_MOVE_UP,
    ACTION_MOVE_DOWN,
  }
  proc min(type t) param
    where t == geneTypeSink do return geneTypeSink.ACTION_MOVE_LEFT;
  proc max(type t) param
    where t == geneTypeSink do return geneTypeSink.ACTION_MOVE_DOWN;

  proc geneToString(v: integral, type t)
    where t == geneTypeSource || t == geneTypeSink {
    if v >= min(t):int && v <= max(t):int then
      return v:t:string;
    if v >= min(geneTypeInternal):int && v <= max(geneTypeInternal):int then
      return v:geneTypeInternal:string;
    return "UNKNOWN";
  }

  record genome {
    var numConnections: uint;
    var connections: c_array(geneConnection, genomeMaxConnections);
    proc init(numConnections: uint) {
      this.numConnections = numConnections;
      this.connections = new c_array(geneConnection, genomeMaxConnections);
      this.complete();
      var rng = createRandomStream(uint(32));
      for idx in 0..<this.numConnections {
        this.connections[idx] = new geneConnection(rng.getNext());
      }
      this.normalize();
      this.prune();
    }

    proc normalize() {
      use geneField;
      for idx in 0..<numConnections {
        var src = connections[idx](source);
        src = src % max(geneTypeSource):int;
        connections[idx].set(source, src);

        var snk = connections[idx](sink);
        snk = snk % max(geneTypeSink):int;
        connections[idx].set(sink, snk);
      }
    }
    proc prune() {
      use geneField;
      /*
      for each connection
      if source is internal and never sunk to, remove
      if sink is internal and never sourced, remove
      */
      var changed = true;
      while changed {
        changed = false;
        var i = 0;
        while i<numConnections {
          var c = connections[i];
          // A: if source and sink are same, points to itself, remove
          // B: if source is internal and never sunk to, remove
          // C: if sink is internal and never sourced, remove
          var A = c.isInternal(source) &&
                  c.isInternal(sink) &&
                  c(source):int(8) == c(sink):int(8);
          var B = c.isInternal(source) &&
                  !geneIsUsed(sink, c(source));
          var C = c.isInternal(sink) &&
                  !geneIsUsed(source, c(sink));
          // writeln((c:string, c.isInternal(source), c.isInternal(sink)));
          // var  B, C = false;
          if A || B || C {
            changed = true;
            writeln("Pruning because ", (A, B, C), " ", connections[i]:string);
            // overwrite this connection with the last connection
            connections[i] = connections[numConnections - 1];
            numConnections-=1;
            i-=1;
          }
          i+=1;
        }
      }
    }

    proc geneIsUsed(param which: geneField, gene: ?t)
      where which == geneField.source || which == geneField.sink {

      for i in 0..<numConnections {
        if connections[i](which):t == gene then return true;
      }
      return false;
    }

    proc prettyPrint() {
      use geneField;
      for idx in 0..<numConnections {
        var c = connections[idx];
        writeln(c:string);
      }
    }

    proc toDot() {
      var d = new Dot.Graph("creature brain", true);
      d.setAttribute("splines", "curved");
      for idx in 0..<numConnections {
        connections[idx].toDot(d);
      }
      return d;
    }
  }

}
