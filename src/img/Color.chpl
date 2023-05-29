module Color {


  proc hsvToBgr(hsv: (uint(16), real, real)): 3*uint(8) {
    assert(hsv(0) < 360 &&
           hsv(1) <= 1.0 &&
           hsv(1) >= 0 &&
           hsv(2) <= 1.0 &&
           hsv(2) >= 0);
    var bgr: 3*uint(8);
    var H = hsv(0);
    var S = (hsv(1) * 255):uint(16);
    var V = (hsv(2) * 255):uint(16);

    var C = V * S;
    var Hprime = H / 60;
    var X = C * (1 - abs(Hprime % 2 - 1));
    select Hprime {
      when 0 do bgr = (0, X, C):bgr.type;
      when 1 do bgr = (0, C, X):bgr.type;
      when 2 do bgr = (X, C, 0):bgr.type;
      when 3 do bgr = (C, X, 0):bgr.type;
      when 4 do bgr = (C, 0, X):bgr.type;
      when 5 do bgr = (X, 0, C):bgr.type;
    }
    var m = (V - C):uint(8);
    bgr += (m,m,m);
    return bgr;
  }

}
