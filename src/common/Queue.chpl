module Queue {

class queueElm {
  type valueType;
  var val: valueType;

  var prev: unmanaged queueElm(valueType)?;
  var next: unmanaged queueElm(valueType)?;

  proc init(type valueType, in val: valueType) {
    this.valueType = valueType;
    this.val = val;
    this.prev = nil;
    this.next = nil;
  }

}

class queue {
  type valueType;
  type elmType = queueElm(valueType);
  var front : unmanaged elmType?;
  var back : unmanaged elmType?;

  proc init(type valueType) {
    this.valueType = valueType;
    this.front = nil;
    this.back = nil;
  }

  proc deinit() {
    while !isEmpty() {
      pop();
    }
  }

  proc push(in val: valueType) {
    var elm = new unmanaged elmType(val);
    if front == nil {
      front = elm;
      back = elm;
    }
    else {
      back!.next = elm;
      elm.prev = back;
      back = elm;
    }
  }

  proc isEmpty(): bool {
    return front == nil;
  }

  proc pop(): valueType {
    var elm: unmanaged elmType?;
    if front == back {
      elm = front;
      front = nil;
      back = nil;
    }
    else {
      elm = front;
      front = front!.next;
      front!.prev = nil;
      elm!.next = nil;
    }
    var val = elm!.val;
    delete elm;
    return val;
  }

}

}
