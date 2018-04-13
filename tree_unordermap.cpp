//use c++11
//in term:: MSB(xmsb).x4.x3.x2.x1.LSB(x0)



#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
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
void printPrime(term mask,term minterm,int len=27,char end='\n');

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
    void compareAll(term basemask, std::vector<implicant>& primes,std::unordered_map<term,intermittent*>& itmList){
      std::unordered_map<term,intermittent*>::iterator listLb;
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
                listLb=itmList.find(mask);
                //BUG, use two line below if using map
                // listLb=itmList.lower_bound(mask);
                // if(listLb!=itmList.end() && !(itmList.key_comp()(mask,listLb->first))){ //from https://stackoverflow.com/questions/97050/stdmap-insert-or-stdmap-find
                if(listLb!=itmList.end() ){ //from https://stackoverflow.com/questions/97050/stdmap-insert-or-stdmap-find

                  //mask exist, apped to struct
                  listLb->second->add(minterms[ones][i]);
                  // std::cout << "b1" << '\n';

                }else{
                  //mask not exist, make new
                  intermittent* itmPtr=new intermittent();
                  itmPtr->construct(mask,max1s-1);
                  itmPtr->add(minterms[ones][i]);
                  itmList.insert(listLb,std::unordered_map<term,intermittent*>::value_type(mask,itmPtr));
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
            printPrime(basemask,minterms[ones][i]);

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

  void construct(const std::vector<implicant>& in,int length=32) {//require length, allow optmise (merge)in future operation
    // std::cout << "****require 32 bit long******" << '\n';
    space=32-__builtin_clz(in.size());//leading 1 from RHS
    std::cout << "space is "<<space << '\n';

    //set up correct mask
    unsigned long mask=1;
    for(int i=space;i<32;){
      mask=mask|(mask<<(i));
      i=i*2;
    }
    // std::cout << "mask is "<<std::bitset<64>(mask) << '\n';

    //cacualte the vec
    count.reserve(space);
    unsigned long tm=in[0].mask;

    //1st
    for(int j=0;j<space;j++){
      count.push_back( (tm>>j) & (mask) );
      // std::cout << count[j] << '\n';
    }
    // std::cout << "first count is" << '\n';
    // for(int i=0;i<count.size();i++){
    //   std::cout << std::bitset<64>(count[i]) << '\n';
    // }
    //after 1
    for(int i=1;i<in.size();i++){
      tm=in[i].mask;
      for(int j=0;j<space;j++){
        count[j]=count[j]+ ((tm>>j) & (mask));
      }
      // std::cout << i<<" count is" << '\n';
      // for(int xx=0;xx<count.size();xx++){
      //   std::cout << std::bitset<64>(count[xx]) << '\n';
      // }
      // std::cout << '\n';
    }
  }

  int get(int digit){
    unsigned long shift=digit/space*space;
    // std::cout << "shift is "<<shift << '\n';
    // std::cerr << (((long)1<<(space+1))-1) << '\n';
    return (count[digit%space]>>shift) &  ((1L<<(space))-1);//(((long)1<<(space+1))-1) produce a mask of 1 [from LSB to space], x^(space+1)-1
  }

};

typedef bdnode* bdt;
typedef std::unordered_map<term,intermittent*> ItmList;
typedef std::unordered_map<term, std::vector<implicant>> PIchart;


//DEBUG FUNC
  template <class T>
  void printV(std::vector<T>& v, char end='\n', char m=' ');

  template <class T>
  void printBit(std::vector<T>& v, char end='\n', char m=' ');

  template <class T>
  int find(std::vector<T>& v, T a);

  void genInput(std::vector<term>& in,int digit,std::vector<std::string>& out);
  void printTree(bdt t, int depth);
  void printTreeRec( bdnode*  t, int depth,std::vector<std::string>& out);
  bool testCorrectness(bdt rt, std::vector<term> correct);
  int getTreeNodeNum(bdnode*  t);
  void getTreeNodeNumRec(bdnode*  t,int& count);
  std::string getMinterm(term i);
  long getAss1NodeNum();

  void appedPrime(std::string str, std::vector<implicant>& primes,bool fval=true);

  void deltree(bdt t);

//END DEBUG FUNC

/// do not alter these two function declarations
bdt buildcompactbdt(const std::vector<std::string>& fvalues);
std::string evalcompactbdt(bdt t, const std::string& input);

bdt newnode(std::string val=std::string(), bdt left=NULL, bdt right=NULL );
void recTreeConstructor(bdnode* node,std::vector<implicant>& primes, term nodeRemains);
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
  #define bitsize 11
  #define startwithPI false
  #define outputToFile false
  std::vector<term> input;
  // input={0,1,2,5,6,7,8,9,10,14};
  // input={4, 7, 8, 15, 20, 21, 26, 29, 30, 32, 35, 38, 43, 45, 46, 49, 52, 54, 55, 56, 57, 58, 59, 60, 62, 63, 64, 68, 69, 71, 72, 74, 77, 80, 82, 84, 88, 89, 90, 91, 92, 94, 95, 96, 100, 104, 105, 106, 109, 110, 111, 114, 115, 117, 121, 123, 126, 127, 129, 131, 132, 133, 137, 138, 139, 142, 143, 145, 146, 147, 150, 152, 153, 158, 160, 162, 163, 165, 166, 167, 168, 170, 171, 172, 173, 176, 177, 178, 179, 181, 183, 185, 186, 188, 190, 191, 195, 196, 197, 200};
  // 6 input={0, 2, 4, 8, 9, 10, 11, 12, 16, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 39, 40, 44, 46, 49, 51, 52, 61, 63};
  // input={0, 2, 3, 5, 6, 9, 10, 11, 12, 13, 15, 17, 18, 19, 21, 27, 28, 30, 32, 34, 35, 36, 41, 42, 43, 44, 47, 51, 52, 56, 57, 58, 63, 67, 69, 70, 74, 76, 77, 78, 79, 81, 88, 89, 91, 93, 94, 97, 98, 99, 101, 103, 106, 107, 110, 112, 113, 114, 118, 119, 126, 127, 128, 129, 131, 132, 133, 134, 135, 137, 141, 142, 144, 145, 147, 148, 149, 151, 152, 154, 158, 159, 163, 165, 166, 167, 168, 170, 171, 172, 174, 175, 176, 178, 181, 182, 183, 185, 187, 188, 189, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 205, 206, 210, 214, 218, 219, 220, 221, 222, 223, 228, 229, 231, 232, 235, 236, 241, 245, 249, 250, 251, 252, 254, 258, 262, 265, 266, 267, 269, 270, 271, 273, 274, 280, 282, 285, 289, 290, 291, 292, 298, 301, 302, 304, 306, 307, 308, 310, 312, 313, 316, 317, 318, 319, 320, 321, 323, 325, 327, 328, 331, 338, 339, 340, 346, 348, 350, 352, 354, 355, 358, 359, 360, 363, 364, 369, 370, 373, 375, 380, 382, 385, 386, 387, 391, 392, 393, 396, 397, 400, 403, 405, 409, 411, 413, 414, 416, 417, 419, 421, 423, 424, 425, 426, 427, 429, 431, 435, 437, 439, 442, 443, 444, 448, 450, 452, 454, 456, 460, 461, 463, 465, 469, 472, 474, 475, 477, 478, 479, 485, 486, 487, 489, 492, 493, 496, 499, 500, 501, 503, 511, 512, 513, 519, 520, 521, 522, 525, 527, 531, 532, 534, 535, 536, 541, 543, 546, 548, 549, 550, 551, 554, 555, 556, 557, 558, 561, 563, 564, 567, 568, 569, 573, 574, 575, 576, 577, 580, 582, 583, 584, 586, 588, 590, 591, 593, 594, 596, 598, 601, 609, 611, 613, 615, 617, 620, 625, 628, 629, 630, 631, 632, 633, 634, 635, 640, 643, 644, 647, 648, 649, 650, 652, 654, 655, 657, 658, 659, 660, 661, 662, 667, 668, 670, 671, 672, 673, 675, 682, 686, 689, 690, 701, 702, 704, 706, 707, 708, 709, 710, 712, 713, 714, 719, 720, 722, 727, 728, 731, 732, 733, 735, 736, 737, 739, 740, 743, 744, 745, 746, 748, 751, 753, 756, 760, 763, 764, 765, 767, 769, 770, 772, 773, 774, 776, 778, 781, 782, 783, 784, 787, 788, 789, 790, 791, 794, 798, 799, 800, 801, 802, 803, 804, 807, 809, 812, 813, 818, 820, 822, 823, 825, 827, 828, 829, 830, 831, 832, 834, 836, 837, 838, 840, 842, 843, 844, 845, 846, 848, 849, 852, 854, 856, 857, 859, 860, 863, 864, 865, 866, 867, 872, 873, 874, 875, 878, 879, 880, 881, 882, 885, 886, 888, 890, 895, 896, 902, 904, 905, 907, 908, 909, 910, 913, 915, 919, 921, 922, 923, 924, 925, 929, 932, 933, 934, 935, 938, 939, 945, 951, 952, 954, 956, 957, 962, 963, 966, 967, 968, 969, 971, 975, 981, 982, 984, 986, 991, 997, 1000};

input={393, 244, 943, 368, 238, 497, 829, 365, 522, 281, 106, 453, 597, 58, 414, 654, 210, 831, 1018, 897, 783, 241, 838, 619, 258, 319, 820, 937, 814, 313, 708, 405, 565, 172, 1020, 190, 259, 643, 885, 757, 747, 566, 373, 952, 177, 213, 678, 386, 872, 666, 124, 363, 192, 780, 812, 18, 119, 612, 455, 1015, 60, 410, 685, 47, 982, 999, 412, 684, 437, 561, 235, 148, 430, 502, 624, 981, 724, 881, 832, 105, 874, 433, 432, 130, 866, 129, 40, 300, 140, 487, 580, 786, 1, 568, 889, 728, 283, 150, 867, 341, 326, 343, 17, 178, 110, 137, 96, 687, 321, 574, 205, 263, 680, 796, 848, 411, 1022, 934, 906, 903, 267, 916, 200, 1006, 141, 389, 749, 722, 371, 960, 160, 603, 120, 908, 82, 600, 912, 471, 372, 171, 591, 721, 674, 133, 467, 63, 390, 801, 781, 880, 36, 242, 628, 227, 325, 311, 712, 818, 805, 226, 374, 626, 976, 289, 498, 199, 49, 413, 317, 604, 737, 620, 884, 310, 957, 367, 77, 1019, 694, 911, 775, 9, 136, 439, 927, 658, 216, 434, 166, 827, 12, 219, 813, 844, 142, 697, 492, 887, 62, 509, 337, 563, 625, 354, 353, 681, 406, 68, 369, 661, 501, 206, 631, 853, 861, 890, 879, 264, 609, 787, 334, 465, 3, 0, 629, 427, 257, 308, 250, 710, 208, 322, 944, 845, 428, 875, 75, 738, 478, 294, 750, 840, 182, 512, 282, 269, 544, 751, 627, 858, 27, 26, 113, 72, 772, 562, 543, 23, 836, 79, 969, 601, 977, 925, 429, 632, 615, 725, 938, 770, 709, 834, 842, 901, 946, 422, 637, 304, 634, 236, 183, 864, 876, 327, 773, 457, 164, 616, 740, 577, 569, 54, 990, 19, 331, 187, 395, 878, 91, 43, 401, 576, 987, 260, 963, 362, 95, 514, 65, 763, 797, 979, 852, 733, 25, 638, 594, 280, 13, 972, 128, 29, 53, 618, 821, 364, 527, 540, 197, 825, 755, 551, 155, 766, 112, 788, 175, 239, 20, 446, 394, 440, 915, 558, 186, 421, 107, 505, 117, 877, 290, 1021, 830, 99, 745, 799, 690, 94, 21, 729, 230, 669, 33, 214, 909, 161, 868, 312, 696, 1013, 209, 519, 649, 154, 964, 932, 375, 617, 807, 291, 1009, 970, 234, 533, 508, 380, 482, 859, 356, 224, 998, 882, 571, 306, 134, 926, 778, 595, 525, 557, 593, 1005, 316, 366, 2, 785, 385, 55, 466, 930, 584, 270, 988, 249, 657, 243, 32, 760, 810, 554, 24, 460, 102, 794, 132, 399, 111, 607, 535, 191, 309, 549, 81, 115, 204, 1017, 899, 66, 758, 436, 553, 194, 50, 93, 536, 993, 379, 1004, 841, 905, 816, 355, 804, 275, 288, 839, 324, 378, 920, 444, 808, 14, 328, 575, 635, 4, 587, 477, 201, 902, 240, 174, 459, 586, 426, 360, 913, 419, 188, 735, 555, 918, 646, 135, 345, 336, 44, 699, 744, 567, 35, 139, 452, 727, 931, 711, 564, 123, 388, 114, 583, 869, 762, 717, 784, 1016, 782, 425, 663, 835, 479, 997, 795, 689, 493, 376, 611, 768, 726, 854, 118, 675, 548, 790, 255, 888, 673, 975, 254, 806, 761, 37, 488, 147, 539, 315, 651, 404, 815, 16, 222, 639, 448, 948, 215, 332, 225, 1008, 1023, 245, 233, 391, 971, 485, 789, 156, 271, 949, 947, 279, 451, 458, 955, 886, 895, 39, 966, 30, 614, 579, 268, 303, 928, 528, 217, 252, 138, 361, 871, 400, 731, 169, 621, 734, 358, 481, 342, 370, 286, 92, 231, 423, 873, 917, 862, 774, 397, 146, 599, 623, 251, 803, 743, 572, 443, 330, 688, 144, 811, 556, 945, 180, 85, 56, 793, 276, 101, 333, 377, 179, 892, 706, 978, 122, 532, 883, 1002, 1000, 676, 513, 302, 648, 671, 958, 500, 415, 950, 472, 86, 48, 359, 67, 45, 7, 152, 347, 701, 170, 809, 157, 545, 798, 700, 705, 339, 203, 384, 1012, 515, 266, 531, 698, 857, 11, 494, 483, 940, 495, 645, 198, 153, 51, 702, 636, 8, 538, 1010, 103, 473, 610, 984, 246, 314, 529, 108, 42, 176, 476, 454, 418, 935, 753, 296, 475, 765, 402, 70, 237, 28, 382, 516, 891, 714, 822, 435, 942, 936, 100, 220, 965, 759, 995, 396, 677, 992, 305, 518, 46, 802, 278, 664, 652, 223, 742, 542, 748, 297, 961, 974, 983, 752, 715, 581, 78, 640, 431, 462, 31, 923, 76, 870, 843, 441, 622, 573, 143, 672, 228, 703, 962, 530, 691, 655, 907, 207, 320, 921, 989, 647, 849, 10, 552, 894, 865, 732, 817, 792, 83, 69, 74, 776, 833, 109, 1014, 416, 764, 87, 644, 524, 856, 605, 61, 126, 167, 295, 668, 642, 71, 349, 598, 469, 292, 159, 800, 771, 929, 900, 212, 504, 754, 941, 121, 767, 442, 165, 64, 104, 667, 116, 850, 407, 158, 318, 704, 896, 541, 445, 80, 503, 707, 823, 851, 272, 420, 506, 656, 173, 461, 480, 741, 560, 352, 686, 933, 287, 127, 590, 846, 720, 338, 910, 181, 954, 335, 608, 537, 660, 163, 417, 84, 89, 248, 329, 592, 904, 633, 38, 499, 299, 5, 511, 559, 456, 346, 922, 351, 449, 670, 695, 185, 484, 596, 837, 357, 59, 824, 277, 131, 520, 447, 408, 994, 723, 381, 307, 265, 967, 665, 550, 470, 184, 968, 348, 746, 973, 510, 679, 6, 298, 450, 247, 659, 229, 588, 924, 424, 195, 474, 898, 490, 996, 692, 863, 253, 398, 392, 719, 653, 193, 211, 409, 828, 791, 585, 468, 756, 273, 350, 613, 189, 606, 97, 777, 582, 387, 196, 125, 90, 578, 464, 301, 939, 293, 149, 221, 340, 285, 202, 517, 991, 736, 383, 919, 262, 716, 739, 168, 826, 274, 15, 893, 496, 403, 526, 779, 602, 489, 323, 22, 589, 88, 1011, 73, 284, 232, 662, 956, 145, 641, 630, 546, 41, 713, 986, 819, 860, 959, 256, 523, 547, 218, 34, 683, 1003, 438, 521, 1001, 52, 718, 98, 914, 847, 486, 57, 463, 951, 344, 1007, 650, 980, 162, 682, 507, 693, 769, 855, 953, 491, 985, 730, 261, 151, 534, 570};
  // input={40, 51, 37, 38, 27, 50, 14, 5, 42, 53, 61, 20, 21, 16, 15, 22, 63, 30, 25, 47, 6, 19, 17, 23, 13, 56, 26, 9, 52, 45, 24, 12, 1, 7, 62, 41, 28, 32, 58, 8, 0, 29, 39, 35, 43, 60, 57, 59, 11, 31, 3, 44, 54, 46, 36, 4, 48, 2, 34, 18};
// input={23, 6, 18, 8, 14, 0, 16, 31, 4, 10, 2, 13, 28, 12, 11, 17, 20, 26, 9, 5, 25, 30, 24, 29, 1, 3, 21, 15, 22, 7, 19, 27};
  if(outputToFile){
    freopen("output.txt","w",stdout);
  }

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
// std::cout << evalcompactbdt(fbdt,"0100") << '\n';
  testCorrectness(fbdt,input);

  long nodeNum=getTreeNodeNum(fbdt);
  std::cout << "\n\n the tree has "<<nodeNum << " nodes\ncompare to tree in ass1 require "<<getAss1NodeNum()<<" node\nreduced by "<<((getAss1NodeNum()-nodeNum)*100/getAss1NodeNum())<<"%\n";


  deltree(fbdt);
  return 0;
}

