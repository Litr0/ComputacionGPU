#include <iostream>
#include "Matrix.h"

int main(int argc, char *argv[]) {
    Matrix mat(3, 2); 
    mat[0, 1] = 3;    
    mat[1, 1] = 2;    
    mat[2, 0] = 1;    
    
    std::cout << mat; 
    
    return 0;
}