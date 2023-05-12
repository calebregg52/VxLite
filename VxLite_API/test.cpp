#include "VxLite.hpp"

int main()
{
  VxLite::sbs s(10, 10, 10, 1);
  std::cout<<"Created bytespace of size "<<s.xs<<" by "<<s.ys<<" by "<<s.zs<<std::endl;
  std::cout<<"Each voxel is "<<s.bpv<<" byte(s)"<<std::endl;
  return 0;
}
