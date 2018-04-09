//use c++11
//in term:: MSB(xmsb).x4.x3.x2.x1.LSB(x0)



#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdlib.h>
#include <bitset>




typedef unsigned long term;
#define popcount(x) __builtin_popcountl(x)


struct bdnode{
    std::string val;
    bdnode* left;
    bdnode* right;

};

struct implicant{
  term mask;
  term minterm;
};

template <class T>
bool notRepeated(std::vector<T>& v, T a);

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
      minterm=minterm&baseNot; // filter minterm[digit] to 0, when mask[digit]=1 (is "-")
      minterms[popcount(minterm)].push_back(minterm);
      used[popcount(minterm)].push_back(false);
    };
    void compareAll(term basemask, std::vector<implicant>& primes,std::map<term,intermittent*>& itmList){
      std::map<term,intermittent*>::iterator listLb;
      ///////////
      // for(int i=0;i<minterms.size();i++){
      //   std::cout << i<<" ones | ";
      //   printV(minterms[i]);
      // }
      /////////////////
      for(int ones=0;ones<minterms.size()-1;ones++){
        //compare between level
          for(int i=0;i<minterms[ones].size();i++){
            for(int j=0;j<minterms[ones+1].size();j++){
              //i is in lower 1s group [0...l-2], j is higher 1s group [1...l-1]
              term mask=minterms[ones][i] ^ minterms[ones+1][j];
              // std::cout << "mask is "<<mask <<" | "<<!(mask & (mask-1))<< '\n';
              if(!(mask & (mask-1))){ //check if mask is power of 2, *0 will always be false
                // i&j can be simplifted
                used[ones][i]=true;
                used[ones+1][j]=true;
                // std::cout << "find" << '\n';

                /// add mask & data
                mask=basemask|mask;
                listLb=itmList.lower_bound(mask);
                  //BUG:work for empty map?
                if(listLb!=itmList.end() && !(itmList.key_comp()(mask,listLb->first))){ //from https://stackoverflow.com/questions/97050/stdmap-insert-or-stdmap-find
                  //mask exist, apped to struct
                  listLb->second->add(minterms[ones][i]);
                  // std::cout << "b1" << '\n';

                }else{
                  //mask not exist, make new
                  intermittent* itmPtr=new intermittent();
                  itmPtr->construct(mask,max1s-1);
                  itmPtr->add(minterms[ones][i]);
                  itmList.insert(listLb,std::map<term,intermittent*>::value_type(mask,itmPtr));
                  ////////
                  // std::cout << "b2" << '\n';
                  // auto xxx1=itmList.rbegin()->second->minterms;
                  // auto xxx2=itmList.rbegin()->first;

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

      // std::cout << "start xxxxxxxxxxxx all" << '\n';


      // move unsed case to prime implicant
      std::vector<term> v;
      for(int ones=0;ones<used.size();ones++){
        for(int i=0;i<used[ones].size();i++){
          if(!used[ones][i] && notRepeated(v,minterms[ones][i])){
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

struct bitCount{
  int space;//space for count of single bit, (least to avoid overflow)
  std::vector<long> count;//can be optimsed, current method waste higher bit when length is small, merge term for large operation

  void construct(const std::vector<implicant>& in,int length=64) {//require length, allow optmise (merge)in future operation
    // std::cout << "****require 64 bit long******" << '\n';
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
    unsigned long tm=in[0].mask;

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
      tm=in[i].mask;
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

typedef bdnode* bdt;
typedef std::map<term,intermittent*> ItmList;
typedef std::map<term, std::vector<implicant>> PIchart;


//DEBUG FUNC
  template <class T>
  void printV(std::vector<T>& v, char end='\n', char m=' ');

  template <class T>
  void printBit(std::vector<T>& v, char end='\n', char m=' ');

  template <class T>
  int find(std::vector<T>& v, T a);

  void printPrime(term mask,term minterm,int len,char end='\n');
  void genInput(std::vector<term>& in,int digit,std::vector<std::string>& out);
  void printTree(bdt t, int depth);
  void printTreeRec( bdnode*  t, int depth,std::vector<std::string>& out);
  bool testCorrectness(bdt rt, std::vector<term> correct);
  int getTreeNodeNum(bdnode*  t);
  void getTreeNodeNumRec(bdnode*  t,int& count);
  std::string getMinterm(term i);

//END DEBUG FUNC

/// do not alter these two function declarations
bdt buildcompactbdt(const std::vector<std::string>& fvalues);
std::string evalcompactbdt(bdt t, const std::string& input);

bdt newnode(std::string val=std::string(), bdt left=NULL, bdt right=NULL );
void recTreeConstructor(bdnode* node,std::vector<implicant>& primes, term nodeLeft);
// inline int popcount(long a);
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



int main(){
  #define bitsize 8
  std::vector<term> input;
  // input={0,1,2,5,6,7,8,9,10,14};
  // input={197,0,56,48,35,107,116,144,8,174,179,128,141,171,141};
  input={0,1,5,8,10,13,10};




  // std::vector<std::string> fvalues={"000000","000001","000100","001000","000101","001010","001100","100100","001101","001110","110010","001111"};
  // std::vector<std::string> fvalues={"000000","000001","000010","000100","001000","000101","001010","001100","100100","001101","001110","110010","001111"};
  std::vector<std::string> fvalues;
  genInput(input,bitsize,fvalues);
  printV(input);
  printV(fvalues);


  bdt fbdt=buildcompactbdt(fvalues);

  // for(int i=0;i<fvalues.size();i++){
  //   std::cout << evalcompactbdt(fbdt,fvalues[i])<<" | "<< std::bitset<bitsize>(fvalues[i]) << '\n';
  // }
  //
  // std::cout << evalcompactbdt(fbdt,"1111") << '\n';
  printTree(fbdt,fvalues[0].size()+1);
std::cerr << "\n\n" << '\n';
  testCorrectness(fbdt,input);

  std::cout << "\n\n the tree has "<<getTreeNodeNum(fbdt) << " nodes\n";



  return 0;
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

bool is1(char c){
  if(int(c)==48){
  //check if the char is "0"
    return false;
  }else{
  // treat not "0" as 1
    return true;
  }
}

template <class T>
bool notRepeated(std::vector<T>& v, T a){
  for(int i=0;i<v.size();i++){
    if(v[i]==a){
      return false;
    }
  }
  v.push_back(a);
  return true;
}

bdt newnode(std::string val, bdt left, bdt right){
  bdt pt=new bdnode;
  pt->val=val;
  pt->left=left;
  pt->right=right;
  return pt;
}

// inline int popcount(long a){
//   return __builtin_popcountl(a);
// }

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
    // std::cout << "process first pass" << '\n';
    intermittent* itmPtr=new intermittent();
    itmPtr->construct(0,fvalues[0].size());
    for(int i=0;i<minterms.size();i++){
      itmPtr->add(minterms[i]);
    }
    itmPtr->compareAll(0,primes,itmList);

    delete itmPtr;

    //all other path
    while (!itmList.empty()) {//!itmList.empty()
      // std::cout << "process "<<debugcount<<" pass" << '\n';
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


    std::cout << "\n====================================\nsimplifted min term are:" << '\n';
    for(int x=0;x<primes.size();x++){
      printPrime(primes[x].mask,primes[x].minterm,fvalues[0].size());
    }

    //reduce no. of term of prime implicants by using Petrick's method
    // PIchart chart;
    // for(int i=0;i<minterms.size();i++){
    //   chart[minterms[i]]={};
    // }
    //
    // for(int i=0;i<primes.size();i++){
    //   int counter=popcount(primes[i].mask);
    //   term max=(1<<counter)-1;
    //   int digit=0;
    //   std::vector<int> shifts;
    //   shifts.reserve(counter);
    //
    //   //locate 1 in mask
    //   while (counter>0) {
    //     if((1L<<digit)&primes[i].mask){
    //       --counter;
    //       shifts.push_back(digit);
    //     }
    //     ++digit;
    //   }
    //   //produce all minterm it repersent
    //   for(term j=0;j<=max;j++){
    //     //iterate all combination
    //     term mask=0;
    //     for(int s=0;s<shifts.size();s++){
    //       if(j&(1L<<s)){
    //         mask=mask|(1L<<shifts[s]);
    //       }
    //     }
    //
    //     //minterm is primes[i].minterm|mask, for all the current primes[i], for all i
    //     printPrime(0,minterm,bitsize);
    //     term minterm=primes[i].minterm|mask;
    //     chart[minterm].push_back(primes[i]);
    //     chart[minterm].push_back(primes[i]);
    //
    //   }
    // }



    //make tree
    bdt rootpt = newnode();
    recTreeConstructor(rootpt,primes,(1L<<fvalues[0].size())-1);

    return rootpt;
}

void recTreeConstructor(bdnode* node,std::vector<implicant>& primes, term nodeLeft){//nodeLeft => 0 for not used node, 1 for used node
  if(primes.size()==0){
    //check if case for 0, empty primes
    node->val="0";
    return;
  }else{
    int maskSum;

    //check if case for 1, one prime with --------
    int maxMaskCount=popcount(nodeLeft);
    std::vector<int> maskCount(primes.size(),0);
    for(int i=0;i<primes.size();i++){
      int cm=popcount(primes[i].mask);
      if(cm==maxMaskCount){
        node->val="1";
        return; //terminate loop
      }
      maskCount[i]=cm;
      maskSum=maskSum+cm;
    }

    //check if no mask, no "-"; use tree builder in ass1
    //not nessuary for correctness, speed up in case with no "-"
    // if(maskSum==0){
    //
    //   return;
    // }

    // hestristc, find node to chooice as mask next
    // direction of scan MSB <-- <-- LSB
    bitCount cnt; //no need to in struct move it to function
    unsigned int mincount=(int)-1;//get 0xffff....
    int optimalDigit;

    cnt.construct(primes);
    int counter=popcount(nodeLeft);
    int digit=0;
    while (counter>0) {
      if((1L<<digit)&nodeLeft){
        counter--;
        //node that is not in the tree, chooice between these
        if(cnt.get(digit)<mincount /*|| (cnt.get(digit)==mincount && maskCount[]) */){
          //TODO: look horozotinaly
          optimalDigit=digit;
          mincount=cnt.get(digit);
        }
      }
      digit++;
    }
    node->val="x"+std::to_string(optimalDigit+1);

    //RecCall, maske sub tree
    std::vector<implicant> leftPrimes;//left node, 0
    std::vector<implicant> rightPrimes;//right node, 1
    term optimalMask=1<<optimalDigit;
    for(int i=0;i<primes.size();i++){
      if( (primes[i].minterm&optimalMask)==0){
        //when minterm[digit]==0, mask[digit] may =1
        if( (primes[i].mask&optimalMask)!=0 ){
          //mask is one, need normalise mask and copy to both right & left listLb
          implicant temp=primes[i];
          temp.mask=temp.mask & (~optimalMask);
          leftPrimes.push_back(temp);
          rightPrimes.push_back(temp);
        }else{
          //mask is 0, normal case
          leftPrimes.push_back(primes[i]);
        }
      }else{
        //minterm[digit]==1 -> mask[digit] cant be 1, no need to check
        rightPrimes.push_back(primes[i]);
      }
    }

    // node->val="x"+
    node->left=newnode();
    recTreeConstructor(node->left,leftPrimes, nodeLeft& (~optimalMask));

    node->right=newnode();
    recTreeConstructor(node->right,rightPrimes, nodeLeft& (~optimalMask));
  }
}

std::string evalcompactbdt(bdt t, const std::string& input){
    while(t->val.size()>1){
      if(is1(input[stoi(t->val.substr(1))-1])){
        //right node
        t=t->right;
      }else{
        //left node
        t=t->left;
      }
    }

    return t->val;
}




/// add here the implementation for any other functions you wish to define and use
//DEBUG FUNC


  template <class T>
  int find(std::vector<T>& v, T a){
    for(int i=0;i<v.size();i++){
      if(v[i]==a){
        return i;
      }
    }
    // v.push_back(a);
    return -1;
  }

  std::string getMinterm(term i){
    long printmask=1;
    std::string temp;
    for(int j=bitsize-1;j>=0;j--){
      if(i&(printmask<<(j))){
        temp+='1';
      }else{
        temp+='0';
      }
    }
    return temp;
  }

  template <class T>
  void printV(std::vector<T>& v, char end, char m){
    for (int i=0;i<v.size();++i){
      std::cout << v[i]<<m;
    }
    std::cout << end;
  }

  template <class T>
  void printBit(std::vector<T>& v, char end, char m){
    for (int i=0;i<v.size();++i){
      std::bitset<bitsize> b(v[i]);
      std::cout << b<<m;
    }
    std::cout << end;
  }

  void printPrime(term mask,term minterm,int len,char end){
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

  int getTreeNodeNum(bdnode*  t) {
    int count=0;
    getTreeNodeNumRec(t,count);
    return count;
  }

  void getTreeNodeNumRec(bdnode*  t,int& count){
    if(t!=NULL){
      count++;
      getTreeNodeNumRec(t->left,count);
      getTreeNodeNumRec(t->right,count);
    }
  }

  void printTreeRec( bdnode*  t, int depth,std::vector<std::string>& out){
    if(t!=NULL){
      // out[depth].append("\t"+(t->val));
      out[depth]=out[depth]+"  "+t->val;
      printTreeRec(t->left,depth+1,out);
      printTreeRec(t->right,depth+1,out);
    }
  }

  void printTree(bdt t, int depth){
    std::vector<std::string> out;

    for(int i=0;i<depth;i++){
      out.push_back("");
    }

    printTreeRec(t,0,out);

    for (int i=0;i<depth;i++){
      std::cout << i<<"\t| "<<out[i]  << '\n';
    }

  }

  bool testCorrectness(bdt rt, std::vector<term> correct){
    std::cerr << "test start" << '\n';
    term max=(1<<bitsize)-1;
    std::vector<term> outTerm;
    std::vector<bool> outBool;
    outTerm.reserve(correct.size());
    outBool.reserve(correct.size());


    for(term i=0;i<=max;i++){
      std::string temp=getMinterm(i);

      if(is1(evalcompactbdt(rt,temp)[0])){
        std::cout << temp<<"("<<i<<") " << ' '<<std::flush;
        outTerm.push_back(i);
        outBool.push_back(false);
      }
    }

    std::cout << "\n\n*************************************\n* all test done, generateing errors *\n*************************************\n";

    std::cout << "below should return 0, but should be 1:" << '\n';
    for(int i=0;i<correct.size();i++){
      int f=find(outTerm,correct[i]);
      if(f>=0){
        outBool[f]=true;
        // std::cout << outTerm[f] << '\n';
      }else{
        std::cout <<"term "<< getMinterm(outTerm[i])<< "("<<outTerm[i]<<")" << '\n';
      }
    }

    std::cout << "\nbelow should return 1, but should be 0:" << '\n';

    for(int i=0;i<outBool.size();i++){
      if(outBool[i]!=true){
        std::cout <<"term "<< getMinterm(outTerm[i])<< "("<<outTerm[i]<<")" << '\n';
      }
    }

    std::cout << "\nend of errors" << '\n';
  }

//END DEBUG FUNC
