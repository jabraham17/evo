module Environment {
  use Creature;
  use Random;
  use Image;
  use Queue;
  use Random;

  config var envWidth: uint = 256;
  config var envHeight: uint = 256;
  config var imgScaleFactor: uint = 1;
  config var initialCreatures: uint = 100;
  var maxCreatures: uint = initialCreatures; // not a config for now
  config var brainSize: uint = 6;
  config var stepsPerGeneration: uint = 300;
  config var numGenerations: uint = 10;
  config var selectionCriteria: criteriaType = criteriaType.left;

  enum criteriaType {
    left,right,up,down
  }
  class criteria {
    proc this(env: environment, c: borrowed creature): bool do return false;
  }
  class criteriaLeft: criteria {
    override proc this(env: environment, c: borrowed creature): bool {
      const left = env.envD[..env.envD.high[0]/2, ..];
      return left.contains(c.pos());
    }
  }
  var criteriaMapD: domain(criteriaType);
  var criteriaMap: [criteriaMapD] owned criteria?;

  criteriaMapD += criteriaType.left;
  criteriaMap[criteriaType.left] = new criteriaLeft();


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
    var grid: [fuzzyEnvD] borrowed creature?;
    var creatures: [0..<maxCreatures] owned creature?;


    proc init() {
      this.complete();
      this.populate(initialCreatures, 0);
    }

    proc selectCreatures() {
      foreach (c, idx) in zip(creatures, creatures.domain) {
        if c != nil && ! criteriaMap[selectionCriteria]!(this, c!) {
          // remove grid borrow before killing
          grid[c!.pos()] = nil;
          c = nil;
        }
      }
    }

    /*move all alive creatures to the front
    return the next open index, or size of creatures*/
    proc shuffleCreatures(): uint {
      var openSlots = new queue(uint);
      for idx in creatures.domain {
        ref c = creatures[idx];
        if c == nil then openSlots.push(idx);
        else {
          // no openSlots, dont need to move
          if !openSlots.isEmpty() {
            var openIdx = openSlots.pop();
            creatures[openIdx] <=> c;
            openSlots.push(idx);
          }
        }
      }
      if openSlots.isEmpty()
        then return creatures.size;
        else return openSlots.pop();
    }

    proc populate(nCreatures: integral,
                  startIdx: integral = 0,
                  repopulate: bool = false) {
      var rng = createRandomStream(int);

      for idx in startIdx..<(startIdx+nCreatures) {
        var newCreature: owned creature?;
        if !repopulate
          then newCreature = new creature(brainSize);
          else {
            // get a random alive creature
            var randIdx = rng.getNext(creatures.domain.low:int, startIdx:int-1);
            newCreature = new creature(creatures[randIdx]!.borrow());
            newCreature!.mutate();
          }
        // regenerate positions until we find an empty slot
        var done = false;
        while !done {
          const pos = (rng.getNext(envD.low[0], envD.high[0]), rng.getNext(envD.low[1], envD.high[1]));
          if grid[pos] == nil {
            newCreature!.setPos(pos);
            creatures[idx] = newCreature;
            grid[pos] = creatures[idx]!.borrow();
            // creature set, we are done
            done = true;
          }
        }
      }
    }


    proc toImg() {
      var img = new image(envWidth * imgScaleFactor, envHeight * imgScaleFactor);

      foreach c in creatures {
        if c == nil then continue;
        const pos = c!.pos();
        const scaledPos = pos * imgScaleFactor:int;
        const color = c!.uniqueColor();
        for i in 0..<imgScaleFactor:int {
          for j in 0..<imgScaleFactor:int {
            img.set(scaledPos + (i,j), color);
          }
        }
      }
      return img;

    }

    proc run() {
      for 1..numGenerations {
        runGeneration();
        selectCreatures();
        var nextOpen = shuffleCreatures();
        var repopulateNum = creatures.domain.high - nextOpen + 1;
        populate(repopulateNum, startIdx=nextOpen, repopulate=true);
      }
    }

    proc runGeneration() {
      for 1..stepsPerGeneration {
        runStep();
      }
    }

    proc findTheBadOne() {
      writeln("finding bad one");
      for pos in envD {
        const ref c = grid[pos];
        if c == nil then continue;
        writeln(pos, " ", c!.brainSize);
      }
    }

    proc runStep() {
      // TODO: for now run serial, we will want to make this parallel to support bigger env
      // findTheBadOne();
      var newGrid: [grid.domain] borrowed creature?;
      for pos in envD {
        const ref c = grid[pos];
        // skip if nil
        if c == nil then continue;

        const ss = new sliceState(this, pos);
        const act = c!.express(ss);
        var newPos = pos + act.getPosOffset();
        newPos = clamp(newPos, envD.low, envD.high);

        // if there is no creature at the pos, move there
        // otherwise failed move
        if newGrid[newPos] == nil {
          newGrid[newPos] <=> grid[pos];
        }

      }
      this.grid <=> newGrid;
    }
  }

  record sliceState {
    // TODO: parametrize slicing/how far a creature can see
    var envSliceD: domain(2,int) = {-1..1, -1..1};
    var envSlice: [envSliceD] borrowed creature?;
    var distanceToObstacleLeft: uint = 0;
    var distanceToObstacleRight: uint = 0;
    var distanceToObstacleUp: uint = 0;
    var distanceToObstacleDown: uint = 0;
    proc init(const ref env: environment, pos: 2*int) {
      const ref tt = env.grid[pos[0]-1..pos[0]+1, pos[1]-1..pos[1]+1];
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
