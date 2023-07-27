//     This file is part of VxLite.
//
//     VxLite is free software: you can redistribute it and/or modify
//     it under the terms of the Lesser GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     VxLite is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     Lesser GNU General Public License for more details.
//
//     You should have received a copy of the Lesser GNU General Public License
//     along with VxLite. If not, see <http://www.gnu.org/licenses/>.

#ifndef VXLITE_INCLUDED
#define VXLITE_INCLUDED

#include <iostream>
#include <fstream>
#include <cstdint>

namespace VxLite
{
  // A sparse bytespace is a rectangular `xs` * `ys` * `zs` prism of voxels, each of which is `bpv` bytes long.
  struct sbs
  {
    sbs();
    sbs(const sbs& _s);
    //sbs(const uint64_t _xs, const uint64_t _ys, const uint64_t _zs, const uint64_t _bpv);

    sbs(const size_t _xs, const size_t _ys, const size_t _zs, const size_t _bpv);

      ~sbs();

    // `at()` accesses the byte at the (x, y, z) voxel with `offset` relative to the voxel.
    constexpr uint8_t& at(const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
    {
      return *(data + (z*xs*ys + y*xs + x)*bpv + offset);
    }

    constexpr uint8_t get(const uint64_t x, const uint64_t y, const uint64_t z, const uint64_t offset)
    {
      if(x >= xs) return 0;
      if(y >= ys) return 0;
      if(z >= zs) return 0;
      if(offset >= bpv) return 0;
      return at(x, y, z, offset);
    }

    uint64_t xs;
    uint64_t ys;
    uint64_t zs;
    uint64_t bpv;
    uint8_t* data;
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
    uint8_t* CompressedFilterData = nullptr;
    uint8_t* CompressedSpaceData = nullptr;
    ~vls_file()
    {
      if(CompressedFilterData != nullptr) delete[] CompressedFilterData;
      if(CompressedSpaceData != nullptr) delete[] CompressedSpaceData;
    }
  };

  // Return a pointer to a loaded vls_file from disk.
  static vls_file* OpenFromFile(const std::string& filename)
  {
    vls_file* ret = new vls_file;
    std::ifstream infile(filename, std::ios::binary);
    if(!infile.is_open()) return nullptr;

    char buf[8];

    infile.read(buf, 8);
    ret->xs = 0;
    for(int i = 0; i < 8; i++)
    {
      ret->xs <<= 8;
      ret->xs |= (uint64_t(buf[7-i]) & 0xFF);
    }

    infile.read(buf, 8);
    ret->ys = 0;
    for(int i = 0; i < 8; i++)
    {
      ret->ys <<= 8;
      ret->ys |= (uint64_t(buf[7-i]) & 0xFF);
    }

    infile.read(buf, 8);
    ret->zs = 0;
    for(int i = 0; i < 8; i++)
    {
      ret->zs <<= 8;
      ret->zs |= (uint64_t(buf[7-i]) & 0xFF);
    }

    infile.read(buf, 8);
    ret->bpv = 0;
    for(int i = 0; i < 8; i++)
    {
      ret->bpv <<= 8;
      ret->bpv |= (uint64_t(buf[7-i]) & 0xFF);
    }

    infile.read(buf, 8);
    ret->CompressedFilterDataSize = 0;
    for(int i = 0; i < 8; i++)
    {
      ret->CompressedFilterDataSize <<= 8;
      ret->CompressedFilterDataSize |= (uint64_t(buf[7-i]) & 0xFF);
    }

    infile.read(buf, 8);
    ret->CompressedSpaceDataSize = 0;
    for(int i = 0; i < 8; i++)
    {
      ret->CompressedSpaceDataSize <<= 8;
      ret->CompressedSpaceDataSize |= (uint64_t(buf[7-i]) & 0xFF);
    }

    ret->CompressedFilterData = new uint8_t[ret->CompressedFilterDataSize];
    ret->CompressedSpaceData = new uint8_t[ret->CompressedSpaceDataSize];

    infile.read((char*)ret->CompressedFilterData, ret->CompressedFilterDataSize);
    infile.read((char*)ret->CompressedSpaceData, ret->CompressedSpaceDataSize);

    return ret;
  }

  // Save a vls_file instance to disk.
  static void SaveToFile(const vls_file& file, const std::string filename)
  {
    std::ofstream outfile(filename, std::ofstream::out);
    if(!outfile.is_open()) return;

    char buf[8];
    for(int i = 0; i < 8; i++)
    {
      buf[i] = uint8_t(file.xs >> (i*8));
    }
    outfile.write(buf, 8);

    for(int i = 0; i < 8; i++)
    {
      buf[i] = uint8_t(file.ys >> (i*8));
    }
    outfile.write(buf, 8);

    for(int i = 0; i < 8; i++)
    {
      buf[i] = uint8_t(file.zs >> (i*8));
    }
    outfile.write(buf, 8);

    for(int i = 0; i < 8; i++)
    {
      buf[i] = uint8_t(file.bpv >> (i*8));
    }
    outfile.write(buf, 8);

    for(int i = 0; i < 8; i++)
    {
      buf[i] = uint8_t(file.CompressedFilterDataSize >> (i*8));
    }
    outfile.write(buf, 8);

    uint64_t d = file.CompressedSpaceDataSize;
    for(int i = 0; i < 8; i++)
    {
      buf[i] = uint8_t(file.CompressedSpaceDataSize >> (i*8));
    }

    outfile.write(buf, 8);

    outfile.write((char*)file.CompressedFilterData, file.CompressedFilterDataSize);
    outfile.write((char*)file.CompressedSpaceData, file.CompressedSpaceDataSize);

    outfile.close();
  }

  // A compression context contains a pointer to a sparse bytespace and a collection of filters for each line of bytes along the X-axis.
  class ctx
  {
  public:
    ctx();
    ctx(sbs* _space);
    ~ctx();

    void OptimizeFilters_old();
    void OptimizeFilters();
    void FilterSpace();
    void UnfilterSpace();
    void Compress(vls_file&);
    void Decompress(const vls_file&);

    sbs* space;
    uint8_t* filters;
  private:
  };
}

#endif
