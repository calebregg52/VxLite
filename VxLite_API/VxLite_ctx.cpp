#include "VxLite.hpp"
#include "lz4hc.h"

VxLite::ctx::ctx()
{
  space = nullptr;
  filters = nullptr;
}

VxLite::ctx::ctx(VxLite::sbs* _space) : space(_space)
{
  // Filters are applied along the X-axis.
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
  return space->get(x-1, y, z, offset);
}

constexpr uint8_t predFiltUp(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return space->get(x, y-1, z, offset);
}

constexpr uint8_t predFiltBack(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return space->get(x, y, z-1, offset);
}

constexpr uint8_t predFiltAvgXY(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return (space->get(x-1, y, z, offset) + space->get(x, y-1, z, offset)) >> 1;
}

constexpr uint8_t predFiltAvgXZ(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return (space->get(x-1, y, z, offset) + space->get(x, y, z-1, offset)) >> 1;
}

constexpr uint8_t predFiltAvgYZ(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{
  return (space->get(x, y-1, z, offset) + space->get(x, y, z-1, offset)) >> 1;
}

#define abs(x) ((x >= 0) ? x : -x)

constexpr uint8_t predFiltPaeth2(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{

  const uint8_t a = space->get(x-1, y, z, offset);
  const uint8_t b = space->get(x, y-1, z, offset);
  const uint8_t c = space->get(x-1, y-1, z, offset);

  const int p = a + b - c;

  const uint8_t pa = abs(p-a);
  const uint8_t pb = abs(p-b);
  const uint8_t pc = abs(p-c);

  if(pa <= pb && pa <= pc) return a;
  else if(pb <= pc) return b;
  else return c;
}

constexpr uint8_t predFiltPaeth3(VxLite::sbs* space, const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
{

  const uint8_t a = space->get(x-1, y, z, offset);
  const uint8_t b = space->get(x, y-1, z, offset);
  const uint8_t c = space->get(x-1, y-1, z, offset);
  const uint8_t d = space->get(x-1, y-1, z-1, offset);
  const uint8_t g = space->get(x, y, z-1, offset);

  const int p = a + b + d - c - g;

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

const int FILTERS_CNT = sizeof(predictionFilters)/sizeof(predictionFilters[0]);

void VxLite::ctx::FilterSpace()
{
  //std::cout<<"Using "<<FILTERS_CNT<<" filters"<<std::endl;
  for(int64_t z = space->zs-1; z >= 0; z--)
    for(int64_t y = space->ys-1; y >= 0; y--)
    {
      const uint8_t filter = filters[z*space->ys + y];
      for(int64_t x = space->xs-1; x >= 0; x--)
        for(int64_t b = space->bpv-1; b >= 0; b--)
        {
          //std::cout<<"Filtering "<<x<<", "<<y<<", "<<z<<", "<<b<<std::endl;
          space->at(x, y, z, b) -= predictionFilters[filter](space, x, y, z, b);
        }
    }
}

void VxLite::ctx::UnfilterSpace()
{
  for(uint64_t z = 0; z < space->zs; ++z)
    for(uint64_t y = 0; y < space->ys; ++y)
    {
      const uint8_t filter = filters[z*space->ys + y];
      //std::cout<<"Line at "<<y<<", "<<z<<" uses filter "<<(int)filter<<std::endl;
      for(uint64_t x = 0; x < space->xs; ++x)
        for(uint64_t b = 0; b < space->bpv; ++b)
        {
          //std::cout<<"Unfiltering "<<x<<", "<<y<<", "<<z<<", "<<b<<std::endl;
          space->at(x, y, z, b) += predictionFilters[filter](space, x, y, z, b);
        }
    }
}

void VxLite::ctx::OptimizeFilters()
{
  std::cout<<"Using "<<FILTERS_CNT<<" filters"<<std::endl;
  for(int64_t z = space->zs-1; z >= 0; z--)
    for(int64_t y = space->ys-1; y >= 0; y--)
    {
      uint64_t bestSum = -1;
      uint8_t bestFilter = 0;
      for(int f = 0; f < FILTERS_CNT; ++f)
      {
        uint64_t sum = 0;
        for(int64_t x = space->xs-1; x >= 0; x--)
        {
          for(int64_t b = space->bpv-1; b >= 0; b--)
          {
            //std::cout<<"Filtering "<<x<<", "<<y<<", "<<z<<", "<<b<<std::endl;
            sum += abs(space->at(x, y, z, b) - predictionFilters[f](space, x, y, z, b));
          }
        }
        //std::cout<<"Sum is "<<sum<<std::endl;
        if(sum < bestSum)
        {
          bestSum = sum;
          bestFilter = f;
        }
      }
      //std::cout<<"Best filter for "<<z<<", "<<y<<" was "<<(int)bestFilter<<" with sum "<<bestSum<<std::endl;
      filters[z*space->ys+y] = bestFilter;
    }
}
