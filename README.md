# automata

Linux/ubuntu


1. Create a directory 
    mkdir dev
2. cd dev
3. git clone https://github.com/dpbahena/automata
4. cd automata
5. mkdir build
6. cd build
7. cmake ..
8. make


That's it!

To run the program:

Option 1:   You can access ramdomly all patterns from rule 0 to 2^32. Once, you are in the program, follow the on-screen instructions.
            When accessing a particular rule you can press SPACE  (Sorry, you have to click back/forth from terminal to window)

 ./cellautomata 


Option 2:   You have more control on the window size and allows you to make the matrix bigger so you have more scanning capability with more depth.

./cellautomata <display width> <display hight> <rule> <matrix-multiplier>
             
Example 1:

./cellautomata 1857 1057 1635 40     

This command line the rule is 1635 and makes the data matrix 40 times the width and 40 times the height. 

Example 2: 

Sometimes the shape does not occuppy the whole width of the screen. For example rule, 1599.  You may use:

./cellautomata 800 1057 1599 40        

This way, the time to produce a matrix 40 times the display dimensions is reduce. 











