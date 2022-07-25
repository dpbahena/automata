#include "cell_automata.h"
#include <iostream>
#include <stdexcept>


//using namespace std;


nature::DisplayMatrix::DisplayMatrix(int width, int height, unsigned int rule, int factor) : width(width), height(height), rule(rule), matrix_factor(factor) {
    //this->width = width;
    //this->height = height;
    dot_size_max = height *.29;
    
    init_display();
    //std::cout << "after INIT _ Display" << std::endl;
}


nature::DisplayMatrix::~DisplayMatrix(){
    std::cout << "closing display" << std::endl;
    XCloseDisplay(display_);  // close the display
}

void nature::DisplayMatrix::init_display(){

    display_ = XOpenDisplay(NULL); 
    if(!display_){
        std::runtime_error("Error opening display \n");
    }
    
    screen_ = DefaultScreen(display_);
    int x = width; //PHYSICAL_WIDTH;
    int y = height; //PHYSICAL_HEIGHT;
    window_ =  XCreateSimpleWindow(display_, RootWindow(display_,screen_), 0, 0, x, y, 1, BlackPixel(display_,screen_), WhitePixel(display_,screen_));
    //cout << "after opening windows"<< "rows: " << rows << " and columns " << cols << endl;
    XMapWindow(display_,window_);
    
    XSelectInput(display_, window_, KeyPressMask | ExposureMask);   // capture keyboard and exposure events  (exposure : allow to draw on window)
    
    
}

int nature::DisplayMatrix::get_Width(){
    return width;
}

int nature::DisplayMatrix::get_Height(){
    return height;
}

Display * nature::DisplayMatrix::get_DisplayPtr(){
    return display_;
}

