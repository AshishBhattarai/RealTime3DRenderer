#include "serializer.h"
#include "third_party/catch.hpp"

Serializer &serializer = Serializer::getInstance();

TEST_CASE("Single pack & unpack test for signed integers.") {
  s8 toPakA = 125;
  s8 toPakB = -125;
  s16 toPakC = 32767;
  s16 toPakD = -32767;
  s32 toPakH = 2147483647;
  s32 toPakI = -2147483647;
  s64 toPakE = 9223372036854775807;
  s64 toPakF = -9223372036854775807;
  char toPakG = 125;
  char toPakJ = -125;

  s8 unPakA;
  s8 unPakB;
  s16 unPakC;
  s16 unPakD;
  s32 unPakH;
  s32 unPakI;
  s64 unPakE;
  s64 unPakF;
  char unPakG;
  char unPakJ;

  Buffer bufA(1);
  Buffer bufB(1);
  Buffer bufC(2);
  Buffer bufD(2);
  Buffer bufH(4);
  Buffer bufI(4);
  Buffer bufE(8);
  Buffer bufF(8);
  Buffer bufG(1);
  Buffer bufJ(1);

  serializer.pack(bufA, 0, toPakA);
  serializer.pack(bufB, 0, toPakB);
  serializer.pack(bufC, 0, toPakC);
  serializer.pack(bufD, 0, toPakD);
  serializer.pack(bufH, 0, toPakH);
  serializer.pack(bufI, 0, toPakI);
  serializer.pack(bufE, 0, toPakE);
  serializer.pack(bufF, 0, toPakF);
  serializer.pack(bufG, 0, toPakG);
  serializer.pack(bufJ, 0, toPakJ);

  serializer.unPack(bufA, 0, unPakA);
  serializer.unPack(bufB, 0, unPakB);
  serializer.unPack(bufC, 0, unPakC);
  serializer.unPack(bufD, 0, unPakD);
  serializer.unPack(bufH, 0, unPakH);
  serializer.unPack(bufI, 0, unPakI);
  serializer.unPack(bufE, 0, unPakE);
  serializer.unPack(bufF, 0, unPakF);
  serializer.unPack(bufG, 0, unPakG);
  serializer.unPack(bufJ, 0, unPakJ);

  REQUIRE(toPakA == unPakA);
  REQUIRE(toPakB == unPakB);
  REQUIRE(toPakC == unPakC);
  REQUIRE(toPakD == unPakD);
  REQUIRE(toPakH == unPakH);
  REQUIRE(toPakI == unPakI);
  REQUIRE(toPakE == unPakE);
  REQUIRE(toPakF == unPakF);
  REQUIRE(toPakG == unPakG);
  REQUIRE(toPakJ == unPakJ);
}

TEST_CASE("Single pack & unpack test for unsigned integers.") {
  u8 toPakA = 0;
  u8 toPakB = 255;
  u16 toPakC = 0;
  u16 toPakD = 65535;
  u32 toPakH = 0;
  u32 toPakI = 4294967295;
  u64 toPakE = 0;
  u64 toPakF = 1.844674407370955e19;
  uchar toPakG = 255;
  uchar toPakJ = 0;

  u8 unPakA;
  u8 unPakB;
  u16 unPakC;
  u16 unPakD;
  u32 unPakH;
  u32 unPakI;
  u64 unPakE;
  u64 unPakF;
  uchar unPakG;
  uchar unPakJ;

  Buffer bufA(1);
  Buffer bufB(1);
  Buffer bufC(2);
  Buffer bufD(2);
  Buffer bufH(4);
  Buffer bufI(4);
  Buffer bufE(8);
  Buffer bufF(8);
  Buffer bufG(1);
  Buffer bufJ(1);

  serializer.pack(bufA, 0, toPakA);
  serializer.pack(bufB, 0, toPakB);
  serializer.pack(bufC, 0, toPakC);
  serializer.pack(bufD, 0, toPakD);
  serializer.pack(bufH, 0, toPakH);
  serializer.pack(bufI, 0, toPakI);
  serializer.pack(bufE, 0, toPakE);
  serializer.pack(bufF, 0, toPakF);
  serializer.pack(bufG, 0, toPakG);
  serializer.pack(bufJ, 0, toPakJ);

  serializer.unPack(bufA, 0, unPakA);
  serializer.unPack(bufB, 0, unPakB);
  serializer.unPack(bufC, 0, unPakC);
  serializer.unPack(bufD, 0, unPakD);
  serializer.unPack(bufH, 0, unPakH);
  serializer.unPack(bufI, 0, unPakI);
  serializer.unPack(bufE, 0, unPakE);
  serializer.unPack(bufF, 0, unPakF);
  serializer.unPack(bufG, 0, unPakG);
  serializer.unPack(bufJ, 0, unPakJ);

  REQUIRE(toPakA == unPakA);
  REQUIRE(toPakB == unPakB);
  REQUIRE(toPakC == unPakC);
  REQUIRE(toPakD == unPakD);
  REQUIRE(toPakH == unPakH);
  REQUIRE(toPakI == unPakI);
  REQUIRE(toPakE == unPakE);
  REQUIRE(toPakF == unPakF);
  REQUIRE(toPakG == unPakG);
  REQUIRE(toPakJ == unPakJ);
}

