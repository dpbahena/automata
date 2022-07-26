#include <iostream>
#include "cell_automata.h"
#include <stdlib.h>

void shifting(unsigned int dec);
int main(int args, char **argv){
    int width, height, factor;
    unsigned int rule;
    bool generalized;
    char * end;
    
    if(args == 5){
        width = atoi(argv[1]);
        height = atoi(argv[2]);
        rule = strtoul(argv[3],&end, 10 );
        factor = atoi(argv[4]);
        
        
        
        
    }else{
        std::cout << "Missing Parameters. Usage: ./cellautomata <width> <height> <rule> <factor> <generalized>-- Max width = 1857   Max height = 1057  rule range 0 - 4294967295 factor 2 - 40 "  << std::endl;
        std::cout << "************************Using default values :   ./cellautomata 1857 1057 1599 6 " << std::endl;
        width = 1857;
        height = 1057;
        rule = 1599;
        factor = 6;
        
    }

    
       
    nature::MakePatterns makepatterns(width, height, rule, factor); 
    makepatterns.run();
        
    return 0; 
}

void shifting(unsigned int dec){
   


    //unsigned int dec = 750000000;  //4 bytes
    char bin32[]  = "00000000000000000000000000000000";
    for (int pos = 31; pos >= 0; --pos)
    {
        if (dec % 2){ 
            //std::cout << "1" << std::endl;
            bin32[pos] = '1';
        }
        dec /= 2;
    }

    std::cout << "The binary of the given number is: " << bin32 << std::endl;


    

}




    
    



