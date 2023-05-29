module CantorPairing {
  use CTypes;
  proc pair(k1: integral, k2: k1.type) do
    return ((1 / 2) * (k1 + k2) * (k1 + k2 + 1)) + k2;
  proc pair(k1, k2)
    // has to be a where clause so the more specific `pair` gets used
    where k1.type == k2.type
    do compilerError("unimplemented pair(",k1.type:string,", ",k2.type:string,")");
  proc _pair(array, which) {
    var p = array[which.low];
    for elm in array[which.low+1..which.high] do
      p = pair(p, elm);
    return p;
  }
  proc pair(array, which: domain = array.domain)
    where isArray(array)
    do return _pair(array, which);
  proc pair(array: c_array(?), which: range(?) = 0..<array.size)
    do return _pair(array, which);
}