void nature::DisplayMatrix::mapMatrix(){
    int offset = dot_size;
    
    
    if(dot_size < dot_size_min){   // make sure we have a dot
        dot_size = dot_size_max; // start over with big dots 
        offset = dot_size;  // reset offset
    }
    if(dot_size > dot_size_max){   // make sure the dot is not too big
        dot_size = dot_size_min;
        offset = dot_size;  // reset offset, as well
        
    }
    std::cout << "max dot size : " << dot_size_max << std::endl;
    if (rule < 0){
        rule = 0;    // do not move
    }else if(rule >= 0 && rule <= 255){
        pattern.is_8bit = true;
    }else if(rule >=256 && rule <=2186){
        pattern.is_ternary = true;
    }else if (rule >= 2187 && rule <= 4294967295/*65535*/){
        pattern.is_mobile = true;
    }


    
    
    int skip = 0;

    if(scan_mode || pattern.compressed){
        if(!scan_matrix_ready){
            int width_factor;
            if(pattern.compressed){
                matrix_factor = 60; //60; // DEFAULT so a graph can be compressed 60 times the height of the screen
                
                width_factor = 1;   // Ideal for compression .. increase efficiency. The width stays the same. Width is not needed for compression
                                    // in compression, the height multiplied by matrix factor is what is needed to get a good compression
            }else{
                width_factor = matrix_factor;   // otherwise keep both factors the same for SCAN option
            }

            std::cout << "matrix factor " << matrix_factor << std::endl;
            matrix = ArrayXXf::Zero((int)(height/dot_size) * matrix_factor, (int)(width/dot_size) * width_factor);
            MatrixPattern mp_ = MatrixPattern(matrix, rule, pattern);
            scan_matrix_ready = true;
        }
    }else{

        matrix = ArrayXXf::Zero((int)(height/dot_size), (int)(width/dot_size));            
        MatrixPattern mp_ = MatrixPattern(matrix, rule, pattern);
        scan_matrix_ready = false;
    }
   
    matrix_rows_per_display = height/dot_size;
    matrix_columns_per_display = width/dot_size;
    //std::cout << "Dot_size : " << dot_size << std::endl;
    //std::cout << "Matrix rows per display : " << matrix_rows_per_display << std::endl;
    //std::cout << "Matrix columns per display : " << matrix_columns_per_display << std::endl;
    std::cout << "RULE: " << rule << std::endl;
    std::cout << "Display width  is : " << width << " - height is : " << height << std::endl;
    std::cout << "matrix: [" << matrix.rows() << ", " << matrix.cols() << "]" << std::endl;
    int rowStep = start_row + height/dot_size;
    
     /* assume that the matrix has 12 rows.. if start_row is 10 and the screen size is 8  and dot_size is 2..then 8/2 = 4
     *  the screen can accomodate 4 more rows then 10 + 4 is more than the number of total rows in the matrix
     *  To adjust the start_row:  step = start_row + height/dot_size  10 + 4 = 14   and 14 > 12!! will create out-of-range
     *  Take the difference  matrix.rows() - step   or 12 - 14 = -2  then add this difference from start_row.
     *  start_row = start_row + diff  or  10 + -2 = 8  
    *
    */
    if(start_row < 0){
        start_row = 0;
    }
    else if(rowStep > matrix.rows() ){    
        start_row = start_row + (matrix.rows() - rowStep); // Moves downs up the the limit 
    }
    
    
    if (scan_mode){

        start_column =  matrix.cols()/2 - width/(2*dot_size) + scan_columns;
    }else{
        start_column =  matrix.cols()/2 - width/(2*dot_size);
    }
    
    int colStep = start_column + width/dot_size;
    //std::cout << "colStrep : " << colStep << std::endl;

    
    //std::cout << "Start column before adjustment: " << start_column << std::endl;
    
    
    
    //std::cout << "scan columns : " << scan_columns << std::endl;
    if (start_column < 0){
        start_column = 0;
        
        left_limit_reached = true;
        
        
    }else if(colStep > matrix.cols()){
        start_column = start_column + (matrix.cols() - colStep);
        right_limit_reached = true;
        

    }
    

    //std::cout << "Start column after ajustment if needed " << start_column << std::endl;
    //std::cout << "scan columsn after : " << scan_columns << std::endl;
    //std::cout << "Matrix starting row : " << start_row << " - Starting column : " << start_column << std::endl;
    
    int indexi=start_row, indexj = start_column; // start_column;
    if(pattern.is_8bit){
        for(int i = 0; i < height - offset;  i = i + dot_size, indexi++){    //iterates every row 

            for(int j = 0; j < width - offset; j = j + dot_size, indexj++){    // iterates every column
                
                if(matrix(indexi,indexj)==0){
                    //std::cout << "at matrix[" << indexi << ", " << indexj <<"] = " << matrix(indexi, indexj) << "the i is : " << i << "and : j "<< j << std::endl;
                    drawRect(color5.toneYellow,j,i,dot_size,dot_size);
                }else {
                    drawRect(color5.tonePurple,j,i,dot_size,dot_size);
                }
            }
            indexj=start_column;
        }
        //cout << "matrix rows : " <<  matrix.rows() << "cols: " << matrix.cols() << endl;
        //cout << "indexi rows : " << indexi << "indexj cols : " << indexj << endl;
        //cout << "displayrows : " << rows <<   "cols : " << cols << endl;
        drawText();
        pattern.is_8bit = false;  // once is done

    }else if(pattern.is_ternary){
        for(int i = 0; i < height - offset;  i = i + dot_size, indexi++){    //iterates every row 

            for(int j = 0; j < width - offset; j = j + dot_size, indexj++){    // iterates every column
                
                if(matrix(indexi,indexj)==0){
                    //std::cout << "at matrix[" << indexi << ", " << indexj <<"] = " << matrix(indexi, indexj) << "the i is : " << i << "and : j "<< j << std::endl;
                    drawRect(color2.purple,j,i,dot_size,dot_size);
                }else if (matrix(indexi,indexj)==1){
                    //std::cout << "at matrix[" << indexi << ", " << indexj <<"] = " << matrix(indexi, indexj) << "the j is : " << j << std::endl;
                    drawRect(color2.green,j,i,dot_size,dot_size);
                }else {
                    drawRect(color2.orange,j,i,dot_size,dot_size);
                }
            }
            indexj=start_column;
        }
        //cout << "matrix rows : " <<  matrix.rows() << "cols: " << matrix.cols() << endl;
        //cout << "indexi rows : " << indexi << "indexj cols : " << indexj << endl;
        //cout << "displayrows : " << rows <<   "cols : " << cols << endl;
        drawText();
        pattern.is_ternary = false;
    }else if(pattern.is_mobile && !pattern.compressed){      // mobile_16 bits  
        for(int i = 0; i < height - offset;  i = i + dot_size, indexi++){    //iterates every row 

            for(int j = 0; j < width - offset; j = j + dot_size, indexj++){    // iterates every column
                
                if(matrix(indexi,indexj)==0){
                    //std::cout << "at matrix[" << indexi << ", " << indexj <<"] = " << matrix(indexi, indexj) << "the i is : " << i << "and : j "<< j << std::endl;
                    drawRect(colorWhite,j,i,dot_size,dot_size);
                }else if (matrix(indexi,indexj)==1){
                    //std::cout << "at matrix[" << indexi << ", " << indexj <<"] = " << matrix(indexi, indexj) << "the j is : " << j << std::endl;
                    drawRect(colorBlack,j,i,dot_size,dot_size);
                }else if (matrix(indexi, indexj) == 2) { // draw white cell with a dot 
                    drawRect(colorWhite,j,i,dot_size,dot_size);
                    drawDot(colorRed,j,i, dot_size);

                }else if (matrix(indexi, indexj) == 3){  // draw black cell with a dot
                    drawRect(colorBlack,j,i,dot_size,dot_size);
                    drawDot(colorRed,j,i, dot_size);
                }
            }
            
            indexj=start_column;
        }
        //cout << "matrix rows : " <<  matrix.rows() << "cols: " << matrix.cols() << endl;
        //cout << "indexi rows : " << indexi << "indexj cols : " << indexj << endl;
        //cout << "displayrows : " << rows <<   "cols : " << cols << endl;
        drawText();
        pattern.is_mobile = false;

    }else if(pattern.is_mobile && pattern.compressed){
        for(int i = 0; i < height - offset;  i = i + dot_size, indexi++){    //iterates every row 
            bool skip = false;
            for(int j = 0; j < width - offset; j = j + dot_size, indexj++){    // iterates every column
                
                if(matrix(indexi,indexj)==0){
                    //std::cout << "at matrix[" << indexi << ", " << indexj <<"] = " << matrix(indexi, indexj) << "the i is : " << i << "and : j "<< j << std::endl;
                    drawRect(colorWhite,j,i,dot_size,dot_size);
                }else if (matrix(indexi,indexj)==1){
                    //std::cout << "at matrix[" << indexi << ", " << indexj <<"] = " << matrix(indexi, indexj) << "the j is : " << j << std::endl;
                    drawRect(colorBlack,j,i,dot_size,dot_size);
                }else if (matrix(indexi, indexj) == 2) { // draw white cell with a dot 
                    if(indexj >= pattern.most_right){
                        drawRect(colorWhite,j,i,dot_size,dot_size);
                        drawDot(colorRed,j,i, dot_size);
                        pattern.most_right = indexj;
                    }else if (indexj < pattern.most_left){
                        drawRect(colorWhite,j,i,dot_size,dot_size);
                        drawDot(colorRed,j,i, dot_size);
                        pattern.most_left = indexj;  
                    }else{
                        skip = true;
                    }

                }else if (matrix(indexi, indexj) == 3){  // draw black cell with a dot
                    skip = false;
                    if(indexj > pattern.most_right){
                        drawRect(colorBlack,j,i,dot_size,dot_size);
                        drawDot(colorRed,j,i, dot_size);
                        pattern.most_right = indexj;
                    }else if(indexj < pattern.most_left){
                        drawRect(colorBlack,j,i,dot_size,dot_size);
                        drawDot(colorRed,j,i, dot_size);
                        pattern.most_left = indexj;    
                    }else{
                        skip = true;
                    }
                }
            }
            indexj=start_column;
            if(skip) {
                skip = false; 
                i = i - dot_size;
                //std::cout << "i : " << i << std::endl;

                }
            if (indexi >= matrix.rows())
                break;
            //if(skip) { skip = false; indexi = indexi - 1;}
        }
        //cout << "matrix rows : " <<  matrix.rows() << "cols: " << matrix.cols() << endl;
        //cout << "indexi rows : " << indexi << "indexj cols : " << indexj << endl;
        //cout << "displayrows : " << rows <<   "cols : " << cols << endl;
        drawText();
        pattern.is_mobile = false;
        //pattern.compressed = false;      
    }
}



void nature::DisplayMatrix::drawRect(unsigned long color, int x, int y, int width, int height){
    XSetForeground(display_, DefaultGC(display_, screen_), color);
    XFillRectangle(display_, window_, DefaultGC(display_, screen_), x, y, width, height);
       

}
void nature::DisplayMatrix::drawDot(unsigned long color, int x, int y, int radius){
    float factor = 0.3;
    int r = radius * factor;

    XSetForeground(display_, DefaultGC(display_, screen_), color);
    XFillArc(display_, window_, DefaultGC(display_,screen_), x+r/2, y-1+r/2, radius-r, radius-r, 0, 360*64 );
}

