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
