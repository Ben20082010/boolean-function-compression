//use c++11
//HAS RECEIVED PERMISSION from MAX, to return vector in the overload functions
//in term:: MSB(xmsb).x4.x3.x2.x1.LSB(x0)



#include <string>
#include <vector>
#include <map>
#include <stdlib.h>

//for DEBUG
#include <chrono>
#include <math.h>
#include <iostream>
#include <bitset>


#define wdlength 64
#define wdMax 0xFFFFFFFFFFFFFFFF
typedef uint64_t wd;
typedef std::vector<wd> term;


//DEBUG config
#define bitsize 15
#define startwithPI false
#define outputToFile false

//function used by struct
term operator~(const term& v1);
term operator&(const term& v1,const term& v2);
term operator|(const term& v1,const term& v2);
term operator^(const term& v1,const term& v2);

template <class T>
bool notRepeated(std::vector<T>& v, T a);
inline int popcount(uint64_t x);
inline int popcount(const term& v);
inline int leading1(uint64_t x);
bool isPower2(const term& v);

//DEBUG functions
void printPrime(const term& mask,const term& minterm, int len=bitsize, char end='\n');
void printMinterm(const term& minterm);
//END DEBUG functions

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
  term baseNot;
  std::vector<std::vector<term>> minterms;
  std::vector<std::vector<bool>> used;

  void construct(const term& mask,int len){
    max1s=len;
    baseNot=~mask;

    for(int i=0;i<=max1s;i++){
      minterms.push_back(std::vector<term>());
      used.push_back(std::vector<bool>());
    }
    // printMinterm(baseNot);
  };

  void add(term minterm) {
    minterm=minterm&baseNot; // filter minterm[digit] to 0, when mask[digit]=1 (is "-")
    int pcount=popcount(minterm);

    if(notRepeated(minterms[pcount],minterm)){
      used[pcount].push_back(false);
    }

    // minterms[pcount].push_back(minterm);
    // used[pcount].push_back(false);


  };
  void compareAll(term basemask, std::vector<implicant>& primes,std::map<term,intermittent*>& itmList){
    std::map<term,intermittent*>::iterator listLb;

    for(int ones=0;ones<minterms.size()-1;ones++){
      //compare between level
      for(int i=0;i<minterms[ones].size();i++){
        for(int j=0;j<minterms[ones+1].size();j++){
          //i is in lower 1s group [0...l-2], j is higher 1s group [1...l-1]
          term mask=minterms[ones][i] ^ minterms[ones+1][j];
          if(isPower2(mask)){
            // i&j can be simplifted
            used[ones][i]=true;
            used[ones+1][j]=true;

            /// add mask & data
            mask=basemask|mask;
            listLb=itmList.lower_bound(mask);
            //BUG:work for empty map?
            if(listLb!=itmList.end() && !(itmList.key_comp()(mask,listLb->first))){ //from https://stackoverflow.com/questions/97050/stdmap-insert-or-stdmap-find
              //mask exist, apped to struct
              listLb->second->add(minterms[ones][i]);


            }else{
              //mask not exist, make new
              intermittent* itmPtr=new intermittent();
              itmPtr->construct(mask,max1s-1);
              itmPtr->add(minterms[ones][i]);
              itmList.insert(listLb,std::map<term,intermittent*>::value_type(mask,itmPtr));
            }
          }
        }
      }
    }

    // move unsed case to prime implicant
    std::vector<term> v;
    for(int ones=0;ones<used.size();ones++){
      for(int i=0;i<used[ones].size();i++){
        if(!used[ones][i] && notRepeated(v,minterms[ones][i])){
          primes.push_back((implicant){basemask,minterms[ones][i]});
        }
      }
    }
  }
};

struct bitCount{
  // this struct is used to accerate bit count of each digit, by mask, shift and add

  int space;//leaset amount of space required for each digit to avoid overflow of digit
  int size;//size of input vector
  std::vector<wd> maskcount;
  std::vector<wd> termcount;