void nature::DisplayMatrix::drawText(){
    
    //auto theFont    = XLoadFont(display_, "10x20" );   // the font are hard coded for info go to: https://www.oreilly.com/library/view/x-window-system/9780937175149/Chapter05.html
    //XSetFont(display_, DefaultGC(display_,screen_), theFont);     // use alias names only. xlsfonts in terminal for font lists. More info:  https://www.math.utah.edu/~beebe/fonts/X-Window-System-fonts.html
    XSetForeground(display_, DefaultGC(display_, screen_), 0x000000);
    float tab_factor = 0.10f;   // info position on left side
    float tab_factor_2 = 6.5f;  // info position on right side

    std::stringstream ss;
    ss << "Rule # " << rule; 
    std::string str = ss.str();
    XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,20, str.c_str(), str.size());
    
    ss.str(std::string());  // clears buffer
    ss << "Dot Size : " << dot_size; 
    str = ss.str();
    XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,40, str.c_str(), str.size());
    
    ss.str(std::string());  // clears buffer
    
    
    if(scan_mode){
        
        ss << "Scan Mode ON   -   Toggle 'S' for ON/OFF";
        str = ss.str();
        XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,60, str.c_str(), str.size());
        ss.str(std::string());  // clears buffer
        ss << "UP, DOWN, LEFT, RIGHT arrows to SCAN around the shape";
        str = ss.str();
        XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,80, str.c_str(), str.size());
        ss.str(std::string());  // clears buffer
        ss << "For deeper scan, increase matrix factor (default: 6)";
        str = ss.str();
        XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,100, str.c_str(), str.size());
        ss.str(std::string());  // clears buffer
        ss << "./cellautomata <width> <height> <rule> <factor> ";
        str = ss.str();
        XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,120, str.c_str(), str.size());
        ss.str(std::string());  // clears buffer
        ss << "Example: ./cellautomata 1800 1000 1599 25 ";
        str = ss.str();
        XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,140, str.c_str(), str.size());

    }else if (!scan_mode && !pattern.compressed){
        ss << "Toggle 'S' for Scan Mode ON/OFF   ---- Toggle 'C' to ON/OFF compressed mode (Mobile16bit - Mobile32bit only)";
        str = ss.str();
        XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,60, str.c_str(), str.size());
        ss.str(std::string());  // clears buffer

        ss << "Toggle random: 'E' ELEMENTARY  - 'T' TOTALISTIC -  'N' Mobile 16bit  'M' MOBILE32bit -  'G' Generalized Mobile";
        str = ss.str();
        XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,80, str.c_str(), str.size());
        ss.str(std::string());  // clears buffer

        ss << "Press 'H' to Enter SPECIFIC GENERALIZE RULE #";
        str = ss.str();
        XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,100, str.c_str(), str.size());
        ss.str(std::string());  // clears buffer

        ss << "UP, DOWN: Change resolution.";
        str = ss.str();
        XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,120, str.c_str(), str.size());
        ss.str(std::string());  // clears buffer
        ss << "LEFT, RIGHT: change the shape.";
        str = ss.str();
        XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,140, str.c_str(), str.size());
        ss.str(std::string());  // clears buffer
        ss << "SPACE : jump to another shape (go to Terminal)";
        str = ss.str();
        XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,160, str.c_str(), str.size());

    }
    
   
    
    // compressed mode
    if(pattern.compressed){

        ss.str(std::string());  // clears buffer
        ss << "All operations are DISABLE in COMPRESSED mode!!!";
        str = ss.str();
        XDrawString(display_, window_, DefaultGC(display_,screen_), width * tab_factor, 80, str.c_str(), str.size());


        ss.str(std::string()); // clears buffer
        ss << "For HIGH resolution (dot_size 1), use maximum display settings for width and height of your computer for BETTER results";
        str = ss.str();
        XDrawString(display_, window_, DefaultGC(display_,screen_), width * tab_factor, 100, str.c_str(), str.size());

        ss.str(std::string()); // clears buffer
        ss << "Toggle 'C' to exit Compressed mode";
        str = ss.str();
        XDrawString(display_, window_, DefaultGC(display_,screen_), width * tab_factor, 120, str.c_str(), str.size());
        

    }

    
    
    // ESC 
    ss.str(std::string());  // clears buffer
    ss << "ESC to exit";
    str = ss.str();
    XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor,180, str.c_str(), str.size());

    // pattern statistics
    ss.str(std::string());  // clears buffer
    ss <<   "matrix.sum():       " << pattern.matrix_sum;
    str = ss.str();
    XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor * tab_factor_2,20, str.c_str(), str.size());
    
    ss.str(std::string());  // clears buffer
    ss <<   "matrix.prod():      " <<  pattern.matrix_prod;
    str = ss.str();
    XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor * tab_factor_2,40, str.c_str(), str.size());
    
    ss.str(std::string());  // clears buffer
    ss <<   "matrix.mean():      " << pattern.matrix_mean;
    str = ss.str();
    XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor * tab_factor_2,60, str.c_str(), str.size());
    
    ss.str(std::string());  // clears buffer
    ss <<   "matrix.minCoeff():  " << pattern.matrix_minCoef;
    str = ss.str();
    XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor * tab_factor_2,80, str.c_str(), str.size());
    
    ss.str(std::string());  // clears buffer
    ss <<   "matrix.maxCoeff():  " << pattern.matrix_maxCoef;
    str = ss.str();
    XDrawString(display_,window_,DefaultGC(display_, screen_),width * tab_factor * tab_factor_2,100, str.c_str(), str.size());

    
}

void nature::DisplayMatrix::reDraw(){
    
    if(!scan_mode){
        XClearWindow(display_, window_);
        std::cout << "borrando el display..." << std::endl;
    }
    Window root_window;
    int x, y;
    unsigned int width, height;
    unsigned int border_width, depth;

    XGetGeometry(display_, window_, &root_window, &x, &y, &width, &height, &border_width, &depth );
    

    XEvent ev;   /// create an event

    ev.xexpose.type = Expose;
    ev.xexpose.display = display_;
    ev.xexpose.window = window_;
    ev.xexpose.x = x;
    ev.xexpose.y = y;
    ev.xexpose.width = width;
    ev.xexpose.height = height;
    ev.xexpose.count = 0;

    XSendEvent(display_, window_, false, ExposureMask, &ev);

}

