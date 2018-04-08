//change order of bit
//to MSB(xmsb).x4.x3.x2.x1.LSB(x0)

#include <iostream>
#include <map>
#include <bitset>
#include <vector>
#include <string>
#include <cstdlib>

#define popcount(x) __builtin_popcountl(x)
#define bitsize 6

typedef unsigned long term;

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
      count.push_back( (tm>>j) & (mask) );
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
        count[j]=count[j]+ ((tm>>j) & (mask));
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
    return (count[digit%space]>>shift) &  ((1L<<(space+1))-1);//(((long)1<<(space+1))-1) produce a mask of 1 [from LSB to space], x^(space+1)-1
  }

};

bool is1(char c){
  if(int(c)==48){
  //check if the char is "0"
    return false;
  }else{
  // treat not "0" as 1
    return true;
  }
}
void genMinterm(const std::vector<std::string>& fvalues, std::vector<term>& minterms){
  for(int i=0;i<fvalues.size();i++){
    int minterm=0;
    for(int p=fvalues[0].size()-1;p>=0;--p){
      if(is1(fvalues[i][p])){
        minterm=minterm*2+1;
      }else{
        minterm=minterm*2;
      }
    }
    minterms[i]=minterm;
  }
}
void printPrime(term mask,term minterm,int len,char end='\n'){
  term printmask=1;
  for(int i=0;i<len;i++){
    if(mask&(printmask<<(i))){
      std::cout<<'-';
    }else{
      if(minterm&(printmask<<(i))){
        std::cout<<'1';
      }else{
        std::cout<<'0';
      }
    }
  }
  std::cout << end;
}
void genInput(std::vector<term>& in,int digit,std::vector<std::string>& out){
  term printmask=1;
  for(int i=0;i<in.size();i++){
    std::string temp;
    for(int j=digit-1;j>=0;j--){
      if(in[i]&(printmask<<(j))){
        temp+='1';
      }else{
        temp+='0';
      }
    }
    // std::cout << in << '\n';
    out.push_back(temp);
  }
}


template <class T>
void printV(std::vector<T>& v, char end='\n', char m=' '){
  for (int i=0;i<v.size();++i){
    std::cout << v[i]<<m;
  }
  std::cout << end;
}

template <class T>
void printBit(std::vector<T>& v, char end='\n', char m=' '){
  for (int i=0;i<v.size();++i){
    std::bitset<bitsize> b(v[i]);
    std::cout << b<<m;
  }
  std::cout << end;
}

int main ()
{
  std::vector<std::string> fvalues;
  std::vector<term> input={0,2,5,10,12,13,14,16,15,20,50};
  genInput(input,6,fvalues);
  printV(input);
  printV(fvalues);

  std::vector<term> minterms(fvalues.size());
  genMinterm(fvalues,minterms);
  printBit(minterms);
}
