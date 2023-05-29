module MyBitOps {

type BitType = uint;
param NBITS = Types.numBits(BitType);

proc getMask(param N: BitType) param : BitType {
  assert(N <= NBITS);
  return Types.max(BitType) >> (NBITS - N);
}

proc getBitsFromLSB(param   LSB: BitType, 
                    param COUNT: BitType, 
                           bits: BitType): BitType {
  assert(LSB < NBITS);
  return (bits >> LSB) & getMask(COUNT);
}
proc clearBitsFromLSB(param LSB:   BitType,
                      param COUNT: BitType, 
                            bits:  BitType): BitType {
  assert(LSB < NBITS);
  return bits & ~(getMask(COUNT) << LSB);
}
proc setBitsFromLSB(param LSB:   BitType,
                    param COUNT: BitType,
                          bits:  BitType,
                          toSet: BitType): BitType {
  assert(LSB < NBITS);
  var toSetMask = toSet & getMask(COUNT);
  var bitsClear = clearBitsFromLSB(LSB, COUNT, bits);
  var bitsSet = bitsClear | (toSetMask << LSB);
  return bitsSet;
}

proc getBit(param N: BitType, 
                  bits: BitType): BitType {
  assert(N < NBITS);
  return getBitsFromLSB(N, 1, bits);
}
proc clearBit(param N: BitType,
                    bits: BitType): BitType {
  assert(N < NBITS);
  return clearBitsFromLSB(N, 1, bits);
}
proc setBit(param N: BitType, 
                  bits: BitType,
                  toSet: BitType): BitType {
  assert(N < NBITS);
  return setBitsFromLSB(N, 1, bits, toSet);
}


proc getBitsFromMSB(param MSB:   BitType,
                    param COUNT: BitType,
                          bits:  BitType): BitType {
  assert(MSB < NBITS);
  return getBitsFromLSB(MSB - COUNT + 1, COUNT, bits);
}

proc clearBitsFromMSB(param MSB:   BitType,
                      param COUNT: BitType,
                            bits:  BitType): BitType {
  assert(MSB < NBITS);
  return clearBitsFromLSB(MSB - COUNT + 1, COUNT, bits);
}

proc setBitsFromMSB(param MSB:   BitType,
                    param COUNT: BitType,
                          bits:  BitType,
                          toSet: BitType): BitType {
  assert(MSB < NBITS);
  return setBitsFromLSB(MSB - COUNT + 1, COUNT, bits, toSet);
}

// inclusive, MSB to LSB
proc getBitRange(param MSB:  BitType,
                 param LSB:  BitType,
                       bits: BitType): BitType {
  assert(MSB < NBITS && MSB >= LSB);
  return getBitsFromLSB(LSB, MSB - LSB + 1, bits);
}
proc clearBitRange(param MSB:  BitType,
                   param LSB:  BitType,
                         bits: BitType): BitType {
  assert(MSB < NBITS && MSB >= LSB);
  return clearBitsFromLSB(LSB, MSB - LSB + 1, bits);
}
proc setBitRange(param MSB:   BitType,
                 param LSB:   BitType,
                       bits:  BitType,
                       toSet: BitType): BitType {
  assert(MSB < NBITS && MSB >= LSB);
  return setBitsFromLSB(LSB, MSB - LSB + 1, bits, toSet);
}

}
