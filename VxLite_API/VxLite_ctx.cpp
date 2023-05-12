#include "VxLite.hpp"

VxLite::ctx::ctx()
{
  space = nullptr;
  filters = nullptr;
}

VxLite::ctx::ctx(VxLite::sbs* _space) : space(_space)
{
  filters = new uint8_t[_space->ys*_space->zs];
}

VxLite::ctx::~ctx()
{
  if(filters != nullptr) delete[] filters;
}

constexpr uint8_t getPred(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return space->at(x, y, z, offset);
}

constexpr uint8_t predFiltNone(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return 0;
}

constexpr uint8_t predFiltSub(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return space->at(x-1, y, z, offset);
}

constexpr uint8_t predFiltUp(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return space->at(x, y-1, z, offset);
}

constexpr uint8_t predFiltBack(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return space->at(x, y, z-1, offset);
}

constexpr uint8_t predFiltAvgXY(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return (space->at(x-1, y, z, offset) + space->at(x, y-1, z, offset)) >> 1;
}

constexpr uint8_t predFiltAvgXZ(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return (space->at(x-1, y, z, offset) + space->at(x, y, z-1, offset)) >> 1;
}

constexpr uint8_t predFiltAvgYZ(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return (space->at(x, y-1, z, offset) + space->at(x, y, z-1, offset)) >> 1;
}

#define abs(x) ((x >= 0) ? x : -x)

constexpr uint8_t predFiltPaeth2(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  const uint8_t p = space->at(x, y, z, offset);

  const uint8_t a = space->at(x-1, y, z, offset);
  const uint8_t b = space->at(x, y-1, z, offset);
  const uint8_t c = space->at(x-1, y-1, z, offset);

  const uint8_t pa = abs(p-a);
  const uint8_t pb = abs(p-b);
  const uint8_t pc = abs(p-c);
  
  if(pa <= pb && pa <= pc) return a;
  else if(pb <= pc) return b;
  else return c;
}

constexpr uint8_t predFiltPaeth3(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  const uint8_t p = space->at(x, y, z, offset);

  const uint8_t a = space->at(x-1, y, z, offset);
  const uint8_t b = space->at(x, y-1, z, offset);
  const uint8_t c = space->at(x-1, y-1, z, offset);
  const uint8_t d = space->at(x-1, y-1, z-1, offset);
  const uint8_t g = space->at(x, y, z-1, offset);

  const uint8_t pa = abs(p-a);
  const uint8_t pb = abs(p-b);
  const uint8_t pc = abs(p-c);
  const uint8_t pd = abs(p-d);
  const uint8_t pg = abs(p-g);

  if(pa <= pb && pa <= pc && pa <= pd && pa <= pg) return a;
  else if(pb <= pc && pb <= pd && pb <= pg) return b;
  else if(pc <= pd && pc <=  pg) return c;
  else if(pd <= pg) return d;
  else return g;
}

#undef abs

constexpr static uint8_t (*predictionFilters[])(VxLite::sbs*, const uint64_t, const uint64_t, const uint64_t, const uint64_t) =
{
  predFiltNone,
  predFiltSub,
  predFiltUp,
  predFiltBack,
  predFiltAvgXY,
  predFiltAvgXZ,
  predFiltAvgYZ,
  predFiltPaeth2,
  predFiltPaeth3,
};

void VxLite::ctx::FilterSpace()
{

}

void VxLite::ctx::UnfilterSpace()
{

}
