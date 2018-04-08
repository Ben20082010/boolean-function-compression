//C++11

#include <iostream>
#include <string>
#include <vector>
/// include here all the headers you need (but see gbinarytreeuidelines)
#include <bitset>
#include <math.h>
#include <stdlib.h>


struct bdnode{
    std::string val;
    bdnode* left;   //0 branch
    bdnode* right;  //1 branch

};

/// do not alter the typedef
typedef bdnode* bdt;

/// do not alter these two function declarations
bdt buildbdt(const std::vector<std::string>& fvalues);
std::string evalbdt(bdt t, const std::string& input);
bool is1(char c);
std::string evalcompactbdt(bdt t, const std::string& input);


bool isleft(char c);
bdt newnode(std::string val, bdnode* left=NULL, bdnode* right=NULL );
bdt constTreeWithOne(const std::vector<std::string>& fvalues,int depth);
void fillTreeWithZero(bdt& nodept, int height,int lable);

void printTreeRec(bdt t, int depth,std::vector<std::string>& out){
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
std::cout << "x" << '\n';

}


/// you can define and use additional functions
/// for the implementation of buildbdt and evalbdt

/// add here the declarations (not the implementation)
/// for any other functions you wish to define and use

/// before the submission you need to remove the main
/// (either delete it or comment it out)
/// otherwise it will intefere with the automated testing

int main(){

    /// write the code for the main here in order to test your functions
    std::vector<std::string> input;
    std::string in = "1001";
    input.push_back(in);
    in = "1101";
    input.push_back(in);
    in = "1111";
    input.push_back(in);
    int l=input[0].length();
    std::cout << "building..." << '\n';
    bdt fbdt = buildbdt(input);


    // printTree(fbdt,input[0].length()-1);
    printTree(fbdt,l+1);

    std::cout << evalbdt(fbdt,"1001")<<" | "<<evalcompactbdt(fbdt,"1001") << '\n';
    std::cout << evalbdt(fbdt,"0000")<<" | "<<evalcompactbdt(fbdt,"0000") << '\n';
    std::cout << evalbdt(fbdt,"1101")<<" | "<<evalcompactbdt(fbdt,"1101") << '\n';
    std::cout << evalbdt(fbdt,"1111")<<" | "<<evalcompactbdt(fbdt,"1111") << '\n';


    // for (int i=0;i<pow(2,l+1)-1;i++){
    //   std::cout << evalbdt(fbdt,std::bitset<l>i.to_string()) << '\n';
    // }


    return 0;
}

bdt constTreeWithOne(const std::vector<std::string>& fvalues, int depth){


  bdt rootpt = newnode("x1");
  bdt pt;

  for(int i=0;i<fvalues.size();i++){
    //iterate all the strings
    pt=rootpt;
    std::cout << "string proceeing is "<<fvalues[i] << '\n';

    for(int d=0;d<depth-1;d++){
      //implement tree for the current string,but not the leaft node
      if(isleft(fvalues[i][d])){
        if(pt->left==NULL){
          pt->left = newnode("x"+std::to_string(d+2));
        }
        pt=pt->left;

      }else{
        if(pt->right==NULL){
          pt->right = newnode("x"+std::to_string(d+2));
        }
        pt=pt->right;
      }
    }

    // leaft node of the current string
    // std::cout << pt->val << '\n';
    if(isleft(fvalues[i][depth-1])){
      pt->left = newnode("1");
    }else{
      pt->right = newnode("1");
    }
  }

  return rootpt;
}

void fillTreeWithZero(bdt& nodept, const int height, const int lable){
  if(height>1){
    if(nodept->left==NULL){
      nodept->left=newnode("x"+std::to_string(lable));
    }
      fillTreeWithZero(nodept->left,height-1,lable+1);

    if(nodept->right==NULL){
      nodept->right=newnode("x"+std::to_string(lable));
    }
      fillTreeWithZero(nodept->right,height-1,lable+1);
  }else{
    //for the last level, base case
    // std::cout << nodept->val << '\n';
    if(nodept->left==NULL){
      nodept->left=newnode("0");
    }
    if(nodept->right==NULL){
      nodept->right=newnode("0");
    }
  }
}

bdt buildbdt(const std::vector<std::string>& fvalues){
  const int depth=fvalues[0].length();
  bdt rootpt;
  rootpt=constTreeWithOne(fvalues,depth);

  fillTreeWithZero(rootpt,depth,2);

  return rootpt;
}

std::string evalbdt(bdt t, const std::string& input){
  for(int i=0;i<input.length();i++){
    // std::cout << t->val << '\n';
    if(isleft(input[i])){
      t=t->left;
    }else{
      t=t->right;
    }
  }
  return t->val;
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
bool isleft(char c){
  if(int(c)==48){
  //check if the char is "0" (left)
    return true;
  }else{
  // treat not "0" as 1 (right)
    return false;
  }
}

bdt newnode(std::string val, bdnode* left, bdnode* right){
  bdt pt=new bdnode;
  pt->val=val;
  pt->left=left;
  pt->right=right;
  return pt;
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
