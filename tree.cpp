/// include here all the headers you need (but see guidelines)
#include <iostream>
#include <string>
#include <vector>
#include <map>


//DEBUG
template <class T>
void printV(std::vector<T>& v, char end='\n', char m=' '){
  for (int i=0;i<v.size();++i){
    std::cout << v[i]<<m;
  }
  std::cout << end;
}

void printPrime(long mask,long minterm,int len,char end='\n'){
  long printmask=1;
  for(int i=len-1;i>=0;i--){
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

/// do not use using namespace std

/// do not alter the struct declaration

typedef long term;

struct bdnode{
    std::string val;
    bdnode* left;
    bdnode* right;

};

struct implicant{
  term mask;
  term minterm;
};



struct intermittent{
    int max1s;
    // term basemask;
    term baseNot;
    std::vector<std::vector<term>> minterms;
    std::vector<std::vector<bool>> used;

    void construct(term mask,int len){
      // basemask=mask;
      max1s=len;
      baseNot=~mask;
      // std::vector<std::vector<term>> minterms=std::vector<std::vector<term>> minterms(len+1);
      // std::vector<std::vector<bool>> used=std::vector<std::vector<bool>> used(len+1);
      for(int i=0;i<=max1s;i++){
        minterms.push_back(std::vector<term>());
        used.push_back(std::vector<bool>());
      }

    };
    void add(term minterm) {
      minterm=minterm&baseNot; // filter mask bit to 0
      minterms[__builtin_popcount(minterm)].push_back(minterm);
      used[__builtin_popcount(minterm)].push_back(false);
    };
    void compareAll(term basemask, std::vector<implicant>& primes,std::map<term,intermittent*>& itmList){
      std::map<term,intermittent*>::iterator listLb;
      ///////////
      for(int i=0;i<minterms.size();i++){
        std::cout << i<<" ones | ";
        printV(minterms[i]);
      }
      /////////////////
      for(int ones=0;ones<minterms.size()-1;ones++){
        //compare between level
          for(int i=0;i<minterms[ones].size();i++){
            for(int j=0;j<minterms[ones+1].size();j++){
              //i is in lower 1s group [0...l-2], j is higher 1s group [1...l-1]
              term mask=minterms[ones][i] ^ minterms[ones+1][j];
              std::cout << "mask is "<<mask <<" | "<<!(mask & (mask-1))<< '\n';
              if(!(mask & (mask-1))){ //check if mask is power of 2, *0 will always be false
                // i&j can be simplifted
                used[ones][i]=true;
                used[ones+1][j]=true;
                std::cout << "find" << '\n';

                /// add mask & data
                mask=basemask|mask;
                listLb=itmList.lower_bound(mask);
                  //BUG:work for empty map?
                if(listLb!=itmList.end() && !(itmList.key_comp()(mask,listLb->first))){ //from https://stackoverflow.com/questions/97050/stdmap-insert-or-stdmap-find
                  //mask exist, apped to struct
                  listLb->second->add(minterms[ones][i]);
                  std::cout << "b1" << '\n';

                }else{
                  //mask not exist, make new
                  intermittent* itmPtr=new intermittent();
                  itmPtr->construct(mask,max1s-1);
                  itmPtr->add(minterms[ones][i]);
                  itmList.insert(listLb,std::map<term,intermittent*>::value_type(mask,itmPtr));
                  ////////
                  std::cout << "b2" << '\n';
                  auto xxx1=itmList.rbegin()->second->minterms;
                  auto xxx2=itmList.rbegin()->first;

                  // for(int ii=0;ii<xxx1.size();ii++){
                  //   for(int jj=0;jj<xxx1[ii].size();jj++){
                  //     printPrime(xxx2,xxx1[ii][jj]);
                  //   }
                  // }
                  ////////
                }

                //// CHECK/BUG: CAN SAME MASK SHOW MUTIPLE TIMES? => YES!, using map

              }
            }
          }
      }

      std::cout << "start xxxxxxxxxxxx all" << '\n';


      // move unsed case to prime implicant
      for(int ones=0;ones<used.size();ones++){
        for(int i=0;i<used[ones].size();i++){
          if(!used[ones][i]){
            /////
            // std::cout << "term: " << '\n';
            // printPrime(basemask,minterms[ones][i]);
            // std::cout << (implicant){basemask,minterms[ones][i]} << '\n';

            //////
            primes.push_back((implicant){basemask,minterms[ones][i]});
          }
        }
      }
    }
};

typedef bdnode* bdt;
typedef std::map<term,intermittent*> ItmList;


/// do not alter these two function declarations
bdt buildcompactbdt(const std::vector<std::string>& fvalues);
std::string evalcompactbdt(bdt t, const std::string& input);

void genMinterm(const std::vector<std::string>& fvalues, std::vector<term>& minterms);
bool is1(char c);



// template <typename T>
// std::ostream& operator << (std::ostream &o, const implicant &a){
//   term printmask=1;
//   for(int i=0;i<(inputLen);++i){
//     if(a.mask&(printmask<<(i))){
//       o<<'-';
//     }else{
//       if(a.minterm&(printmask<<(i))){
//         o<<'1';
//       }else{
//         o<<'0';
//       }
//     }
//   }
//   return o;
// }

/// you can define and use additional functions
/// for the implementation of buildcompactbdt and evalcompactbdt

/// add here the declarations (not the implementation)
/// for any other functions you wish to define and use

/// before the submission you need to remove the main
/// (either delete it or comment it out)
/// otherwise it will intefere with the automated testing

int main(){


  std::vector<std::string> fvalues={"000000","000001","000100","001000","000101","001010","001100","100100","001101","001110","110010","001111"};
  printV(fvalues);
  buildcompactbdt(fvalues);


    /// write the code for the main here in order to test your functions

    return 0;
}

void genMinterm(const std::vector<std::string>& fvalues, std::vector<term>& minterms){
  for(int i=0;i<fvalues.size();i++){
    int minterm=0;
    for(int p=0;p<fvalues[0].size();p++){
      if(is1(fvalues[i][p])){
        minterm=minterm*2+1;
      }else{
        minterm=minterm*2;
      }
    }
    minterms[i]=minterm;
  }
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

bdt buildcompactbdt(const std::vector<std::string>& fvalues){
    /// write the implementation for the function here

    //convert to minterm
    //string cant be termer than 64 bit, using template to solve this
    std::vector<term> minterms(fvalues.size());
    genMinterm(fvalues,minterms);

    //find prime implicants by useing Quine–McCluskey
    std::vector<implicant> primes;
    ItmList itmList;

    //first pass
    std::cout << "process first pass" << '\n';
    intermittent* itmPtr=new intermittent();
    itmPtr->construct(0,fvalues[0].size());
    for(int i=0;i<minterms.size();i++){
      itmPtr->add(minterms[i]);
    }
    itmPtr->compareAll(0,primes,itmList);

    delete itmPtr;

    //all other path
    int debugcount=0;
    while (!itmList.empty()) {//!itmList.empty()
      std::cout << "process "<<debugcount<<" pass" << '\n';
      debugcount++;
      ItmList tempList;
      ///DEBUG
      if(!tempList.empty()){
        std::cout << "ERROE, init tempList is not empty" << '\n';
      }
      //END_DEBUG
      for(ItmList::const_iterator  itr=itmList.begin();itr!=itmList.end();++itr ){//use iterator  if throw error
        intermittent* itmPtr=itr->second;
        itmPtr->compareAll(itr->first,primes,tempList);
        delete itmPtr; // avoid memroy leak & same memroy when map swap
      }

      itmList.swap(tempList);
    }

    //reduce term of prime implicants by using Petrick's method
    std::cout << "simplifted min term are:" << '\n';
    for(int x=0;x<primes.size();x++){
      printPrime(primes[x].mask,primes[x].minterm,fvalues[0].size());
    }
}

std::string evalcompactbdt(bdt t, const std::string& input){
    /// write the implementation for the function here
}

/// add here the implementation for any other functions you wish to define and use
