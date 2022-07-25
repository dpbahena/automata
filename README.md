# automata

<h1>Linux/ubuntu</h1>


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

<h1>To run the program:</h1>

<h2>Option 1:</h2>   
You can access ramdomly all patterns from rule 0 to 2^32. Once, you are in the program, follow the on-screen instructions. When accessing a particular rule you can press SPACE  (Sorry, you have to click back/forth from terminal to window)


 ./cellautomata 


<h2>Option 2:</h2>   
You have more control on the window size and allows you to make the matrix bigger so you have more scanning capability with more depth.

 
./cellautomata <code>&lt;display width&gt; &lt;display hight&gt; &lt;rule&gt; &lt;matrix-multiplier&gt;
</code>
            
   #### Example 1: ####

./cellautomata 1857 1057 1635 40     

This command line the rule is 1635 and makes the data matrix 40 times the width and 40 times the height. 

   #### Example 2: ####

Sometimes the shape does not occuppy the whole width of the screen. For example rule, 1599.  You may use:

./cellautomata 800 1057 1599 40        

This way, the time to produce a matrix 40 times the display dimensions is reduce. 


   <h2>ELEMENTARY RULES  0 - 255    based on 8-bit binary</h2>

 * See examples #1 and #2

 or

 * press "E" to for random rule

 or 
 
 * press "SPACE" to type specific rule.

   <h2>TOTALISTIC RULES  256 - 2186   base 3</h2>

* See examples #1 and #2
 
or

* press "T" to for random rule 

or

*press "SPACE" to type specific rule.

   <h2>MOBILE-16 BITS     (I started it from 2187 to 65535 or 2^16)</h2>

* See examples #1 and #2

or

* press "N" to for random rule

or

* press "SPACE" to type specific rule.

Press "C" for COMPRESSED mode (compressed mode only works in certain shapes)
Examples from "A New Kind of Science" by Stephen Wolfram:

* Rule # 42927
* Rule # 55843
* Rule # 27125
* Rule # 44971
* Rule # 19255
* Rule # 50723
* Rule # 55815   COMPRESSED-MODE
* Rule # 58163   COMPRESSED-MODE


 <h2>MOBILE-32 BITS     (I started it from 65536 to 4,294,967,295  or 2^32)</h2>

See examples #1 and #2 or press "M" to for random rule or  press "SPACE" to type specific rule.

Press "C" for COMPRESSED mode (compressed mode only works in certain shapes)
Examples from "A New Kind of Science" by Stephen Wolfram:

See pages 73, 74, 75;

* Rule # 2442340644  press 'C' for Compressed mode     
* Rule # 1936145189  press 'C' for Compressed mode
* Rule # 1808763288  press 'C' for Compressed mode


Other interesting shapes:

* Rule # 1936145421  
* Rule # 1936145551
* Rule # 1936148996  press 'C' for Compressed mode
* Rule # 841165963   press 'C' for Compressed mode
* Rule # 1502583525






 <h2>GENERALIZED MOBILE</h2>

#### Option 1: ####

Press "G" for random mode

#### Option 2: ####

Press "H" for specific rule

Interesting rules:
from "A New Kind of Science" by Stephen Wolfram 

* Rule # 267718719    page 76
* Rule # 2756018441   page 77
* Rule # 3378589909   page 77
* Rule # 4096999591   page 77
* Rule #  507100781   page 77
* Rule # 3635480489   page 77
* Rule # 2178553569   page 77
* Rule # 1369713987   page 77
* Rule # 2174551651   page 77


Anothers ones are :   
* Rule # 2174551975
* Rule # 507100823   (really weird)
* Rule # 507100809
* Rule # 507100605 

COMPRESS MODE: Makes no sense in Generalized Mobiles



Note:  You can find your own interesting shapes, the problem is there are more than 4 billions to look from!!
Good Luck!!



