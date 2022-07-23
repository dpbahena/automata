#if __INTELLISENSE__
#undef __ARM_NEON
#undef __ARM_NEON__
#endif

#ifndef __CELL_AUTOMATA_H__
#define __CELL_AUTOMATA_H__

#include <Eigen/Core>
#include <Eigen/Dense>
#include <X11/Xlib.h>
#include <sstream>


using namespace Eigen;
#define     PHYSICAL_WIDTH      1857  //1920   remove 63 dot-resolution from the left margen dedicated to ubuntu menu
#define     PHYSICAL_HEIGHT     1057  // 1080  remove 23 dot-resolution (height) the top.. which is used by the program vsc

#define     UP          111
#define     DOWN        116
#define     LEFT        113
#define     RIGHT       114
#define     ESC           9
#define     SPACE        65
#define     SCAN         39
#define     COMPRESS     54
#define     MOBILE16B    57
#define     MOBILE32B    58
#define     ELEMENTARY   26
#define     TOTALISTIC   28
#define     GENERALIZED  42

struct stats{
            float matrix_sum;
            float matrix_mean;
            float matrix_prod;
            float matrix_minCoef;
            float matrix_maxCoef;
            bool is_8bit = false;
            bool is_ternary = false;
            bool is_mobile = false;  //bool is_mobile_16bit = false;
            bool is_16bit = false;
            bool is_Generalize_Mobile = true;
            bool compressed = false;
            unsigned int current_compressed;
            int most_left;
            int most_right;
};


namespace nature{
    
    class DisplayMatrix{
    public:
        DisplayMatrix(int width, int height, unsigned int rule, int factor);
        ~DisplayMatrix();

        unsigned int rule;
       
        int dot_size = 15;
        int dot_size_min = 1;
        
        int dot_size_max;
        int start_row = 0;
        int start_column = 0;
        int scan_columns = 0;
        int scan_factor = 2;

        bool scan_mode = false;
        int matrix_rows_per_display;
        int matrix_columns_per_display;
        bool left_limit_reached = false;
        bool right_limit_reached = false;
        
        stats pattern;

        int get_Width();
        int get_Height();
        Display *get_DisplayPtr();
        void mapMatrix();
        int user_input_rule();
        // colors of the dots
        unsigned long colorBlack = 0x000000; // black; 
        unsigned long colorWhite = 0xffffff; // white
        unsigned long colorGrey = 0x808080; // gray
        unsigned long colorRed = 0xff0000; // red
        void drawRect(unsigned long color, int x, int y, int width, int height);
        void drawDot(unsigned long color, int x, int y, int radius); // draw small circle
        void reDraw();

        
    
    private:

        int width;
        int height;
        //int last_scan_column = 0;
        int matrix_factor;
        
        
        
        void drawText();
        
        ArrayXXf matrix;
        bool scan_matrix_ready = false;

        
        // Graphics Display
        Display *display_;
        int screen_;   // default screen handle;
        Window window_;

        void init_display();
        
    };

    // MakeMatrix ------------------------------>
    class MatrixPattern{
        public:
            
            MatrixPattern(ArrayXXf &matrix, unsigned int rule, stats& pattern); 
            

        private:
        
            int rows;
            int cols;
           
            std::string base3;
            std::stringstream ss;
            std::string bin32;
            //char bin32[31]; //  = "00000000000000000000000000000000";
            void getShape(ArrayXXf &matrix, stats &pattern);   
            std::string dec_to_32bit();    
           
            void decimal_Base3(int N);
            unsigned int rule;

        

    

    };

    // MakePatterns -------------------------->
    class MakePatterns{
    public:
        MakePatterns(int w, int h, unsigned int rule, int factor);
        void run();
        

    private:
        int width, height;
        unsigned int rule;
        int factor;
        bool compressed;
        unsigned int ruleStart = 0;
        unsigned int ruleElemEnds = 255;
        unsigned int ruleTotStarts = 256;
        unsigned int ruleTotEnds = 2186;
        unsigned int rule16bstarts = 2187;
        unsigned int rule16bends = 65535;
        unsigned int ruleEnd = 4294967295;
        
        DisplayMatrix dp_ = DisplayMatrix(width, height, rule, factor);
        
        
        XEvent event_;   // create an extructure that has all events
        bool is_running_ = true;
        
        
        void handle_event();

    };



    

}






#endif