#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <bitset>
#include <chrono>

typedef uint64_t wd;
typedef std::vector<wd> term;

#define bitsize 128
#define wdlength 64
#define startwithPI false
#define outputToFile false
#define wdMax 0xFFFFFFFFFFFFFFFF


term operator~(const term& v1){
  term out;
  out.reserve(v1.size());
  for(term::const_iterator it=v1.begin(); it!=v1.end(); ++it){
    out.push_back(~(*it));
  }
  return out;
}
term operator&(const term& v1,const term& v2){
  term out;
  out.reserve(v1.size());
  for(int i=0;i<v1.size();++i){
    out.push_back(v1[i]&v2[i]);
  }
  return out;
}
term operator|(const term& v1,const term& v2){
  term out;
  out.reserve(v1.size());
  for(int i=0;i<v1.size();++i){
    out.push_back(v1[i]|v2[i]);
  }
  return out;
}
term operator^(const term& v1,const term& v2){
  term out;
  out.reserve(v1.size());
  for(int i=0;i<v1.size();++i){
    out.push_back(v1[i]^v2[i]);
  }
  return out;
}

bool isPower2(const term& v){
  bool alreadyPw2=false;
  for(int i=0;i<v.size();++i){
    if(v[i] != 0){
      if((v[i] & (v[i] - 1))){//if is pw2, x&(x-1) is 0
        //when is not pw2
        return false;
      }else{
        //when is  pw2
        if(alreadyPw2){
          return false;
        }
        alreadyPw2=true;
      }
    }
  }
  return alreadyPw2; //will return true if is not all 0
}

const uint64_t m1  = 0x5555555555555555; //binary: 0101...
const uint64_t m2  = 0x3333333333333333; //binary: 00110011..
const uint64_t m4  = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
const uint64_t h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...
inline int popcount(uint64_t x){
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
    return (x * h01) >> 56;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...

}
int popcount(const term& v){
  int count=0;
  for(term::const_iterator it=v.begin(); it!=v.end(); ++it){
    count+=popcount(*it);
  }
  return count;
}

// void printPrime(const term& mask,const term& minterm, int len,const int wordlen,char end){
//   wd printmask=1;
//   int numWord=len/wordlen;
//   int lastlen=len%wordlen;
//
//   std::cout << "from Xn <-- X1: ";
//
//   for(int i=lastlen-1;i>=0;i--){
//     if(mask[numWord]&(printmask<<(i))){
//       std::cout<<'-';
//     }else{
//       if(minterm[numWord]&(printmask<<(i))){
//         std::cout<<'1';
//       }else{
//         std::cout<<'0';
//       }
//     }
//   }
//
//   numWord--;
//
//   while (numWord>=0) {
//     std::cout << " ";
//     for(int i=wordlen-1;i>=0;i--){
//       if(mask[numWord]&(printmask<<(i))){
//         std::cout<<'-';
//       }else{
//         if(minterm[numWord]&(printmask<<(i))){
//           std::cout<<'1';
//         }else{
//           std::cout<<'0';
//         }
//       }
//     }
//     numWord--;
//   }
//
//   std::cout << end;
// }
void printPrime(const term& mask,const term& minterm, int len, char end){
  wd printmask=1;
  int numWord=len/wdlength;
  int lastlen=len%wdlength;

  std::cout << "from Xn <-- X1: ";

  for(int i=lastlen-1;i>=0;i--){
    if(mask[numWord]&(printmask<<(i))){
      std::cout<<'-';
    }else{
      if(minterm[numWord]&(printmask<<(i))){
        std::cout<<'1';
      }else{
        std::cout<<'0';
      }
    }
  }

  numWord--;

  while (numWord>=0) {
    std::cout << " " << '\n';
    for(int i=wdlength-1;i>=0;i--){
      if(mask[numWord]&(printmask<<(i))){
        std::cout<<'-';
      }else{
        if(minterm[numWord]&(printmask<<(i))){
          std::cout<<'1';
        }else{
          std::cout<<'0';
        }
      }
    }
    numWord--;
  }

  std::cout << end;
}

bool is1(char c){
  if(int(c)==48){
  //check if the char is "0"
    return false;
  }else{
  // treat not "0" as 1
    return true;
  }
}
struct implicant{
  term mask;
  term minterm;
};

void printMinterm(const term& minterm){
  std::cout << "from Xn <-- X1: ";
  for(int i=minterm.size()-1;i>=0;--i){

    std::cout << std::bitset<wdlength>(minterm[i])<<' ';
  }
  std::cout << '\n';
}

void genMinterm(const std::vector<std::string>& fvalues, std::vector<term>& minterms){ //checked order right
  int numwd=fvalues[0].size()/wdlength;
  int lastlen=fvalues[0].size()%wdlength;
  for(int i=0;i<fvalues.size();i++){
    term minterm(numwd+1,0);
    int p=fvalues[0].size()-1;

    //MS wd
    wd word=0;
    for(int z=lastlen-1;z>=0;--z){
      if(is1(fvalues[i][p])){
        word=word*2+1;
      }else{
        word=word*2;
      }
      --p;
    }
    minterm[numwd]=word;

    //less siginficant wd
    while (p>=0) {
      wd word=0;
      for(int z=wdlength-1;z>=0;--z){
        if(is1(fvalues[i][p])){
          word=word*2+1;
        }else{
          word=word*2;
        }
        --p;
      }

      minterm[(p+1)/wdlength]=word;
    }
      std::cout << minterm[0] << '\n';
    printMinterm(minterm);
    minterms[i]=minterm;
  }
}