  void construct(const std::vector<implicant>& in) {
    size=in.size();
    space=leading1(size);//leading 1 from RHS

    //set up correct mask
    wd mask=1;
    for(int i=space;i<wdlength;){
      mask=mask|(mask<<(i));
      i=i*2;
    }

    // set up space
    int maxZone=in[0].mask.size();
    maskcount.reserve(space*maxZone);
    termcount.reserve(space*maxZone);

    term tm=in[0].mask;
    term tr=in[0].minterm;
    //1st term
    for(int z=0;z<maxZone;++z){
      for(int j=0;j<space;j++){
        maskcount.push_back( (tm[z]>>j) & (mask) );
        termcount.push_back( (tr[z]>>j) & (mask) );
      }

    }

    //after 1st term, repeat until all terms had been counted
    for(int i=1;i<in.size();i++){
      tm=in[i].mask;
      tr=in[i].minterm;
      for(int z=0;z<maxZone;++z){
        for(int j=0;j<space;j++){
          maskcount[j+z*space]+= ((tm[z]>>j) & (mask));
          termcount[j+z*space]+= ((tr[z]>>j) & (mask));
        }
      }
    }
  }

  unsigned int getMask(int digit){
    int zone=digit/wdlength;
    digit=digit%wdlength;
    int shift=digit/space*space;
    return (maskcount[zone*space+digit%space]>>shift) &  ((1ULL<<(space))-1);
  }

  unsigned int getTermLeast(int digit){
    int zone=digit/wdlength;
    digit=digit%wdlength;
    int shift=digit/space*space;

    int ones=(termcount[zone*space+digit%space]>>shift) &  ((1ULL<<(space))-1);

    if(ones>size/2){
      return size-ones;
    }else{
      return ones;
    }
  }

};

// typedef for struct
typedef bdnode* bdt;
typedef std::map<term,intermittent*> ItmList;
typedef std::map<term, std::vector<implicant>> PIchart;


//DEBUG FUNC
template <class T>
void printV(std::vector<T>& v, char end='\n', char m=' ');

template <class T>
int find(std::vector<T>& v, T a);
void genInput(std::vector<wd>& in,int digit,std::vector<std::string>& out);
void printTree(bdt t, int depth);
void printTreeRec( bdnode*  t, int depth,std::vector<std::string>& out);
void testCorrectness(bdt rt, const std::vector<wd>& correct);
int getTreeNodeNum(bdnode*  t);
void getTreeNodeNumRec(bdnode*  t,int& count);
std::string getMinterm(wd i);
double getAss1NodeNum(int size);
void appedPrime(std::string str, std::vector<implicant>& primes,bool fval=true);
void deltree(bdt t);
//END DEBUG FUNC

/// do not alter these two function declarations
bdt buildcompactbdt(const std::vector<std::string>& fvalues);
std::string evalcompactbdt(bdt t, const std::string& input);

///functions for the implementation of buildcompactbdt and evalcompactbdt
bdt newnode(std::string val=std::string(), bdt left=NULL, bdt right=NULL );
void recTreeConstructor(bdnode* node,const std::vector<implicant>& primes, const term& nodeRemains);
void genMinterm(const std::vector<std::string>& fvalues, std::vector<term>& minterms);
bool is1(char c);



