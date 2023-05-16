#include "VxLite.hpp"

int main()
{
  srand(time(0));
  VxLite::sbs s(512, 512, 512, 8);
  const uint64_t bytes = s.xs*s.ys*s.zs*s.bpv;
  std::cout<<"Allocated space of "<<double(bytes)/(1024*1024*1024)<<" GB"<<std::endl;
  std::cout<<"Created bytespace of size "<<s.xs<<" by "<<s.ys<<" by "<<s.zs<<std::endl;
  std::cout<<"Each voxel is "<<s.bpv<<" byte(s)"<<std::endl;

  for(size_t z = 0; z < s.zs; z++)
  {
    for(size_t y = 0; y < s.ys; y++)
    {
      for(size_t x = 0; x < s.xs; x++)
      {
        for(size_t b = 0; b < s.bpv; b++)
          s.at(x, y, z, b) = rand();
        //std::cout<<(int)s.at(x, y, z, 0)<<" ";
      }
      //std::cout<<std::endl;
    }
    //std::cout<<std::endl;
  }

  VxLite::sbs s2(s);
  VxLite::ctx context(&s2);
  std::cout<<"Optimizing filters"<<std::endl;
  context.OptimizeFilters();
  std::cout<<"Optimized filters for line sums"<<std::endl;
  std::cout<<"Filtering raw bytespace data..."<<std::endl;
  context.FilterSpace();
  std::cout<<"Filtered space"<<std::endl;
  // for(size_t z = 0; z < s.zs; z++)
  // {
  //   for(size_t y = 0; y < s.ys; y++)
  //   {
  //     for(size_t x = 0; x < s.xs; x++)
  //     {
  //       std::cout<<(int)s2.at(x, y, z, 0)<<" ";
  //     }
  //     std::cout<<std::endl;
  //   }
  //   std::cout<<std::endl;
  // }
  std::cout<<"Unfiltering raw bytespace data..."<<std::endl;
  context.UnfilterSpace();
  std::cout<<"Unfiltered space"<<std::endl;
  // for(size_t z = 0; z < s.zs; z++)
  // {
  //   for(size_t y = 0; y < s.ys; y++)
  //   {
  //     for(size_t x = 0; x < s.xs; x++)
  //     {
  //       std::cout<<(int)s2.at(x, y, z, 0)<<" ";
  //     }
  //     std::cout<<std::endl;
  //   }
  //   std::cout<<std::endl;
  // }
  std::cout<<"Verifying data..."<<std::endl;
  for(size_t z = 0; z < s.zs; z++)
  {
    for(size_t y = 0; y < s.ys; y++)
    {
      for(size_t x = 0; x < s.xs; x++)
      {
        for(size_t b = 0; b < s.bpv; b++)
        if(s2.get(x, y, z, b) != s.get(x, y, z, b))
        {
          std::cout<<"Major error at "<<x<<", "<<y<<", "<<z<<" byte "<<b<<std::endl;
          std::cout<<"Using filter "<<(int)context.filters[z*context.space->ys+y]<<std::endl;
        }
      }
    }
  }
  std::cout<<"Completed verification"<<std::endl;
  return 0;
}
