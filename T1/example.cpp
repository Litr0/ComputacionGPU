#include <iostream>
#include "Matrix.h"

int main(int argc, char *argv[]) {
    Matrix mat(3, 2); 
    mat[0, 1] = 3.5;
    mat[1, 1] = 2.25;
    mat[2, 0] = 1.0;

    std::cout << mat; 
    
    return 0;
}