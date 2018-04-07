#include <iostream>
#include <map>
#include <bitset>
#include <vector>


struct bitCount{
  int space;//space for count of single bit, (least to avoid overflow)
  std::vector<long> count;//can be optimsed, current method waste higher bit when length is small, merge term for large operation

  void construct(std::vector<long>& in,int length=64) {//require length, allow optmise (merge)in future operation
    std::cout << "****require 64 bit long******" << '\n';
    space=32-__builtin_clz(in.size());//leading 1 from RHS

    //set up correct mask
    unsigned long mask=1;
    for(int i=space;i<64;){
      mask=mask|(mask<<(i));
      i=i*2;
    }
    // std::bitset<64> xxx(mask);
    // std::cout << "mask is "<<xxx << '\n';

    //cacualte the vec
    count.reserve(space);
    unsigned long tm=in[0];

    //1st
    for(int j=0;j<space;j++){
      count.push_back((tm&(mask<<(j)))>>j);
      // std::cout << count[j] << '\n';
    }
    // for(int i=0;i<count.size();i++){
    //   std::bitset<64>  x(count[i]);
    //   std::cout << x << '\n';
    // }
    //after 1
    for(int i=1;i<in.size();i++){
      tm=in[i];
      for(int j=0;j<space;j++){
        count[j]=count[j]+((tm&(mask<<(j)))>>j);
      }
      // for(int xx=0;xx<count.size();xx++){
      //   std::bitset<64>  x(count[xx]);
      //   std::cout << x << '\n';
      // }
      // std::cout << '\n';
    }
  }

  int get(int digit){
    unsigned long shift=digit/space*space;
    // std::cout << "shift is "<<shift << '\n';
    // std::cerr << (((long)1<<(space+1))-1) << '\n';
    return ( count[digit%space] & ( ((1L<<(space+1))-1) <<(shift)) )>>shift;//(((long)1<<(space+1))-1) produce a mask of 1 [from LSB to space], x^(space+1)-1
  }

};

int main ()
{
  std::vector<long> in={11,15,8,11,15,8};
  bitCount count;

  count.construct(in);
  for(int i=0;i<=4;i++){
    std::cout << i<<"th bit is | "<<count.get(i) << '\n';
  }

  std::cout << "\n size is"<<count.space << '\n';
  for(int i=0;i<count.count.size();i++){
    std::bitset<64>  x(count.count[i]);
    std::cout << x << '\n';
  }


  const long long1=1;
  std::bitset<64> yyy(((1L<<(60+1))-1));
  std::cerr << yyy << '\n';


}
