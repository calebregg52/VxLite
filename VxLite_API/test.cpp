#include "VxLite.hpp"

int main()
{
  VxLite::sbs s(4, 4, 4, 1);
  std::cout<<"Created bytespace of size "<<s.xs<<" by "<<s.ys<<" by "<<s.zs<<std::endl;
  std::cout<<"Each voxel is "<<s.bpv<<" byte(s)"<<std::endl;
  s.at(0, 0, 0, 0) = 253;
  s.at(3, 3, 3, 0) = 253;
  for(size_t z = 0; z < s.zs; z++)
  {
    for(size_t y = 0; y < s.ys; y++)
    {
      for(size_t x = 0; x < s.xs; x++)
      {
        std::cout<<(int)s.at(x, y, z, 0)<<std::endl;
      }
    }
  }
  return 0;
}