int main(){


  std::vector<wd> input;
  input={0, 2, 4, 8, 9, 10, 11, 12, 16, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 39, 40, 44, 46, 49, 51, 52, 61, 63};
  // input={0, 2, 3, 5, 6, 9, 10, 11, 12, 13, 15, 17, 18, 19, 21, 27, 28, 30, 32, 34, 35, 36, 41, 42, 43, 44, 47, 51, 52, 56, 57, 58, 63, 67, 69, 70, 74, 76, 77, 78, 79, 81, 88, 89, 91, 93, 94, 97, 98, 99, 101, 103, 106, 107, 110, 112, 113, 114, 118, 119, 126, 127, 128, 129, 131, 132, 133, 134, 135, 137, 141, 142, 144, 145, 147, 148, 149, 151, 152, 154, 158, 159, 163, 165, 166, 167, 168, 170, 171, 172, 174, 175, 176, 178, 181, 182, 183, 185, 187, 188, 189, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 205, 206, 210, 214, 218, 219, 220, 221, 222, 223, 228, 229, 231, 232, 235, 236, 241, 245, 249, 250, 251, 252, 254, 258, 262, 265, 266, 267, 269, 270, 271, 273, 274, 280, 282, 285, 289, 290, 291, 292, 298, 301, 302, 304, 306, 307, 308, 310, 312, 313, 316, 317, 318, 319, 320, 321, 323, 325, 327, 328, 331, 338, 339, 340, 346, 348, 350, 352, 354, 355, 358, 359, 360, 363, 364, 369, 370, 373, 375, 380, 382, 385, 386, 387, 391, 392, 393, 396, 397, 400, 403, 405, 409, 411, 413, 414, 416, 417, 419, 421, 423, 424, 425, 426, 427, 429, 431, 435, 437, 439, 442, 443, 444, 448, 450, 452, 454, 456, 460, 461, 463, 465, 469, 472, 474, 475, 477, 478, 479, 485, 486, 487, 489, 492, 493, 496, 499, 500, 501, 503, 511, 512, 513, 519, 520, 521, 522, 525, 527, 531, 532, 534, 535, 536, 541, 543, 546, 548, 549, 550, 551, 554, 555, 556, 557, 558, 561, 563, 564, 567, 568, 569, 573, 574, 575, 576, 577, 580, 582, 583, 584, 586, 588, 590, 591, 593, 594, 596, 598, 601, 609, 611, 613, 615, 617, 620, 625, 628, 629, 630, 631, 632, 633, 634, 635, 640, 643, 644, 647, 648, 649, 650, 652, 654, 655, 657, 658, 659, 660, 661, 662, 667, 668, 670, 671, 672, 673, 675, 682, 686, 689, 690, 701, 702, 704, 706, 707, 708, 709, 710, 712, 713, 714, 719, 720, 722, 727, 728, 731, 732, 733, 735, 736, 737, 739, 740, 743, 744, 745, 746, 748, 751, 753, 756, 760, 763, 764, 765, 767, 769, 770, 772, 773, 774, 776, 778, 781, 782, 783, 784, 787, 788, 789, 790, 791, 794, 798, 799, 800, 801, 802, 803, 804, 807, 809, 812, 813, 818, 820, 822, 823, 825, 827, 828, 829, 830, 831, 832, 834, 836, 837, 838, 840, 842, 843, 844, 845, 846, 848, 849, 852, 854, 856, 857, 859, 860, 863, 864, 865, 866, 867, 872, 873, 874, 875, 878, 879, 880, 881, 882, 885, 886, 888, 890, 895, 896, 902, 904, 905, 907, 908, 909, 910, 913, 915, 919, 921, 922, 923, 924, 925, 929, 932, 933, 934, 935, 938, 939, 945, 951, 952, 954, 956, 957, 962, 963, 966, 967, 968, 969, 971, 975, 981, 982, 984, 986, 991, 997, 1000};
  // input={40, 51, 37, 38, 27, 50, 14, 5, 42, 53, 61, 20, 21, 16, 15, 22, 63, 30, 25, 47, 6, 19, 17, 23, 13, 56, 26, 9, 52, 45, 24, 12, 1, 7, 62, 41, 28, 32, 58, 8, 0, 29, 39, 35, 43, 60, 57, 59, 11, 31, 3, 44, 54, 46, 36, 4, 48, 2, 34, 18};
  // input={104, 89, 69, 74, 25, 31, 23, 75, 57, 81, 0, 13, 60, 61, 119, 71, 79, 46, 124, 93, 22, 32, 112, 39, 102, 94, 41, 28, 101, 64, 17, 77, 21, 35, 83, 99, 88, 54, 53, 26, 92, 18, 55, 36, 48, 125, 68, 91, 49, 42, 4, 33, 27, 44, 84, 107, 12, 6, 34, 66, 123, 113, 5, 30, 16, 38, 116, 15, 29, 37, 97, 115, 73, 62, 78, 96, 87, 50, 63, 45, 8, 7, 10, 100, 11, 19, 105, 82, 76, 20, 9, 98, 108, 1, 52, 51, 2, 118, 120, 110, 122, 59, 103, 86, 109, 3, 58, 65, 126, 106, 95, 80, 70, 72, 90, 127, 114, 56, 121, 67, 85, 111, 14, 117, 40, 47, 24, 43};
  // input={0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
  if(outputToFile){
    freopen("output.txt","w",stdout);
  }

  // std::vector<std::string> fvalues={"000000","000001","000100","001000","000101","001010","001100","100100","001101","001110","110010","001111"};
  std::vector<std::string> fvalues={
    "100010101010000000001111111111111100000000000000000011111111111111111111110000000000000000111111111100000000000000100000000000000",
    "100000101010000000001111111111111100000000000000000011111111111111111111110000000000000000111111111100000000000000100000000000000",
    "100010101010000001001111111111111100000000000000000011111111111111111111110000000000000000111111111100000000000000100000000000000",
    "100010101010000010001111111111111100000000000000000011111111111111111111110000000000000000111111111100000000000000100000000000000",
    "100010101010000011001111111111111100000000000000000011111111111111111111110000000000000000111111111100000000000000100000000000000"};
  // std::vector<std::string> fvalues;
  // genInput(input,bitsize,fvalues);
  // printV(input);
  printV(fvalues);

  auto begin = std::chrono::high_resolution_clock::now();
  bdt fbdt=buildcompactbdt(fvalues);
  auto end = std::chrono::high_resolution_clock::now();

  // testCorrectness(fbdt,input);

  printTree(fbdt,fvalues[0].size()+1);
  std::cout <<'\n';
  std::cout<<evalcompactbdt(fbdt,"100010101010000000001111111111111100000000000000000011111111111111111111110000000000000000111111111100000000000000100000000000000")<<'\n';
  std::cout<<evalcompactbdt(fbdt,"100000101010000000001111111111111100000000000000000011111111111111111111110000000000000000111111111100000000000000100000000000000")<<'\n';
  std::cout<<evalcompactbdt(fbdt,"100010101010000001001111111111111100000000000000000011111111111111111111110000000000000000111111111100000000000000100000000000000")<<'\n';
  std::cout<<evalcompactbdt(fbdt,"100010101010000010001111111111111100000000000000000011111111111111111111110000000000000000111111111100000000000000100000000000000")<<'\n';
  std::cout<<evalcompactbdt(fbdt,"100010101010000011001111111111111100000000000000000011111111111111111111110000000000000000111111111100000000000000100000000000000")<<'\n';

  double nodeNum=getTreeNodeNum(fbdt);
  std::cout << "\n\n the tree has "<<nodeNum << " nodes\ncompare to tree in ass1 require "<<getAss1NodeNum(fvalues[0].size())<<" node\nreduced by "<<((getAss1NodeNum(fvalues[0].size())-nodeNum)*100/getAss1NodeNum(fvalues[0].size()))<<"%\n";
  double t=std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
  std::cout << "\n the buildcompactbdt function takes "<< t/1000000 << "ms\n";
  std::cerr <<'\n';

  deltree(fbdt);
  return 0;
}