void genMinterm(const std::vector<std::string>& fvalues, std::vector<term>& minterms){ //checked order right
  for(int i=0;i<fvalues.size();i++){
    int minterm=0;
    for(int p=fvalues[0].size()-1;p>=0;--p){
      if(is1(fvalues[i][p])){
        minterm=minterm*2+1;
      }else{
        minterm=minterm*2;
      }
    }
    std::cout << "from Xn <-- X1: ";
    std::cout << std::bitset<bitsize>(minterm) << '\n';
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
  std::vector<implicant> primes;

    if(!startwithPI){
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
            // std::cout << "process first pass" << '\n';
            std::cerr << "pepare to gen prime implicant" << '\n';
            intermittent* itmPtr=new intermittent();
            itmPtr->construct(0,fvalues[0].size());
            for(int i=0;i<minterms.size();i++){
              itmPtr->add(minterms[i]);
            }
            std::cerr << "gen prime implicant" << '\n';
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
                // insert alternative here to exist earily
                std::cout << "1 | "<<std::flush;
                itmPtr->compareAll(itr->first,primes,tempList);
                delete itmPtr; // avoid memroy leak & same memroy when map swap
              }

              itmList.swap(tempList);
              std::cout << "next iteration" << '\n';
            }


            std::cout << "\n====================================\nsimplifted min term are:" << '\n';
            for(int x=0;x<primes.size();x++){
              std::cout << "mask: "<<std::bitset<bitsize>(primes[x].mask)<<" minterm: " <<std::bitset<bitsize>(primes[x].minterm)<< '\n';
            }
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
    }else{
            bool fromfvalues=true;
            std::string PI;

            std::cout << "is it from fvalues? 1 or 0" << '\n';
            std::cin >> PI;
            if(PI=="0"){
              fromfvalues=false;
            }
            PI="";

            std::cout << "debug file, enter prime implicant" << '\n';
            std::cin >> PI;
            while (PI!="c"){
              appedPrime(PI,primes,fromfvalues);
              PI="";
              std::cin >> PI;
            };
            std::cout << "/* message */" << '\n';
    }



    //make tree
    bdt rootpt = newnode();
    recTreeConstructor(rootpt,primes,(1L<<fvalues[0].size())-1);

    return rootpt;
}

