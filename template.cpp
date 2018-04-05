/// include here all the headers you need (but see guidelines)

/// do not use using namespace std

/// do not alter the struct declaration
struct bdnode{
    std::string val;
    bdnode* left;
    bdnode* right;

};

/// do not alter the typedef
typedef bdnode* bdt;

/// do not alter these two function declarations
bdt buildcompactbdt(const std::vector<std::string>& fvalues);
std::string evalcompactbdt(bdt t, const std::string& input);

/// you can define and use additional functions
/// for the implementation of buildcompactbdt and evalcompactbdt

/// add here the declarations (not the implementation)
/// for any other functions you wish to define and use

/// before the submission you need to remove the main
/// (either delete it or comment it out)
/// otherwise it will intefere with the automated testing

int main(){

    /// write the code for the main here in order to test your functions

    return 0;
}

bdt buildcompactbdt(const std::vector<std::string>& fvalues){
    /// write the implementation for the function here
}

std::string evalcompactbdt(bdt t, const std::string& input){
    /// write the implementation for the function here
}

/// add here the implementation for any other functions you wish to define and use