int nature::DisplayMatrix::user_input_rule(){
    
    unsigned int input_rule;
    if(pattern.is_Generalize_Mobile){
        std::cout << "        GENERALIZED MOBILES " << std::endl;
        std::cout << "Hardcoded rules to make your enter these rules easier: " << std::endl;
        std::cout << "0: Rule # 267718719" << std::endl;
        std::cout << "1: Rule # 2756018441" << std::endl;
        std::cout << "2: Rule # 3378589909" << std::endl;
        std::cout << "3: Rule # 4096999591" << std::endl;
        std::cout << "4: Rule # 507100781" << std::endl;
        std::cout << "5: Rule # 3635480489" << std::endl;
        std::cout << "6: Rule # 2178553569" << std::endl;
        std::cout << "7: Rule # 1369713987" << std::endl;
        std::cout << "8: Rule # 2174551651" << std::endl;
        std::cout << "9: Rule # 2174551975" << std::endl;
        std::cout << "10: Rule # 507100823  -- weird" << std::endl;
        std::cout << "11: Rule # 507100809" << std::endl;
        std::cout << "12: Rule # 507100605" << std::endl;
        std::cout << "13: Rule # 507100311  -- weird" << std::endl;
        std::cout << "Or enter the full rule number yourself below ..." << std::endl;
       
    
        
        std::cout << " Enter new rule 0 -  :";
        std::cin >> input_rule;
        std::cin.ignore(100, '\n');  // clears buffer
        if (input_rule == 0)
            input_rule = 267718719;
        else if(input_rule == 1)
            input_rule = 2756018441;
        else if(input_rule == 2)
            input_rule = 3378589909;
        else if(input_rule == 3)
            input_rule = 4096999591;
        else if(input_rule == 4)
            input_rule = 507100781;
        else if(input_rule == 5)
            input_rule = 3635480489;
        else if(input_rule == 6)
            input_rule = 2178553569;
        else if(input_rule == 7)
            input_rule = 1369713987;
        else if(input_rule == 8)
            input_rule = 2174551651;
        else if(input_rule == 9)
            input_rule = 2174551975;
        else if(input_rule == 10)
            input_rule = 507100823;
        else if(input_rule == 11)
            input_rule == 507100809;
        else if(input_rule == 12)
            input_rule == 507100605;
        else if(input_rule == 13)
            input_rule = 507100311;
        
        
        
    }else if(pattern.is_mobile){
        std::cout << "        MOBILES 16bit or 32bit " << std::endl;
        std::cout << "Hardcoded rules to make your enter these rules easier: " << std::endl;
        std::cout << "0: Rule # 42927" << std::endl;
        std::cout << "1: Rule # 55843" << std::endl;
        std::cout << "2: Rule # 27125" << std::endl;
        std::cout << "3: Rule # 44971" << std::endl;
        std::cout << "4: Rule # 19255" << std::endl;
        std::cout << "5: Rule # 50723" << std::endl;
        std::cout << "6: Rule # 55815   -- it can be compressed" << std::endl;
        std::cout << "7: Rule # 58163   -- it can be compressed" << std::endl;
        std::cout << "8: Rule # 2442340644 -- it can be compressed" << std::endl;
        std::cout << "9: Rule # 1936145189 -- it can be compressed" << std::endl;
        std::cout << "10: Rule # 1808763288 -- it can be compressed" << std::endl;
        std::cout << "11: Rule # 1936145421" << std::endl;
        std::cout << "12: Rule # 1936145551" << std::endl;
        std::cout << "13: Rule # 1936148996 -- it can be compressed" << std::endl;
        std::cout << "14: Rule # 841165963  -- it can be compressed" << std::endl;
        std::cout << "15: Rule # 1502583525"  << std::endl;
        std::cout << "Or enter the full rule number yourself below ..." << std::endl;
       
    
        
        std::cout << " Enter new rule 0 -  :";
        std::cin >> input_rule;
        std::cin.ignore(100, '\n');  // clears buffer
        if (input_rule == 0)
            input_rule = 42927;
        else if(input_rule == 1)
            input_rule = 55843;
        else if(input_rule == 2)
            input_rule = 27125;
        else if(input_rule == 3)
            input_rule = 44971;
        else if(input_rule == 4)
            input_rule = 19255;
        else if(input_rule == 5)
            input_rule = 50723;
        else if(input_rule == 6)
            input_rule = 55815;
        else if(input_rule == 7)
            input_rule = 58163;
        else if(input_rule == 8)
            input_rule = 2442340644;
        else if(input_rule == 9)
            input_rule = 1936145189;
        else if(input_rule == 10)
            input_rule = 1808763288;
        else if(input_rule == 11)
            input_rule == 1936145421;
        else if(input_rule == 12)
            input_rule == 1936145551;
        else if(input_rule == 13)
            input_rule = 1936148996;
        else if(input_rule == 14)
            input_rule = 841165963;
        else if(input_rule == 15)
            input_rule = 1502583525;

    }else{
        std::cout << " Enter new rule 0 -  :";
        std::cin >> input_rule;
        std::cin.ignore(100, '\n');  // clears buffer
    }

    return input_rule;

}


// MakePatterns class ------------------------>
/*
*
* Values have to be initialized before the brackets to be used by 
* DisplayMatrix and
* PatternMatrix
*
*/
nature::MakePatterns::MakePatterns(int w, int h, unsigned int rule, int factor) 
    :width(w), height(h), rule(rule), factor(factor){       
}

void nature::MakePatterns::run(){
    
    Display *d = dp_.get_DisplayPtr();
    
    while(is_running_){
        if(XPending(d)){   //while there is a pending event coming from the display
            XNextEvent(d, &event_); // wait for the next event
            std::cout << "Event: " << event_.type << std::endl;
            handle_event();
        
        }
        
    }
    //dp_.mapMatrix(matrix);
}




