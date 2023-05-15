module Main {
  use Creature;
  use IO;

  proc main() {
    var c = new genome(10, 32);
    c.prettyPrint();
    writeln("="*80);

    {
      var dot = c.toDot();
      var w = openWriter("brain.dot");
      w.write(dot:string);
    }

    c.prune();
    {
      c.prettyPrint();
      var dot = c.toDot();
      var w = openWriter("pruned.dot");
      w.write(dot:string);
    }
  }

}