inline int leading1(uint64_t x){ ///01000 returns 4
  x=x|(x>>1);
  x=x|(x>>2);
  x=x|(x>>4);
  x=x|(x>>8);
  x=x|(x>>16);
  x=x|(x>>32);

  return popcount(x);

}

struct bitCount{//need remake~~~~
  //for this function, do count recurseively for each unit64_t term (first 64, than later 64) etc
  //also take input of nodeRemains, and only count these???? (difficult opt for time)

  int space;//space for count of single bit, (least to avoid overflow)
  std::vector<wd> count;//can be optimsed, current method waste higher bit when length is small, merge term for large operation

  void construct(const std::vector<implicant>& in) {
    // std::cout << "****require 64 bit long******" << '\n';
    space=leading1(in.size());//leading 1 from RHS
    // std::cout << "space is "<<space << '\n';

    //set up correct mask
    wd mask=1;
    for(int i=space;i<wdlength;){
      mask=mask|(mask<<(i));
      i=i*2;
    }
    // std::cout << "mask is "<<std::bitset<64>(mask) << '\n';

    int maxZone=in[0].mask.size();
    count.reserve(space*maxZone);//BUG mut by the size of term (when its vector)

    term tm=in[0].mask;
    //1st
    for(int z=0;z<maxZone;++z){
      for(int j=0;j<space;j++){
        count.push_back( (tm[z]>>j) & (mask) );
        // std::cout << count[j] << '\n';
      }

    }
    // std::cout << "first count is" << '\n';
    // for(int i=0;i<count.size();i++){
    //   std::cout << std::bitset<64>(count[i]) << '\n';
    // }
    //after 1
    for(int i=1;i<in.size();i++){
      tm=in[i].mask;
      for(int z=0;z<maxZone;++z){
        for(int j=0;j<space;j++){
          count[j+z*space]+= ((tm[z]>>j) & (mask));
        }
      }
      std::cout << i<<" count is" << '\n';
      for(int xx=0;xx<count.size();xx++){
        std::cout << std::bitset<64>(count[xx]) << '\n';
      }
      std::cout << '\n';
    }
  }

  unsigned int get(int digit){
    int zone=digit/wdlength;
    digit=digit%wdlength;
    int shift=digit/space*space;
    // std::cout << "shift is "<<shift << '\n';
    // std::cerr << (((long)1<<(space+1))-1) << '\n';
    return (count[zone*space+digit%space]>>shift) &  ((1ULL<<(space))-1);//(((long)1<<(space+1))-1) produce a mask of 1 [from LSB to space], x^(space+1)-1
  }

};

template <class T>
void printV(std::vector<T>& v, char end){
  for (int i=v.size()-1;i>=0;--i){
    std::cout << std::bitset<64>(v[i])<<" ";
  }
  std::cout << end;
}

void checkBitwise();
void checkGenminterm();
void checkCountbit();
int main(int argc, char const *argv[]) {
  // checkCountbit();
// checkBitwise();
term x={1,1};
term y={2,3};
term z={10,10};
x=y^z;
printMinterm(x);
// term NotoptimalMask(3,wdMax);
// printMinterm(NotoptimalMask);

  // term nodeRemains={0,0};
  // int digit=65;
  // std::cout << ((1ULL<<(digit%wdlength))&nodeRemains[digit/wdlength]) << '\n';
  //
  // if(nodeRemains==(term){0}){
  //   std::cout << "x" << '\n';
  // }

  // std::vector<std::string> fvalues={"1111"};
  //
  // term nodeRemains;
  //   int numwd=fvalues[0].size()/wdlength;
  //   int lastlen=fvalues[0].size()%wdlength;
  //
  //   for(int i=0;i<numwd;++i){
  //     nodeRemains.push_back((signed long long)-1);
  //   }
  //
  //   if(lastlen>0){
  //     nodeRemains.push_back((1ULL<<lastlen)-1);
  //   }
  //
  //   std::cout << nodeRemains.size() << '\n';
  //   printMinterm(nodeRemains);


  return 0;
}

void checkCountbit(){
  std::vector<implicant> v;
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});
  v.push_back((implicant){{0xFFF,77},{0}});

  bitCount x;
  x.construct(v);
  std::cout << v.size() << '\n';
  // std::cout << x.get(1) << '\n';
  for(int i=0;i<128;i++){
    std::cout << i<<"| "<< x.get(i) << '\n';
  }
}

void checkGenminterm(){
  std::vector<std::string> fvalues={"110011111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"};
  std::vector<term> minterms(fvalues.size());
  genMinterm(fvalues,minterms);
}

void checkBitwise(){
  //check bitwise overload
  term x={160,10};
  term y={192,12};
  term a,b,c,d;
    printV(x,'\n');
    a=~x;
    printV(a,'\n');
    b=y&x;
    printV(b,'\n');
    b=x&y;
    printV(b,'\n');
    c=y|x;
    printV(c,'\n');
    c=x|y;
    printV(c,'\n');
    d=y^x;
    printV(d,'\n');
    d=x^y;
    printV(d,'\n');
}
