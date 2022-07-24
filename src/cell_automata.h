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

#define     UP              111  // up arrow
#define     DOWN            116  // down arrow
#define     LEFT            113  // left arrow
#define     RIGHT           114  // right arrow
#define     ESC             9    // esc
#define     SPACE           65   // space
#define     SCAN            39   // s
#define     COMPRESS        54   // c
#define     MOBILE16B       57   // n
#define     MOBILE32B       58   // m
#define     ELEMENTARY      26   // e
#define     TOTALISTIC      28   // t
#define     GENERALIZED     42   // g
#define     SPEC_GENERALIZD 43   // h

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
            bool is_Generalize_Mobile = false;
            bool compressed = false;
            unsigned int current_compressed;
            int most_left;
            int most_right;
};

struct groupColor1{  // triad
    unsigned long teal = 0x008080;
    unsigned long magenta = 0xff00ff;
    unsigned long gold = 0xffd700;
};

struct groupColor2{  // triad
    unsigned long green = 0x008000;
    unsigned long orange = 0xffa500;
    unsigned long purple = 0x6a0dad;
};

struct groupColor3{   // complementary
    unsigned long toneRed = 0xd64a80;
    unsigned long toneGreen = 0x80d64a;
};

struct groupColor4{
    unsigned long toneBlue = 0x5229d9;
    unsigned long toneOrange = 0xd99529;
};

struct groupColor5{
    unsigned long toneYellow = 0xffeb0f;
    unsigned long tonePurple = 0xd70fff;
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
        unsigned long colorYellow = 0xffff8f;     //yellow
        groupColor1 color1;  // triad
        groupColor2 color2;   // triad
        groupColor3 color3;   // complementary (2 colors)
        groupColor4 color4;   // complementary
        groupColor5 color5;   // complementary


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