void nature::MakePatterns::handle_event(){

    if (event_.type == Expose){
        
        dp_.mapMatrix();
    }
    if (event_.type == KeyPress){
        switch(event_.xkey.keycode){
        case UP: //cout << "up" << endl;  
            if(dp_.pattern.compressed) break;  // do nothing in when a graph is compressed mode
            if(dp_.scan_mode){
                dp_.start_row-=dp_.matrix_rows_per_display/dp_.scan_factor;
                dp_.reDraw();
            }else{
                dp_.dot_size++;
                dp_.reDraw();
            }
            break;
        case DOWN: //cout << "down" << endl;
            if(dp_.pattern.compressed) break;  // do nothing in when a graph is compressed mode
            if(dp_.scan_mode){    // scan true 
                dp_.start_row+=dp_.matrix_rows_per_display/dp_.scan_factor;
                dp_.reDraw();
            }else{
                dp_.dot_size--;
                dp_.reDraw();
            }
            break;
        case LEFT: //cout << "left" << endl;
            if(dp_.pattern.compressed) break;  // do nothing in when a graph is compressed mode
            if(dp_.scan_mode){
                dp_.right_limit_reached = false;
                if(!dp_.left_limit_reached){
                    dp_.scan_columns-=dp_.matrix_columns_per_display/dp_.scan_factor;
                }
                dp_.reDraw();
            }else{
                dp_.rule--;
                dp_.reDraw();
            }
            break;
        case RIGHT: //cout << "right" << endl;
            if(dp_.pattern.compressed) break;  // do nothing in when a graph is compressed mode
            if(dp_.scan_mode){
                dp_.left_limit_reached = false;
                if(!dp_.right_limit_reached){
                    dp_.scan_columns+=dp_.matrix_columns_per_display/dp_.scan_factor;
                }
                dp_.reDraw();
            }else{
                dp_.rule++;
                dp_.reDraw();
            }
            break;
        case SPACE:
            dp_.pattern.is_Generalize_Mobile = false;
            if(dp_.pattern.compressed || dp_.scan_mode) break;  // do not allow to change shapes  when compressed mode or scan mode

            dp_.rule = dp_.user_input_rule();
            dp_.reDraw();
            break;
        case SCAN:    // toggle scan 
            if(dp_.pattern.compressed) break;  // do nothing in when a graph is compressed mode
            if (dp_.scan_mode)
                dp_.scan_mode = false;
            else
                dp_.scan_mode = true;
            break;
        case COMPRESS: // compress figure if available  (only for 16 bit to 32bit shapes)
            if(dp_.pattern.is_Generalize_Mobile) break;   // compressed does not work during Generalized mode.
            if(dp_.pattern.compressed){
                dp_.pattern.compressed = false;
                dp_.rule = dp_.pattern.current_compressed;
                dp_.reDraw();
            }else{
                if(dp_.rule < 2188) break;  // compress-ing does not apply to ELEMENTARY or TOTALISTIC
                dp_.pattern.compressed = true;
                if(dp_.rule < 1)
                    dp_.rule = 1;   // lower dot size will increase more time (3 mins) to build a compressed shape.
                dp_.pattern.current_compressed = dp_.rule;
                dp_.reDraw();
            }
            break;
        case MOBILE16B:
            dp_.pattern.is_Generalize_Mobile = false;
            if(dp_.pattern.compressed) break;  // do nothing in when a graph is compressed mode
            srand((unsigned)time(NULL));
            dp_.rule = rule16bstarts+ (rand() % rule16bends);
            dp_.reDraw();
            break;
        case MOBILE32B:
            dp_.pattern.is_Generalize_Mobile = false;
            if(dp_.pattern.compressed) break;  // do nothing in when a graph is compressed mode
            srand((unsigned)time(NULL));
            dp_.rule = ruleTotEnds + (rand() % ruleEnd);
            dp_.reDraw();
            break;
        case ELEMENTARY:
            dp_.pattern.is_Generalize_Mobile = false;
            if(dp_.pattern.compressed) break;  // do nothing in when a graph is compressed mode
            srand((unsigned)time(NULL));
            dp_.rule = ruleStart + (rand() % ruleElemEnds);
            dp_.reDraw();
            break;
        case TOTALISTIC:
            dp_.pattern.is_Generalize_Mobile = false;
            if(dp_.pattern.compressed) break;  // do nothing in when a graph is compressed mode
            srand((unsigned)time(NULL));
            dp_.rule = ruleTotStarts + (rand() % ruleTotEnds);
            dp_.reDraw();
            break;
        case GENERALIZED:
            dp_.pattern.is_Generalize_Mobile = true;
            //if(dp_.pattern.compressed) break;  // do nothing in when a graph is compressed mode
            srand((unsigned)time(NULL));
            dp_.rule = ruleTotEnds + 1 + (rand() % ruleEnd);
            dp_.reDraw();
            break;
        case SPEC_GENERALIZD:
            dp_.pattern.is_Generalize_Mobile = true;  
            dp_.rule = dp_.user_input_rule();
            dp_.reDraw();
            break;
        case ESC: std::cout << "Exiting" << std::endl;
            is_running_ = false;
            break;
        default:  std::cout << "you pressed any other key" << event_.xkey.keycode << std::endl;

        }
    }
}


// MatrixPattern class

nature::MatrixPattern::MatrixPattern(ArrayXXf &matrix, unsigned int rule, stats &pattern){
        
    
    rows = matrix.rows();
    cols = matrix.cols();
    this->rule = rule;
    
    matrix(0,cols/2) = 1;   //initial state of the matrix
    
    getShape(matrix, pattern);
    
    


}