bdt buildcompactbdt(const std::vector<std::string>& fvalues){
  std::vector<implicant> primes;

  //the if condtion below is for debug code, skip QM when true
  // if(!startwithPI){
    std::cout << "start building trees" << '\n';
    /// write the implementation for the function here

    //convert to minterm
    //string cant be termer than 64 bit, using template to solve this
    std::cerr << "gen minterms" << '\n';
    std::vector<term> minterms(fvalues.size());
    genMinterm(fvalues,minterms);

    //find prime implicants by useing Quine–McCluskey
    ItmList itmList;

    //first pass
    std::cerr << "pepare to gen prime implicant" << '\n';
    intermittent* itmPtr=new intermittent();
    term zeroMask(fvalues[0].size()/wdlength+1,0);
    itmPtr->construct(zeroMask,fvalues[0].size());
    for(int i=0;i<minterms.size();i++){
      itmPtr->add(minterms[i]);
    }
    std::cerr << "gen prime implicant" << '\n';
    itmPtr->compareAll(zeroMask,primes,itmList);

    delete itmPtr;

    //all other pass
    while (!itmList.empty()) {
      ItmList tempList;
      for(ItmList::const_iterator  itr=itmList.begin();itr!=itmList.end();++itr ){
        intermittent* itmPtr=itr->second;
        itmPtr->compareAll(itr->first,primes,tempList);
        delete itmPtr; // avoid memroy leak & same memroy when map swap
      }

      itmList.swap(tempList);
      // std::cout << "next iteration" << '\n';
    }


    // std::cout << "\n====================================\nsimplifted min term are:" << '\n';
    // for(int x=0;x<primes.size();x++){
    //   printPrime(primes[x].mask,primes[x].minterm,fvalues[0].size());
    // }


  // }else{ // commented code below is for debug, will run when startwithPI is true
  //         bool fromfvalues=true;
  //         std::string PI;
  //
  //         std::cout << "is it from fvalues? 1 or 0" << '\n';
  //         std::cin >> PI;
  //         if(PI=="0"){
  //           fromfvalues=false;
  //         }
  //         PI="";
  //
  //         std::cout << "debug file, enter prime implicant" << '\n';
  //         std::cin >> PI;
  //         while (PI!="c"){
  //           appedPrime(PI,primes,fromfvalues);
  //           PI="";
  //           std::cin >> PI;
  //         };
  //         // std::cout << "/* message */" << '\n';
  // }



  //make tree
  bdt rootpt = newnode();

  term nodeRemains;
  int numwd=fvalues[0].size()/wdlength;
  int lastlen=fvalues[0].size()%wdlength;

  for(int i=0;i<numwd;++i){
    nodeRemains.push_back(wdMax);
  }

  if(lastlen>0){
    nodeRemains.push_back((1ULL<<lastlen)-1);
  }

  for(int o=0;o<primes.size();o++){
    printMinterm(primes[o].minterm);
  }

  std::cout << "\n\n==================================\nstart building tree, with node in consideration\n";
  printMinterm(nodeRemains);
  std::cout << '\n';
  recTreeConstructor(rootpt,primes,nodeRemains);

  return rootpt;
}

