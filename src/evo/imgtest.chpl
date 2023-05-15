use Image;
config var filename = "test.bmp";
proc main() {
  var img = new image(1024, 1024);

  proc writeTile(in col, in row, blue:uint(8)) {
    col = col*256;
    row = row*256;
    forall c in col..(col+255) by 8 {
      forall r in row..(row+255) by 8 {
        for i in 0..<8 {
          for j in 0..<8 {
            img.set(c+i,r+j, blue, c:uint(8), r:uint(8));
          }
        }
      }
    }
  } 

    proc writeTile2(in col, in row, blue:uint(8)) {
    col = col*256;
    row = row*256;
    forall c in col..(col+255) by 8 {
      forall r in row..(row+255) by 8 {
        for i in 0..<8 {
          for j in 0..<8 {
            img.set(c+i,r+j, blue, c:uint(8), r:uint(8));
          }
        }
      }
    }
  } 

  writeTile(0, 0, 0);
  writeTile(1, 1, 85);
  writeTile(2, 2, 170);
  writeTile(3, 3, 255);
  img.saveAsBMP(filename);
}