void nature::MatrixPattern::getShape(ArrayXXf &matrix, stats &pattern){
    
    if (rule >= 0 && rule <= 255){    // binary
        pattern.is_8bit = true;
        std::cout << "BINARY 0-255" << std::endl;
        for (int i = 1; i < rows-1;i++)
            for(int j = 1; j < cols -1 ; j++){
                int index = matrix(i-1, j-1)*pow(2,2) + matrix(i-1,j)*pow(2,1) + matrix(i-1, j+1) * pow(2,0);
                int result =(rule & (1 << index)) >> index;
                if (result)
                    matrix(i,j) = 1;
                else 
                    matrix(i,j) = 0;
            }
    }else if (rule >= 256 && rule <= 2186){    // terneary  (base 3)
        pattern.is_ternary = true;
        std::cout <<  "TERNEARIO 256 - 2186" << std::endl; 
        decimal_Base3(rule);
        base3 = ss.str();
        if(base3.length() < 7){
            base3.push_back('0');
        }

        reverse(base3.begin(), base3.end());
        std::cout << base3 << std::endl;

        // std::cout << "this is my base3 number : " << base3 << std::endl;
        // std::cout << "this index 0 " << base3.at(0) << std::endl;
        // std::cout << "this index 1 " << base3.at(1) << std::endl;
        // std::cout << "this index 2 " << base3.at(2) << std::endl;
        // std::cout << "this index 3 " << base3.at(3) << std::endl;
        // std::cout << "this index 4 " << base3.at(4) << std::endl;
        // std::cout << "this index 5 " << base3.at(5) << std::endl;
        // std::cout << "this index 6 " << base3.at(6) << std::endl;
        for (int i = 1; i < rows-1;i++)
            for(int j = 1; j < cols -1 ; j++){
                
                int index = matrix(i-1, j-1) + matrix(i-1,j) + matrix(i-1, j+1);
                //std::cout << " left : " << matrix(i-1, j-1) << "  center: " << matrix(i-1,j) << "  right: " << matrix(i-1, j+1) << std::endl;
                //std::cout << "indexes" << index << std::endl;
                int result =base3.at(index);  // casting not necessary as result = (int)base3.at(index);
                //std::cout << "the result is :" << result << std::endl;
                //if (index > 6 || index < 0 ) exit(0);
                matrix(i,j) = result - 48; // substract 48 since ascii for 0 is 48   and '0' characer is 48 - 48 is 0 as actual number;      
        }

    }else if(rule >= 2087 && rule <= 65535){   //16 bits rules
        std::cout <<  "16-bit MOBILE-AUTOMATA 2187 - 65535" << std::endl;
        pattern.is_mobile = true;
        matrix(0,cols/2) = 2;   //initial state of the matrix (dot on white cell, 3 is a dot on a black cell)
        pattern.most_left = cols/2;
        pattern.most_right = cols/2;
        //std::cout << "Rule : " << rule << std::endl; 
        bin32 = dec_to_32bit();
        //std::cout << "32 bin: " << bin32 << std::endl;
        //int temp2 = pow(2,8);
        //int temp = rule >> 8;
        //printf("shifted 8 places : %u %d \n", rule, temp);
        
        //std::cout << "before loop : " << matrix(0, cols/2) << std::endl;
        //std::cout << matrix << std::endl;

        for (int i = 1; i < rows-1;i++){
            for(int j = 1; j < cols -1 ; j++){
                bool dot = false;
                int dataL, data, dataR;
                // check left neighbor
                if(matrix(i-1, j-1)== 3){ // its a dot on black square, right on top of it
                    //dot = true;
                    dataL = 1;  // 1 for black
                    
                }else if(matrix(i-1,j-1)== 2){ // its a dot on white square right on top of it
                    //dot = true;
                    dataL = 0;  // for white color
                    
                } else{
                    dataL = matrix(i-1, j-1);   // else, then it does not have a dot and could be either white or black 1 or 0
                }

                // check center with dot or not 
                if(matrix(i-1, j)== 3){ // its a dot on black square, right on top of it
                    dot = true;
                    data = 1;  // 1 for black
                    
                }else if(matrix(i-1,j)== 2){ // its a dot on white square right on top of it
                    dot = true;
                    data = 0;  // for white color
                   
                } else{
                    data = matrix(i-1, j);   // else, then it does not have a dot and could be either white or black 1 or 0
                }
                // check right neighbor 
                if(matrix(i-1, j+1)== 3){ // its a dot on black square, right on top of it
                    //dot = true;
                    dataR = 1;  // 1 for black
                    
                }else if(matrix(i-1,j+1)== 2){ // its a dot on white square right on top of it
                    //dot = true;
                    dataR = 0;  // for white color
                    
                } else{
                    dataR = matrix(i-1, j+1);   // else, then it does not have a dot and could be either white or black 1 or 0
                }

                int index = dataL * pow(2,2) + data * pow(2,1) + dataR * pow(2,0);
                
                // if (i == 3){
                //     exit(0);     
                    
                // }
                
                int result =(rule & (1 << index)) >> index;
                //std::cout << "index : " << index <<  "    result : " << result << "  - dot? :" << dot << std::endl;
                if(dot){   // if there was a dot in the above square, then color it accordingly;
                    
                    if (result){     
                        matrix(i,j) = 1;
                        
                        
                    }
                    else{ 
                        matrix(i,j) = 0;
                        
                    }
                }else
                    matrix(i,j) = matrix(i,j) + data;   // if there was no dot above, then just copy the color above plus a dot if it was there.
                
                // process the next 8 bits (out of 16bits)

                int next8bits = (rule >> 8); // shift 8 bits to the left
                //std::cout << "next8bits : " << next8bits << std::endl;
                result =(next8bits & (1 << index)) >> index;   // uses the same index from above
                //std::cout << "new result is : " << result << std::endl;
                if(dot){ // if there was a dot on top in current cell then add the condtions 
                    if (result){  // if result is 1, then places a dot on the right neighbor
                        //data = matrix(i, j+1);   // get the color on the right side
                        matrix(i,j+1) =  2;    // put a dot on the right. If right is white then 0+2=2  ... thats it... right side is originally white or 0
                    }else{   // result is 0 then places a dot on the left side neighbor
                        data = matrix(i, j-1); // gets the color of the left side
                        matrix(i, j-1) =  data + 2;     // 0+2 = 2   means a dot on a white cell or 1 + 2 is a dot on a black cell
                    }
                } // otherwise, ignore and the color was already copied above when there is no dot.
                /// compress option
                
            }
            
        }

    }else if(rule >= 65536 && rule <= 4294967296 && !pattern.is_Generalize_Mobile){   //32 bits rules
        std::cout <<  "32-bit MOBILE-AUTOMATA  65536 - 4billion" << std::endl;
        bin32 = dec_to_32bit();
        std::cout << "32 bin: " << bin32 << std::endl;
        pattern.is_mobile = true;
        matrix(0,cols/2) = 2;   //initial state of the matrix (dot on white cell, 3 is a dot on a black cell)
        pattern.most_left = cols/2;
        pattern.most_right = cols/2;
        //std::cout << "Rule : " << rule << std::endl; 
        bin32 = dec_to_32bit();
        //std::cout << "32 bin: " << bin32 << std::endl;
        //int temp2 = pow(2,8);
        //int temp = rule >> 8;
        //printf("shifted 8 places : %u %d \n", rule, temp);
        
        //std::cout << "before loop : " << matrix(0, cols/2) << std::endl;
        //std::cout << matrix << std::endl;

        for (int i = 1; i < rows-1;i++){
            for(int j = 1; j < cols -1 ; j++){
                bool dot = false;
                bool far_dot = false;
                int dataL, data, dataR;
                // check left neighbor
                if(matrix(i-1, j-1)== 3){ // its a dot on black square, right on top of it
                    far_dot = true;   // there is  a dot that already affected future cells
                    dataL = 1;  // 1 for black
                    
                }else if(matrix(i-1,j-1)== 2){ // its a dot on white square right on top of it
                    far_dot = true;  // there is a dot that already affected the color
                    dataL = 0;  // for white color
                    
                } else{
                    dataL = matrix(i-1, j-1);   // else, then it does not have a dot and could be either white or black 1 or 0
                }

                // check center with dot or not 
                if(matrix(i-1, j)== 3){ // its a dot on black square, right on top of it
                    dot = true;
                    data = 1;  // 1 for black
                    
                }else if(matrix(i-1,j)== 2){ // its a dot on white square right on top of it
                    dot = true;
                    data = 0;  // for white color
                   
                } else{
                    data = matrix(i-1, j);   // else, then it does not have a dot and could be either white or black 1 or 0
                }
                // check right neighbor 
                if(matrix(i-1, j+1)== 3){ // its a dot on black square, right on top of it
                    //dot = true;
                    dataR = 1;  // 1 for black
                    
                }else if(matrix(i-1,j+1)== 2){ // its a dot on white square right on top of it
                    //dot = true;
                    dataR = 0;  // for white color
                    
                } else{
                    dataR = matrix(i-1, j+1);   // else, then it does not have a dot and could be either white or black 1 or 0
                }

                int index = dataL * pow(2,2) + data * pow(2,1) + dataR * pow(2,0);
                
                // if (i == 4 && j == 15){
                //     std::cout  << matrix << std::endl;
                //     exit(0);     
                    
                // }
                
                int result =(rule & (1 << index)) >> index;   // FIRST 8 BITS
                //std::cout << "index : " << index <<  "    result : " << result << "  - dot? :" << dot << std::endl;
                if(dot){   // if there was a dot in the above square, then color it accordingly;
                    
                    if (result){     
                        matrix(i,j) = 1;
                        
                        
                    }
                    else{ 
                        matrix(i,j) = 0;
                        
                    }
                }else{
                    //if(matrix(i,j)==3) data--;  // if there is a dot on a black cell then keep it as it is by substracting data 1
                    if(matrix(i,j) == 0 and !far_dot)  //if there is nothing
                        matrix(i,j) = matrix(i,j) + data;   // if there was no dot above, then just copy the color above .
                    else
                        far_dot = false;  // do nothing and move on.. this cell was already affected by previous dot
                    
                }
                
                // next 8 BITS  = 16 BITS

                int next8bits = (rule >> 8); // shift 8 bits to the left
                
                //std::cout << "next8bits : " << next8bits << " - " << (int)(rule / pow(2,8)) << std::endl;
                result =(next8bits & (1 << index)) >> index;   // uses the same index from above
                //std::cout << "new result is : " << result << std::endl;
                if(dot){ // if there was a dot on top in current cell then add the condtions 
                    if (result){  // if result is 1, then places a dot on the right neighbor
                        //data = matrix(i, j+1);   // get the color on the right side
                        matrix(i,j+1) =  2;    // put a dot on the right. If right is white then 0+2=2  ... thats it... right side is originally white or 0
                    
                    
                    
                    }else{   // result is 0 then places a dot on the left side neighbor
                        data = matrix(i, j-1); // gets the color of the left side
                        matrix(i, j-1) =  data + 2;     // 0+2 = 2   means a dot on a white cell or 1 + 2 is a dot on a black cell



                    }
                } // otherwise, ignore .
                // if (i == 4 && j == 15){
                //     std::cout  << matrix << std::endl;
                //     exit(0);     
                    
                // }
                // next 8 bits = 24 bits
                next8bits = (rule >> 16);  // shifts 8 more bits to the left
                //std::cout << "next8bits : " << next8bits << " - " << (int)(rule / pow(2,16)) << std::endl;
                result = (next8bits & (1 << index)) >> index;   // keeps using the same index
                if(dot){
                    if(result){  // if result is 1 then color black the right neighbor
                        data = matrix(i,j+1);   // get the current state black , dot, white
                        if(data == 3 || data == 1) data--;
                        matrix(i,j+1) = data + 1;

                    }else{   // color white the right neighbor
                        data = matrix(i,j+1);
                        matrix(i,j+1) = data + 0;
                        //watch here about the color or dot that was alredy here!!!!
                    }
                } // otherwise, ignore and the color was already copied above when there is no dot. 
                // if (i == 5 && j == 14){
                //     std::cout  << matrix << std::endl;
                //     exit(0);     
                    
                // }
                // next 8 bits = 32 bits
                next8bits = (rule >> 24); // shifts 8 more bits to the left
                //std::cout << "next8bits : " << next8bits << " - " << (int)(rule / pow(2,24)) << std::endl;
                result = (next8bits & (1 << index)) >> index;   //keeps using same index
                if(dot){
                    if(result){   // color black LEFT neighbor
                        data = matrix(i,j-1);
                        //std::cout << "data : " << data << std::endl;
                        if(data == 3 || data == 1 ) data--; // if it is already black, just adjust to continue being black
                        
                        matrix(i, j-1) = data + 1;
                    }else{     // color white the LEFT neighbor
                        data = matrix(i,j-1);
                        if (data == 3 || data == 1 ) data--; // if it is other color, just adjust to be white
                        matrix(i, j-1) = data + 0;
                        // watch here about the color that was already here!!!!
                    }
                } // otherwise, ignore and the color was already copied above when there is no dot.
                // if (i == 3 && j == 10){
                //     std::cout  << matrix << std::endl;
                //     exit(0);     
                    
                // }
            }
            
        }

    }else if(pattern.is_Generalize_Mobile){
        std::cout <<  "GENERALIZED 32-bit MOBILE-AUTOMATA  65536 - 4billion" << std::endl;
        bin32 = dec_to_32bit();
        std::cout << "32 bin: " << bin32 << std::endl;
        pattern.is_mobile = true;
        matrix(0,cols/2) = 2;   //initial state of the matrix (dot on white cell, 3 is a dot on a black cell)
        pattern.most_left = cols/2;
        pattern.most_right = cols/2;
        //std::cout << "Rule : " << rule << std::endl; 
        bin32 = dec_to_32bit();
        //std::cout << "32 bin: " << bin32 << std::endl;
        //int temp2 = pow(2,8);
        //int temp = rule >> 8;
        //printf("shifted 8 places : %u %d \n", rule, temp);
        
        //std::cout << "before loop : " << matrix(0, cols/2) << std::endl;
        //std::cout << matrix << std::endl;

        for (int i = 1; i < rows-1;i++){
            for(int j = 1; j < cols -1 ; j++){
                bool dot = false;
                bool far_dot = false;
                int dataL, data, dataR;
                // check left neighbor
                if(matrix(i-1, j-1)== 3){ // its a dot on black square, right on top of it
                    far_dot = true;   // there is  a dot that already affected future cells
                    dataL = 1;  // 1 for black
                    
                }else if(matrix(i-1,j-1)== 2){ // its a dot on white square right on top of it
                    far_dot = true;  // there is a dot that already affected the color
                    dataL = 0;  // for white color
                    
                } else{
                    dataL = matrix(i-1, j-1);   // else, then it does not have a dot and could be either white or black 1 or 0
                }

                // check center with dot or not 
                if(matrix(i-1, j)== 3){ // its a dot on black square, right on top of it
                    dot = true;
                    data = 1;  // 1 for black
                    
                }else if(matrix(i-1,j)== 2){ // its a dot on white square right on top of it
                    dot = true;
                    data = 0;  // for white color
                   
                } else{
                    data = matrix(i-1, j);   // else, then it does not have a dot and could be either white or black 1 or 0
                }
                // check right neighbor 
                if(matrix(i-1, j+1)== 3){ // its a dot on black square, right on top of it
                    //dot = true;
                    dataR = 1;  // 1 for black
                    
                }else if(matrix(i-1,j+1)== 2){ // its a dot on white square right on top of it
                    //dot = true;
                    dataR = 0;  // for white color
                    
                } else{
                    dataR = matrix(i-1, j+1);   // else, then it does not have a dot and could be either white or black 1 or 0
                }

                int index = dataL * pow(2,2) + data * pow(2,1) + dataR * pow(2,0);
                
                // if (i == 4 && j == 15){
                //     std::cout  << matrix << std::endl;
                //     exit(0);     
                    
                // }
                
                int result =(rule & (1 << index)) >> index;   // FIRST 8 BITS
                //std::cout << "index : " << index <<  "    result : " << result << "  - dot? :" << dot << std::endl;
                if(dot){   // if there was a dot in the above square, then color it accordingly;
                    
                    if (result){     
                        data = matrix(i,j);
                        if(data == 0 || data == 2) // if there is a dot on a black cell
                            matrix(i,j) = data + 1;
                        
                        //    matrix(i,j) = 3;    //keep it there
                        else
                            matrix(i,j) = 1;   // otherwise, just color it black
                        
                        
                    }
                    else{ 
                        data = matrix(i,j); // color white
                        if(data == 2)  // if there is a dot on a white cell
                            matrix(i,j) = data + 0; // keep the dot on a while cell
                        //else
                            //matrix(i,j) = 0;  // otherwise   just color it white
                        
                    }
                }else{
                    //if(matrix(i,j)==3) data--;  // if there is a dot on a black cell then keep it as it is by substracting data 1
                    //if(matrix(i,j) == 0 /*and !far_dot*/)  //if there is nothing
                        matrix(i,j) = matrix(i,j) + data;   // if there was no dot above, then just copy the color above .
                    //else
                        //far_dot = false;  // do nothing and move on.. this cell was already affected by previous dot
                    
                }
                
                // next 8 BITS  = 16 BITS ==== places a dot in the middle
                 int next8bits = (rule >> 8); // shift 8 bits to the left
                
                //std::cout << "next8bits : " << next8bits << " - " << (int)(rule / pow(2,8)) << std::endl;
                result =(next8bits & (1 << index)) >> index;   // uses the same index from above
                //std::cout << "new result is : " << result << std::endl;
                if(dot){ // if there was a dot on top in current cell then add the condtions 
                    if (result){  // if result is 1, then places a dot in the middle
                        data = matrix(i, j);   // get the color in the middle
                        if(data == 0 || data == 2)
                            matrix(i,j) =  2;    // If center is white then 2  ... if black then dot on black cell 3
                        if(data == 1 || data == 3)
                            matrix(i,j) = 3;
                    
                    
                    }else{   // result is 0 then do nothing...NO dot in the middle or maybe, there was already one before.
                        //nothing
                    }
                }

                // next 8 bits = 24 bits === places a dot on the right side or not    
                next8bits = (rule >> 16); // shift 16 bits to the left
                
                //std::cout << "next8bits : " << next8bits << " - " << (int)(rule / pow(2,8)) << std::endl;
                result =(next8bits & (1 << index)) >> index;   // uses the same index from above
                //std::cout << "new result is : " << result << std::endl;
                if(dot){ // if there was a dot on top in current cell then add the condtions 
                    if (result){  // if result is 1, then places a dot on the right neighbor
                        //data = matrix(i, j+1);   // get the color on the right side
                        matrix(i,j+1) =  2;    // put a dot on the right. If right is white then 0+2=2  ... thats it... right side is originally white or 0
                    
                    
                    
                    }else{   // result is 0 , do nothing                       
                       // nothing
                    }
                } // otherwise, ignore .

                // if (i == 4 && j == 15){
                //     std::cout  << matrix << std::endl;
                //     exit(0);     
                    
                // }
                
                // next 8 bits = 32 bits
                next8bits = (rule >> 24); // shifts 8 more bits to the left
                //std::cout << "next8bits : " << next8bits << " - " << (int)(rule / pow(2,24)) << std::endl;
                result = (next8bits & (1 << index)) >> index;   //keeps using same index
                if(dot){
                    if(result){   // put a dot on  LEFT neighbor
                        data = matrix(i,j-1);
                        //std::cout << "data : --------------------------->" << data << std::endl;
                        if(data == 3 || data == 1 )  // if it is already black, just adjust to continue being black
                            matrix(i, j-1) = 3;
                        if(data == 0 || data == 2 )
                            matrix(i,j-1) = 2;
                    }else{     // do nothing

                    }
                } // otherwise, ignore and the color was already copied above when there is no dot.
                // if (i == 6 && j == 13){
                //     std::cout  << matrix << std::endl;
                //     exit(0);     
                    
                // }
            }
            
        }
    }

    //}
    // matrix has been completed
    //std::cout  << matrix << std::endl;
    //exit(0);
    pattern.matrix_sum = matrix.sum();
    pattern.matrix_prod = matrix.prod();
    pattern.matrix_mean = matrix.mean();
    pattern.matrix_minCoef = matrix.minCoeff();
    pattern.matrix_maxCoef = matrix.maxCoeff();
    //std::cout << matrix.trace() << std::endl;
    
    
    
}   

void nature::MatrixPattern::decimal_Base3(int N){

    // Base case
    if (N == 0)
        return;
 
    // Finding the remainder
    // when N is divided by 3
    int x = N % 3;
    N /= 3;
    if (x < 0)
        N += 1;
 
    // Recursive function to
    // call the function for
    // the integer division
    // of the value N/3
    decimal_Base3(N);
 
    // Handling the negative cases
    if (x < 0)
        ss << x + (3 * -1);
    else
        ss << x;
}


std::string nature::MatrixPattern::dec_to_32bit(){
    char bin32[] = "00000000000000000000000000000000";
    unsigned int r = rule;
    for (int pos = 31; pos >= 0; --pos)
    {
        if (r % 2){ 
            //std::cout << "1" << std::endl;
            bin32[pos] = '1';
        }
        r /= 2;
    }
    return bin32;
}