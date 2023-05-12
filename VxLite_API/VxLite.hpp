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
    sbs(const size_t _xs, const size_t _ys, const size_t _zs, const size_t _bpv);
    ~sbs();

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
    void OptimizeFilters();
  private:
    sbs* space;
    uint8_t* filters;
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
