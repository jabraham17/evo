module Environment {
  use Creature;
  use Random;
  use Image;

  config var envWidth: uint = 256;
  config var envHeight: uint = 256;
  config var imgScaleFactor: uint = 1;
  config var initialCreatures: uint = 100;
  config var brainSize: uint = 6;
  config var stepsPerGeneration: uint = 300;
  config var numGenerations: uint = 10;

  proc clamp(value: integral, minValue: integral, maxValue: integral) {
    if value < minValue then return minValue;
    if value > maxValue then return maxValue;
    return value;
  }
  proc clamp(value: ?n*integral, minValue: value.type, maxValue: value.type) {
    var res: value.type;
    for param i in 0..<n:int {
      res(i) = clamp(value(i), minValue(i), maxValue(i));
    }
    return res;
  }
    

  record environment {
    var envD: domain(2,int) = {0..<envWidth:int, 0..<envHeight:int};
    // a slightly large domain to with edges fuzzed to make slicing easy
    // TODO: paramterize to the same amount of edge as slice lookaround size
    var fuzzyEnvD: domain(2,int) = {envD.low[0]-1..envD.high[0]+1, envD.low[1]-1..envD.high[1]+1};
    var creatures: [fuzzyEnvD] owned creature?;

    proc init() {
      this.complete();
      var rng = createRandomStream(int);

      // we dont need locks on the fuzzy grid, just the real one
      var gridLocks: [envD] atomic bool = false;

      forall 1..initialCreatures {

        // regenerate positions until we find an empty slot
        var done = false;
        while !done {
          const pos = (rng.getNext(envD.low[0], envD.high[0]), rng.getNext(envD.low[1], envD.high[1]));
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

      forall pos in envD {
        ref c = creatures[pos];
        // if c == nil then continue;
        const scaledPos = pos * imgScaleFactor:int;
        var color: 3*uint(8);
        if c != nil then
          color = (255:uint(8),0:uint(8),0:uint(8));
        else
          color = (255:uint(8),255:uint(8),255:uint(8));
        for i in 0..<imgScaleFactor:int {
          for j in 0..<imgScaleFactor:int {
            img.set(scaledPos + (i,j), color);
          }
        }
      }
      return img;

    }

    proc run() {
      // TODO: populate
      for 1..numGenerations {
        runGeneration();
        // TODO: mutate children and repopulate
      }
    }

    proc runGeneration() {
      for 1..stepsPerGeneration {
        runStep();
      }
    }

    proc runStep() {
      // TODO: for now run serial, we will want to make this parallel to support bigger env
      // set to true once processed
      var processed: [envD] bool = false;
      for pos in envD {
        // skip if already processed or nil
        if processed[pos] || creatures[pos] == nil then continue;

        ref c = creatures[pos];
        const ss = new sliceState(this, pos);
        const act = c!.express(ss);
        var newPos = pos + act.getPosOffset();
        newPos = clamp(newPos, envD.low, envD.high);

        // if there is no creature at the pos, move there
        // otherwise failed move
        if creatures[newPos] == nil {
          // move semantics because owned
          creatures[newPos] = creatures[pos];
          processed[newPos] = true;
        }
        else {
          processed[pos] = true;
        }
        
      }
    }
  }

  record sliceState {
    // TODO: parametrize slicing/how far a creature can see
    var envSliceD: domain(2,int) = {-1..1, -1..1};
    var envSlice: [envSliceD] borrowed creature? = nil;
    var distanceToObstacleLeft: uint = 0;
    var distanceToObstacleRight: uint = 0;
    var distanceToObstacleUp: uint = 0;
    var distanceToObstacleDown: uint = 0;
    proc init(const ref env: environment, pos: 2*int) {
      const ref t = env.creatures[pos[0]-1..pos[0]+1, pos[1]-1..pos[1]+1];
      var tt = t.borrow();
      this.envSlice = tt.reindex(envSliceD);
      this.complete();
      this.setDistanceToObstacle(pos[0], envWidth-pos[0], pos[1], envWidth-pos[1]);
    }
    proc setDistanceToObstacle(left: uint, right: uint, up: uint, down: uint) {
      this.distanceToObstacleLeft = left;
      this.distanceToObstacleRight = right;
      this.distanceToObstacleUp = up;
      this.distanceToObstacleDown = down;
    }
  }

}
