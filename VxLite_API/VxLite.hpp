#ifndef VXLITE_INCLUDED
#define VXLITE_INCLUDED

#include <iostream>
#include <cstdint>

namespace VxLite
{
  // A sparse bytespace is a rectangular `xs` * `ys` * `zs` prism of voxels, each of which is `bpv` bytes long.
  struct sbs
  {
    sbs();
    sbs(const uint64_t _xs, const uint64_t _ys, const uint64_t _zs, const uint64_t _bpv);
    ~sbs();

    // `at()` accesses the byte at the (x, y, z) voxel with `offset` relative to the voxel.
    constexpr uint8_t& at(const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
    {
      return *(data + (z*xs*ys + y*xs + x)*bpv + offset);
    }

    constexpr uint8_t get(const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
    {
      if(x < 0 || x >= xs) return 0;
      if(y < 0 || y >= ys) return 0;
      if(z < 0 || z >= zs) return 0;
      return at(x, y, z, offset);
    }

    uint64_t xs;
    uint64_t ys;
    uint64_t zs;
    uint64_t bpv;
    uint8_t* data;
  };

  // A compression context contains a pointer to a sparse bytespace and a collection of filters for each line of bytes along the X-axis.
  class ctx
  {
  public:
    ctx();
    ctx(sbs* _space);
    ~ctx();

    // TODO: filter application and optimization
    void OptimizeFilters();
    void FilterSpace();
    void UnfilterSpace();

    sbs* space;
    uint8_t* filters;
  private:
  };

  // A VxLite compressed space (.vls file) is a vls file header followed by compressed filter data, and then compressed voxel data.
  struct vls_file
  {
    uint64_t xs;
    uint64_t ys;
    uint64_t zs;
    uint64_t bpv;

    uint64_t CompressedFilterDataSize;
    uint64_t CompressedSpaceDataSize;
    uint8_t* CompressedFilterData;
    uint8_t* CompressedSpaceData;
  };

  // TODO: Buffer-level compression and decompression
  vls_file CompressBytespace(uint8_t* in, const size_t _xs, const size_t _ys, const size_t _zs, const size_t _bpv);
  void DecompressBytespace(uint8_t* out, vls_file& f);
}

#endif
