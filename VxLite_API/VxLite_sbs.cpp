#include "VxLite.hpp"

VxLite::sbs::sbs()
{
  data = nullptr;
  xs = ys = zs = bpv = -1;
}

VxLite::sbs::sbs(const size_t _xs, const size_t _ys, const size_t _zs, const size_t _bpv) : xs(_xs), ys(_ys), zs(_zs), bpv(_bpv)
{
  data = new uint8_t[_xs*_ys*_zs*_bpv];
}

VxLite::sbs::~sbs()
{
  if(data != nullptr) delete[] data;
}