void recTreeConstructor(bdnode* node,const std::vector<implicant>& primes,const term& nodeRemains){//nodeRemains => 0 for not used node, 1 for used node
  // printMinterm(nodeRemains); // shows grapical view of the order node is choiced

  if(primes.size()==0){
    //check if case for 0, empty primes (base case)
    node->val="0";
    return;
  }else{
    // int maskSum;

    //check if case for 1, one prime with -------- (base case)
    int maxMaskCount=popcount(nodeRemains);

    printMinterm(nodeRemains);
    for(int i=0;i<primes.size();i++){
      int cm=popcount(primes[i].mask);

      if(cm==maxMaskCount){
        node->val="1";
        return; //terminate loop
      }
    }

    // hestristc, find node to chooice as mask next
    // direction of scan MSB <-- <-- LSB // condtion iii
    bitCount cnt;
    unsigned int mincount=(int)-1;//get 0xffff....
    unsigned int mostUnbalanced=(int)-1;
    int optimalDigit;

    cnt.construct(primes);
    int counter=popcount(nodeRemains);
    int digit=0;

    while (counter>0) {
      if((1ULL<<(digit%wdlength))&nodeRemains[digit/wdlength]){
        counter--;
        //node that is not in the tree, chooice between these
        // std::cout << "digit"<<digit<<": "<<cnt.getMask(digit) << '\n';
        if(cnt.getMask(digit)<mincount){
          // condtion i
          optimalDigit=digit;
          mincount=cnt.getMask(digit);
          mostUnbalanced=cnt.getTermLeast(digit);
        }else if(cnt.getMask(digit)==mincount && cnt.getTermLeast(digit)<mostUnbalanced){
          // condtion ii
          optimalDigit=digit;
          mostUnbalanced=cnt.getTermLeast(digit);
        }
      }
      //node already in tree, skip, dont consider
      digit++;
    }



    node->val="x"+std::to_string(optimalDigit+1);

    //RecCall, make sub tree
    std::vector<implicant> leftPrimes;//left node, 0
    std::vector<implicant> rightPrimes;//right node, 1

    wd optimalWord=1ULL<<(optimalDigit%wdlength);
    int optimalNum=optimalDigit/wdlength;
    term NotOptimalMask(nodeRemains.size(),wdMax);
    NotOptimalMask[optimalNum]=~optimalWord;

    for(int i=0;i<primes.size();i++){
      if( (primes[i].minterm[optimalNum]&optimalWord)==0){
        //when minterm[digit]==0, mask[digit] may =1
        if( (primes[i].mask[optimalNum]&optimalWord)!=0 ){
          //mask is one, need normalise mask and copy to both right & left listLb
          implicant temp=primes[i];
          temp.mask=temp.mask & NotOptimalMask;
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

    term nextNodeRemains=nodeRemains;
    nextNodeRemains[optimalNum]=nextNodeRemains[optimalNum] & (~optimalWord);

    node->left=newnode();
    recTreeConstructor(node->left,leftPrimes, nextNodeRemains);

    node->right=newnode();
    recTreeConstructor(node->right,rightPrimes, nextNodeRemains);
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


//overload operator
//NOTE:: Have contacted Max for exception of return vector in overload function below
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

//other functions

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

// modeified from https://stackoverflow.com/questions/23856596/how-to-count-leading-zeros-in-a-32-bit-unsigned-integer
inline int leading1(uint64_t x){
  x=x|(x>>1);
  x=x|(x>>2);
  x=x|(x>>4);
  x=x|(x>>8);
  x=x|(x>>16);
  x=x|(x>>32);

  return popcount(x);

}

// copyed from https://en.wikipedia.org/wiki/Hamming_weight
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

inline int popcount(const term& v){
  int count=0;
  for(term::const_iterator it=v.begin(); it!=v.end(); ++it){
    count+=popcount(*it);
  }
  return count;
}

//DEBUG FUNC
// below are all debug functions

template <class T>
void printV(std::vector<T>& v, char end, char m){
  for (int i=0;i<v.size();++i){
    std::cout << v[i]<<m;
  }
  std::cout << end;
}

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

void genInput(std::vector<wd>& in,int digit,std::vector<std::string>& out){
  wd printmask=1;
  for(int i=0;i<in.size();i++){
    std::string temp;
    for(int j=0;j<digit;j++){
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

double getAss1NodeNum(int size){
  return(pow(2.0,size+1)-1);
}

void deltree(bdt t){
  if(t!=NULL){
    deltree(t->left);
    deltree(t->right);
    delete t;
  }
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
    //   std::cout << minterm[0] << '\n';
    // printMinterm(minterm);
    minterms[i]=minterm;
  }
}

void printMinterm(const term& minterm){
  std::cout << "from Xn <-- X1: ";
  for(int i=minterm.size()-1;i>=0;--i){

    std::cout << std::bitset<wdlength>(minterm[i])<<' ';
  }
  std::cout << '\n';
}
void appedPrime(std::string str, std::vector<implicant>& primes, bool fval){
  wd mask=0;
  wd minterm=0;
  // term printmask=1;
  if(fval){
    for(int i=0;i<str.length();i++){
      if(int(str[i])==45){
        mask=mask|(1ULL<<i);
      }else if(int(str[i]==49)){
        minterm=minterm|(1ULL<<i);
      }
    }
  }else{
    for(int i=0;i<str.length();i++){
      if(int(str[i])==45){
        mask=mask|(1ULL<<(str.length()-i-1));
      }else if(int(str[i]==49)){
        minterm=minterm|(1ULL<<(str.length()-i-1));
      }
    }
  }

  primes.push_back((implicant){{mask},{minterm}});
  printPrime({mask},{minterm},bitsize);
}



std::string getMinterm(wd i){
  uint64_t printmask=1;
  std::string temp;
  for(int j=0;j<bitsize;j++){
    if(i&(printmask<<(j))){
      temp+='1';
    }else{
      temp+='0';
    }
  }
  return temp;
}

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

void testCorrectness(bdt rt, const std::vector<wd>& correct){
  std::cerr << "test start" << '\n';
  wd max=(1ULL<<bitsize)-1;
  std::vector<wd> outTerm;
  std::vector<bool> outBool;
  outTerm.reserve(correct.size());
  outBool.reserve(correct.size());


  for(wd i=0;i<=max;i++){
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
