module NamedTuple {

  record namedTuple {
    type _tupleType;
    var _elms: _tupleType;
    var _names: _tupleType.size*string;
    proc init(elms: _tuple, names: elms.size*string) {
      this._tupleType = elms.type;
      this._elms = elms;
      this._names = names;
    }
    proc init(elms: _tuple) {
      this._tupleType = elms.type;
      this._elms = elms;
    }
    proc init(type elmsType, names: _tuple) {
      this._tupleType = elmsType;
      this._names = names;
      this.complete();

      assert(this._elms.size == this._names.size);
      for param i in 0..<names.size {
        assert(names(i).type == string);
      }
    }

    proc name() do return this._names;
    proc name(idx: integral) do return this._names[idx];

    proc size param do return _elms.size;

    proc init=(other: namedTuple) {
      this._tupleType = other._tupleType;
      this._elms = other._elms;
      this._names = other._names;
    }
    proc init=(other: _tuple) {
      this._tupleType = other.type;
      this._elms = other;
    }
    operator :(x: _tuple, type t: namedTuple) {
      return new namedTuple(x);
    }

    operator =(ref x: namedTuple, y: _tuple)
      where x.size == y.size {
      for param i in 0..<x.size do
        x(i) = y(i);
    }
    operator =(ref x: namedTuple, y: namedTuple)
      where x.size == y.size {
      for param i in 0..<x.size {
        assert(x.name(i) == y.name(i));
        x(i) = y(i);
      }
    }

    proc this(idx: integral) ref {
      return _elms(idx);
    }
    proc this(name: string) ref {
      if name.size == 0 then halt("cannot access empty name");
      for param i in 0..<_names.size {
        if _names(i) == name then return _elms(idx);
      }
      halt("unknown name");
    }
    iter these() {
      for elm in zip(_names, _elms) {
        yield elm;
      }
    }

    proc writeThis(w) throws {
      w.write("(");
      var sep: string;
      for (n, e) in this {
        if n.size != 0
          then w.writef("%s[%s=]%t", sep, n, e);
          else w.writef("%s%t", sep, e);
        sep = ", ";
      }
      w.write(")");
    }
  }
  // proc main() {
  //   var x = (1,2);
  //   var m = new namedTuple(x, ("x", "y"));
  //   writeln(m);
  //   writeln(x);
  //   writeln(m["x"]);
  //   m[1] = 7;
  //   writeln(m);
  //   m["y"] = 3;
  //   writeln(m[1]);

  //   m = x;
  //   writeln(m);

  //   var m2 = new namedTuple((10, 9));
  //   writeln(m2);

  //   var m3 = m;
  //   m3("y") = 10;
  //   writeln(m3);
  //   m3 = (8, 3);
  //   writeln(m3);
  // }
}
