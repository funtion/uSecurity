#this is a introduction of API


----------
##MD5
```c++
#include <iostream>
#include "md5.h"

using std::cout; using std::endl;
 
int main(int argc, char *argv[])
{
    cout << "md5 of 'grape': " << md5("grape") << endl;
    return 0;
}
```

----------
##sha256/512
```c++
#include <iostream>
#include "sha256.h"
#include "sha512.h"
 
using namespace std;
 
int main(int argc, char *argv[])
{
    string input = "grape";
    string output1 = sha256(input);
    string output2 = sha512(input);
    cout << "sha256('"<< input << "'):" << output1 << endl;
    cout << "sha512('"<< input << "'):" << output2 << endl;
    return 0;
}
```
------------
   
