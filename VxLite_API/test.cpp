#include "VxLite.hpp"
#include <chrono>

int main()
{
  srand(time(0));
  VxLite::sbs s(128, 128, 512, 32);
  const uint64_t bytes = s.xs*s.ys*s.zs*s.bpv;
  std::cout<<"Allocated space of "<<double(bytes)/(1024*1024)<<" MB"<<std::endl;
  std::cout<<"Created bytespace of size "<<s.xs<<" by "<<s.ys<<" by "<<s.zs<<std::endl;
  std::cout<<"Each voxel is "<<s.bpv<<" byte(s)"<<std::endl;

  for(size_t z = 0; z < s.zs; z++)
  {
    for(size_t y = 0; y < s.ys; y++)
    {
      for(size_t x = 0; x < s.xs; x++)
      {
        for(size_t b = 0; b < s.bpv; b++)
          s.at(x, y, z, b) = rand() % 16;
        //std::cout<<(int)s.at(x, y, z, 0)<<" ";
      }
      //std::cout<<std::endl;
    }
    //std::cout<<std::endl;
  }

  VxLite::sbs s2(s);
  VxLite::ctx context(&s2);

  std::cout<<"Optimizing filters with method 1"<<std::endl;
  auto t0 = std::chrono::high_resolution_clock::now();
  context.OptimizeFilters();
  auto t1 = std::chrono::high_resolution_clock::now();
  uint64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
  std::cout<<"Optimization took "<<elapsed<<"ms"<<std::endl;

  std::cout<<"Optimizing filters with method 2"<<std::endl;
  t0 = std::chrono::high_resolution_clock::now();
  context.OptimizeFilters2();
  t1 = std::chrono::high_resolution_clock::now();
  elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
  std::cout<<"Optimization took "<<elapsed<<"ms"<<std::endl;

  std::cout<<"Optimized filters for line sums"<<std::endl<<std::endl;

  VxLite::vls_file file = {0, 0, 0, 0, 0, 0, nullptr, nullptr};
  std::cout<<"Compressing unfiltered data..."<<std::endl;
  context.Compress(file);
  std::cout<<"Compressed bytespace to "<<file.CompressedSpaceDataSize<<" bytes before filtering"<<std::endl<<std::endl;

  std::cout<<"Filtering raw bytespace data..."<<std::endl;
  context.FilterSpace();
  std::cout<<"Filtered space"<<std::endl<<std::endl;
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
  std::cout<<"Compressing filtered data..."<<std::endl;
  context.Compress(file);
  std::cout<<"Compressed bytespace to "<<file.CompressedSpaceDataSize<<" bytes after filtering"<<std::endl<<std::endl;
  std::cout<<"Total size is "<<file.CompressedSpaceDataSize+file.CompressedFilterDataSize<<" bytes after filtering"<<std::endl<<std::endl;

  // Destroy the original data
  for(size_t z = 0; z < s.zs; z++)
  {
    for(size_t y = 0; y < s.ys; y++)
    {
      for(size_t x = 0; x < s.xs; x++)
      {
        for(size_t b = 0; b < s.bpv; b++)
          s2.at(x, y, z, b) = rand();
        //std::cout<<(int)s.at(x, y, z, 0)<<" ";
      }
      //std::cout<<std::endl;
    }
    //std::cout<<std::endl;
  }

  std::cout<<"Testing decompression..."<<std::endl;
  context.Decompress(file);
  std::cout<<"Decompressed bytespace and filters."<<std::endl<<std::endl;
  std::cout<<"Unfiltering raw bytespace data..."<<std::endl;
  context.UnfilterSpace();
  std::cout<<"Unfiltered space"<<std::endl<<std::endl;
  // for(size_t z = 0; z < s.zs; z++)
  // {
  //   for(size_t y = 0; y < s.ys; y++)//std::cout<<"Sum is "<<sum<<std::endl;

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
  if(file.CompressedSpaceData != nullptr) free(file.CompressedSpaceData);
  if(file.CompressedFilterData != nullptr) free(file.CompressedFilterData);
  return 0;
}
