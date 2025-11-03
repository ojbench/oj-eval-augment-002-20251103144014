#include "src/include/int2048.h"
#include <iostream>

int main() {
    sjtu::int2048 a;
    a.print(); 
    std::cout << std::endl;

    sjtu::int2048 b((long long)1145141919810);
    b.print(); 
    std::cout << std::endl;
    
    sjtu::int2048 c((long long)-1145141919810);
    c.print(); 
    std::cout << std::endl;

    sjtu::int2048 d("-2333333333333333333333333333333333333333333333333333333");
    d.print(); 
    std::cout << std::endl;

    sjtu::int2048 e("19260817192608171926081719260817192608171926081719260817");
    e.print(); 
    std::cout << std::endl;
    
    return 0;
}