void recTreeConstructor(bdnode* node,std::vector<implicant>& primes, term nodeRemains){//nodeRemains => 0 for not used node, 1 for used node
  if(primes.size()==0){
    //check if case for 0, empty primes
    node->val="0";
    return;
  }else{
    int maskSum;

    //check if case for 1, one prime with --------
    int maxMaskCount=popcount(nodeRemains);
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
    int counter=popcount(nodeRemains);
    int digit=0;
    while (counter>0) {
      if((1L<<digit)&nodeRemains){
        counter--;
        //node that is not in the tree, chooice between these
        // std::cout << "digit"<<digit<<": "<<cnt.get(digit) << '\n';
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
    recTreeConstructor(node->left,leftPrimes, nodeRemains& (~optimalMask));

    node->right=newnode();
    recTreeConstructor(node->right,rightPrimes, nodeRemains& (~optimalMask));
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
void appedPrime(std::string str, std::vector<implicant>& primes, bool fval){
  term mask=0;
  term minterm=0;
  // term printmask=1;
  if(fval){
    for(int i=0;i<str.length();i++){
      if(int(str[i])==45){
        mask=mask|(1L<<i);
      }else if(int(str[i]==49)){
        minterm=minterm|(1L<<i);
      }
    }
  }else{
    for(int i=0;i<str.length();i++){
      if(int(str[i])==45){
        mask=mask|(1L<<(str.length()-i-1));
      }else if(int(str[i]==49)){
        minterm=minterm|(1L<<(str.length()-i-1));
      }
    }
  }

  primes.push_back((implicant){mask,minterm});
  printPrime(mask,minterm,bitsize);
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

  std::string getMinterm(term i){
    long printmask=1;
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
    std::cout << "from Xn <-- X1: ";
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

  void genInput(std::vector<term>& in,int digit,std::vector<std::string>& out){
    term printmask=1;
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

  long getAss1NodeNum(){
    return (1L<<bitsize+1)-1;
  }

  void deltree(bdt t){
    if(t!=NULL){
      deltree(t->left);
      deltree(t->right);
      delete t;
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