TEST_CASE("Single pack & unpack test for floating point types.") {
  float toPakA = std::numeric_limits<float>::max();
  float toPakB = std::numeric_limits<float>::min();
  double toPakC = std::numeric_limits<double>::min();
  double toPakD = std::numeric_limits<double>::max();

  float unPakA;
  float unPakB;
  double unPakC;
  double unPakD;

  Buffer bufA(32);
  Buffer bufB(32);
  Buffer bufC(64);
  Buffer bufD(64);

  serializer.pack(bufA, 0, toPakA);
  serializer.pack(bufB, 0, toPakB);
  serializer.pack(bufC, 0, toPakC);
  serializer.pack(bufD, 0, toPakD);

  serializer.unPack(bufA, 0, unPakA);
  serializer.unPack(bufB, 0, unPakB);
  serializer.unPack(bufC, 0, unPakC);
  serializer.unPack(bufD, 0, unPakD);

  REQUIRE(toPakA == unPakA);
  REQUIRE(toPakB == unPakB);
  REQUIRE(toPakC == unPakC);
  REQUIRE(toPakD == unPakD);
}

TEST_CASE("Multiple signed integers pack & unpack test.") {
  s8 toPakA = 125;
  s8 toPakB = -125;
  s16 toPakC = 32767;
  s16 toPakD = -32767;
  s32 toPakH = 2147483647;
  s32 toPakI = -2147483647;
  s64 toPakE = 9223372036854775807;
  s64 toPakF = -9223372036854775807;
  char toPakG = 125;
  char toPakJ = -125;

  s8 unPakA;
  s8 unPakB;
  s16 unPakC;
  s16 unPakD;
  s32 unPakH;
  s32 unPakI;
  s64 unPakE;
  s64 unPakF;
  char unPakG;
  char unPakJ;

  Buffer buf(32);

  serializer.pack(buf, 0, toPakA, toPakB, toPakC, toPakD, toPakE, toPakF,
                  toPakG, toPakH, toPakI, toPakJ);
  serializer.unPack(buf, 0, unPakA, unPakB, unPakC, unPakD, unPakE, unPakF,
                    unPakG, unPakH, unPakI, unPakJ);

  REQUIRE(toPakA == unPakA);
  REQUIRE(toPakB == unPakB);
  REQUIRE(toPakC == unPakC);
  REQUIRE(toPakD == unPakD);
  REQUIRE(toPakH == unPakH);
  REQUIRE(toPakI == unPakI);
  REQUIRE(toPakE == unPakE);
  REQUIRE(toPakF == unPakF);
  REQUIRE(toPakG == unPakG);
  REQUIRE(toPakJ == unPakJ);
}

TEST_CASE("Multiple unsigned integers pack & unpack test.") {
  u8 toPakA = 0;
  u8 toPakB = 255;
  u16 toPakC = 0;
  u16 toPakD = 65535;
  u32 toPakH = 0;
  u32 toPakI = 4294967295;
  u64 toPakE = 0;
  u64 toPakF = 1.844674407370955e19;
  uchar toPakG = 255;
  uchar toPakJ = 0;

  u8 unPakA;
  u8 unPakB;
  u16 unPakC;
  u16 unPakD;
  u32 unPakH;
  u32 unPakI;
  u64 unPakE;
  u64 unPakF;
  uchar unPakG;
  uchar unPakJ;

  Buffer buf(32);

  serializer.pack(buf, 0, toPakA, toPakB, toPakC, toPakD, toPakE, toPakF,
                  toPakG, toPakH, toPakI, toPakJ);
  serializer.unPack(buf, 0, unPakA, unPakB, unPakC, unPakD, unPakE, unPakF,
                    unPakG, unPakH, unPakI, unPakJ);

  REQUIRE(toPakA == unPakA);
  REQUIRE(toPakB == unPakB);
  REQUIRE(toPakC == unPakC);
  REQUIRE(toPakD == unPakD);
  REQUIRE(toPakH == unPakH);
  REQUIRE(toPakI == unPakI);
  REQUIRE(toPakE == unPakE);
  REQUIRE(toPakF == unPakF);
  REQUIRE(toPakG == unPakG);
  REQUIRE(toPakJ == unPakJ);
}

TEST_CASE("Multiple floating point pack & unpack test.") {
  float toPakA = std::numeric_limits<float>::max();
  float toPakB = std::numeric_limits<float>::min();
  double toPakC = std::numeric_limits<double>::min();
  double toPakD = std::numeric_limits<double>::max();

  float unPakA;
  float unPakB;
  double unPakC;
  double unPakD;

  Buffer buf(192);

  serializer.pack(buf, 0, toPakA, toPakB, toPakC, toPakD);
  serializer.unPack(buf, 0, unPakA, unPakB, unPakC, unPakD);

  REQUIRE(toPakA == unPakA);
  REQUIRE(toPakB == unPakB);
  REQUIRE(toPakC == unPakC);
  REQUIRE(toPakD == unPakD);
}
