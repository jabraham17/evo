module Environment {
  use Creature;
  use Random;
  use Image;

  config var envWidth: uint = 256;
  config var envHeight: uint = 256;
  config var imgScaleFactor: uint = 1;
  config var initialCreatures: uint = 100;
  config var brainSize: uint = 6;

  record environment {
    var envD: domain(2,uint) = {0..<envWidth, 0..<envHeight};
    var creatures: [envD] owned creature?;
    var gridLocks: [envD] atomic bool = false;

    proc init() {
      this.complete();
      var rng = createRandomStream(uint);
      forall 1..initialCreatures {

        // regenerate positions until we find an empty slot
        var done = false;
        while !done {
          var pos = (rng.getNext(0, envWidth-1), rng.getNext(0, envHeight-1));
          ref lock = gridLocks[pos];
          // store true in lock iff value was false
          var expected = false;
          if lock.compareExchange(expected, true) {
            // we have aquired the lock
            if creatures[pos] == nil {
              creatures[pos] = new creature(brainSize);
              // creature set, we are done
              done = true;
            }
            // release lock
            lock.write(false);
          }
        }
      }
    }

    proc toImg() {
      var img = new image(envWidth * imgScaleFactor, envHeight * imgScaleFactor);

      forall (idx, c) in zip(envD, creatures) {
        // if c == nil then continue;
        var pos = idx * imgScaleFactor;
        var color: 3*uint(8);
        if c != nil then
          color = (255:uint(8),0:uint(8),0:uint(8));
        else
          color = (255:uint(8),255:uint(8),255:uint(8));
        for i in 0..<imgScaleFactor {
          for j in 0..<imgScaleFactor {
            img.set(pos + (i,j), color);
          }
        }
      }
      return img;

    }

  }


}
