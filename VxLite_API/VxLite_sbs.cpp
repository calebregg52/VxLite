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

#include "VxLite.hpp"

VxLite::sbs::sbs()
{
  data = nullptr;
  xs = ys = zs = bpv = -1;
}

VxLite::sbs::sbs(const VxLite::sbs& _s)
{
  xs = _s.xs;
  ys = _s.ys;
  zs = _s.zs;
  bpv = _s.bpv;
  if(_s.data != nullptr)
  {
    data = new uint8_t[xs*ys*zs*bpv];
    for(uint64_t i = 0; i < xs*ys*zs*bpv; ++i) data[i] = _s.data[i];
  }
}

VxLite::sbs::sbs(const size_t _xs, const size_t _ys, const size_t _zs, const size_t _bpv) : xs(_xs), ys(_ys), zs(_zs), bpv(_bpv)
{
  data = new uint8_t[_xs*_ys*_zs*_bpv];
}

VxLite::sbs::~sbs()
{
  if(data != nullptr) delete[] data;
}
