#include "src/include/int2048.h"
#include <iostream>

int main() {
    // Test floor division
    sjtu::int2048 a, b;
    
    // 10 / 3 = 3
    a = sjtu::int2048(10);
    b = sjtu::int2048(3);
    std::cout << a / b << std::endl;
    
    // -10 / 3 = -4 (floor division)
    a = sjtu::int2048(-10);
    b = sjtu::int2048(3);
    std::cout << a / b << std::endl;
    
    // 10 / -3 = -4 (floor division)
    a = sjtu::int2048(10);
    b = sjtu::int2048(-3);
    std::cout << a / b << std::endl;
    
    // -10 / -3 = 3
    a = sjtu::int2048(-10);
    b = sjtu::int2048(-3);
    std::cout << a / b << std::endl;
    
    // Test modulo
    // 10 % 3 = 1
    a = sjtu::int2048(10);
    b = sjtu::int2048(3);
    std::cout << a % b << std::endl;
    
    // -10 % 3 = 2 (because -10 - (-4) * 3 = -10 + 12 = 2)
    a = sjtu::int2048(-10);
    b = sjtu::int2048(3);
    std::cout << a % b << std::endl;
    
    return 0;
}

