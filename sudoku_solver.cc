// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 
// Entry to Prof. Hünenberger's challenge to InfoI-students (HS2016)                                                                                                                                           //
// by Andreas Gimpel, agimpel@student.ethz.ch, former InfoI-student                                                                                                                                            //
//                                                                                                                                                                                                             //
// The strategy of this implementation is based on the "Backtracking"-algorithm described on Wikipedia                                                                                                         //
// https://en.wikipedia.org/wiki/Sudoku_solving_algorithms#Backtracking                                                                                                                                        //
//                                                                                                                                                                                                             //
// algorithmic strategy:                                                                                                                                                                                       //
// For a blank cell, find a number that is not present in the same row, column and box.                                                                                                                        //
// This number is placed in the blank. Proceed to the next blank and repeat procedure.                                                                                                                         //
// If for any blank cell, there are no possible numbers to be filled in, reconsider the previous cell that was filled (-> backtrack).                                                                          //
// For that previous cell, choose a different number that also satisfies the sudoku conditions. If there is no other possible number, reset that cell to a blank and backtrack further.                        //
// Once another possible number can be chosen, return to the described procedure.                                                                                                                              //
// The algorithm is finished if the last blank cell is successfully filled. At that point, every blank satisfies the sudoku conditions and the sudoku is solved.                                               //
//                                                                                                                                                                                                             //
// implementation details:                                                                                                                                                                                     //
// *Use of 'char' as main data type for numbers, as 1 byte is sufficent                                                                                                                                        //
// *Use of only one-dimensional arrays as well as pointers                                                                                                                                                     //
// *Use of both 'stationary' and 'relative' pointers to reduce the amount of addition and multiplication necessary to produce the correct offset to an array                                                   //
// *While reading the input grid from file, the row, column and box of each cell is precalculated, the position (=offset, row, column & box) of every blank and the numbers already in use are saved           //
// *With the precalculated offsets, row, column and box, no cell information has to be calculated prior to evaluation of possible numbers                                                                      //
// *Instead of recollecting the numbers within row, column and box every time a cell is considered, the numbers in use for every row, column and box are kept track of                                         //
// *Quite a few small optimizations to preliminary calculation of offsets, loop conditions and value consideration for blank cells                                                                             //
//                                                                                                                                                                                                             //
// compilation and execution:                                                                                                                                                                                  //
// *no additional arguments necessary for compilation and execution                                                                                                                                            //
// *input file 'grid.dat' has to be present in the same directory, with valid sudoku grid                                                                                                                      //
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //



#include <iostream>                                       //iostream for output
#include <fstream>                                        //fstream to read input file

using namespace std;                                      //standard namespace
typedef char num;                                         //define new type 'num' as type char for 1 byte-sized unsigned 'integers', new name to avoid confusion with 'real' characters



// ==============================================================================================================================================================================================================
// GLOBAL VARIABLES
// ==============================================================================================================================================================================================================

num sudoku_grid[81];                                      //array of size 81 to hold all grid information in one-dimensional array, split into usual 9x9 grid via offset=9*row+column
num * sudoku = sudoku_grid;                               //pointer pointing to the memory of sudoku_grid
num * rel_sudoku = sudoku_grid;                           //pointer pointing to the memory of the current position in sudoku_grid

unsigned short int empty_cells[81];                       //array of size 81 to hold all offsets for the empty cells of the sudoku grid; 2D with offset=9*row+column
unsigned short int * empty = empty_cells;                 //pointer pointing to the memory of empty_cells
unsigned short int * rel_empty = empty_cells;             //pointer pointing to the memory of the current position in empty_cells

num value = 0;                                            //will give the value to be evaluated at the current position within the sudoku

unsigned short int cols_grid[81];                         //array of size 81 to hold the column of every blank cell; 2D with offset=9*row+column
unsigned short int * cols = cols_grid;                    //pointer pointing to the memory of cols_grid
unsigned short int * rel_cols = cols_grid;                //pointer pointing to the memory of the current blank in cols_grid
unsigned short int rows_grid[81];                         //array of size 81 to hold the row multiple (that is 9*row) of every blank cell; 2D with offset=9*row+column
unsigned short int * rows = rows_grid;                    //pointer pointing to the memory of rows_grid
unsigned short int * rel_rows = rows_grid;                //pointer pointing to the memory of the current blank in rows_grid
unsigned short int boxes_grid[81];                        //array of size 81 to hold the box of every blank cell; 2D with offset=9*row+column
unsigned short int * boxes = boxes_grid;                  //pointer pointing to the memory of boxes_grid
unsigned short int * rel_boxes = boxes_grid;              //pointer pointing to the memory of the current blank in boxes_grid

bool used_values_col[120];                                 //array of size 120 to hold usage info for every number for every column (if in use=0, 1 otherwise); 2D with offset=12*column+number
bool * used_col = used_values_col;                         //pointer pointing to the memory of used_values_col
bool * used_rel_col = used_values_col;                     //pointer pointing to the memory of the current column and value in used_values_col
bool used_values_row[120];                                 //array of size 120 to hold usage info for every number for every row (if in use=0, 1 otherwise); 2D with offset=12*row+number
bool * used_row = used_values_row;                         //pointer pointing to the memory of used_values_row
bool * used_rel_row = used_values_row;                     //pointer pointing to the memory of the current row and value in used_values_row
bool used_values_box[120];                                 //array of size 120 to hold usage info for every number for every box (if in use=0, 1 otherwise); 2D with offset=12*box+number
bool * used_box = used_values_box;                         //pointer pointing to the memory of used_values_box
bool * used_rel_box = used_values_box;                     //pointer pointing to the memory of the current box and value in used_values_box



// ==============================================================================================================================================================================================================
// MAIN
// ==============================================================================================================================================================================================================

int main(){

  //preliminary operations
  for(num l=0;l<120;l++){                                 //set every element of the used arrays to true to indicate that no numbers are in use yet
    *(used_col+l) = true;                                 //
    *(used_row+l) = true;                                 //
    *(used_box+l) = true;                                 //
  }                                                       //


  ifstream data;                                          //with fstream
  data.open("grid.dat");                                  //open the input file 'grid.dat' in the same directory

  //input loop
  num insert_i = 0;                                       
  while(insert_i!=81){                                    //go through all 81 elements of the sudoku
    data >> *rel_sudoku;                                  //insert current element into array position pointed to by rel_sudoku
    *rel_sudoku = *rel_sudoku - '0';                      //inserted 'number' is considered a 'character', subtract '0'==48 to obtain 'correct' ie. integer value
    unsigned short int col = (insert_i%9)*12;             //calculate column multiple (12*column) by modulo of current offset with 9, gives offset to previous multiple of 9 = column
    unsigned short int row = ((insert_i - col/12)/9)*12;  //calcualte row multiple (12*row) by truncating current offset after division by number of columns p
    unsigned short int box = ((row/36)*3 + (col/36))*12;  //calculate box multiple (12*box) by truncating (row/3) and (column/3) (36 in code since col and row are gives as multiples of 12 and 3*12=36)

    //input evaluation
    if(*rel_sudoku){                                      //if inserted cell is not to be filled, ie. contains a number as sudoku constraint:
      *(used_col+col+*rel_sudoku) = false;                //save this number as a number that is already in use within the current column
      *(used_row+row+*rel_sudoku) = false;                //same for row
      *(used_box+box+*rel_sudoku) = false;                //same for box
    } else {                                              //if inserted cell is to be filled:
      *(rel_empty++) = insert_i;                          //save position in array empty_cells pointed to by rel_empty; increment rel_empty for next iteration
      *(rel_cols++) = col;                                //save column multiple so it does not have to be recalculated if this blank cell is considered, increment for next iteration
      *(rel_rows++) = row;                                //same for row multiple
      *(rel_boxes++) = box;                               //same for box multiple
    }
    rel_sudoku++;                                         //for the next iteration, the next cell is to be filled
    insert_i++;                                           //next cell offset is to be considered
  }
  *rel_empty = 81;                                        //the element after the last saved position of array empty_cells is set to 81 (invalid value) to act as sentinel
  data.close();                                           //close input file

  //reset of pointers
  rel_empty = empty;                                      //reset relative position of pointer rel_empty to beginning of array empty_cells
  rel_cols = cols;                                        //same for rel_cols
  rel_rows = rows;                                        //same for rel_rows
  rel_boxes = boxes;                                      //same for rel_boxes
  rel_sudoku = sudoku + *rel_empty;                       //relative position of rel_sudoku is set to the first blank by offsetting pointer sudoku by the position of the first blank saved in *rel_empty
 
  //loop over blanks
  while(*rel_empty!=81){                                  //as long as the sentinel of empty_cells (==81) is not yet reached, indicating there are empty cells left to fill
    used_rel_row = used_row+*rel_rows;                    //update relative pointer to used numbers in row to current cell's row multiple
    used_rel_col = used_col+*rel_cols;                    //same for column
    used_rel_box = used_box+*rel_boxes;                   //same for box

    //evaluation loop
    bool valid;                                           //stores the result of duplicate comparison, false if duplicate is present, true otherwise
    do{
      value++;                                            //increment value for evaluation (at first iteration from blank==0 to 1)
      valid = *(used_rel_row+value) && *(used_rel_col+value) && *(used_rel_box+value);  //check for duplicates by checking if the respective position within used has been set to 1
    } while(!valid && value!=9);                          //as long as there is a duplicate of the same value within row/column/box and the value is < 9

    //updating of pointers and collections
    if(valid){                                            //if the dowhile-loop terminated due to finding no duplicate -> valid=true
      *rel_sudoku = value;                                //evaluated value is a possible solution, insert it into the array at the position pointed to by rel_sudoku
      *(used_rel_row+value) = false;                      //the new value is now in use in this cell's row, indicate this in the respective row collection
      *(used_rel_col+value) = false;                      //same for column
      *(used_rel_box+value) = false;                      //same for box
      rel_empty++;                                        //the next blank is to be considered
      rel_cols++;                                         //update relative pointer to column
      rel_rows++;                                         //same for row
      rel_boxes++;                                        //same for box
      rel_sudoku = sudoku + *rel_empty;                   //update rel_sudoku accordingly
      value = 0;                                          //value is set to the value of the next blank (ie. 0)
    } else {                                              //if there is no duplicate in row, column and box
      while(value==9){                                    //as long as the previous cells to be filled are at max value
	*rel_sudoku = 0;                                  //reset current cell back to a blank since assumed solution is invalid
	rel_empty--;                                      //go back to the previous cell which was already considered and incorrectly filled
	rel_cols--;                                       //update relative pointer to column 
	rel_rows--;                                       //same for row
	rel_boxes--;                                      //same for box
	rel_sudoku = sudoku + *rel_empty;                 //rel_sudoku now points to the previous cell. If it is 9, the loop continues as the error has to be in a previous cell (9 is max value)
	value = *rel_sudoku;                              //update value to reflect the value of the new cell
	*(used_row+*rel_rows+value) = true;               //the value of the incorrectly filled cell is no longer in use, indicate this in the respective cell's row collection
	*(used_col+*rel_cols+value) = true;               //same for column
	*(used_box+*rel_boxes+value) = true;              //same for box
      }
    }
  }

  //Output of solution
  for(num i=0;i<81;i++){                                  //for all 81 elements of the sudoku
    if(!(i%9)) cout << '\n';                              //insert a new line if new row is reached
    cout << (int) *(sudoku+i);                            //print value of sudoku_grid at position pointed to by (sudoku+i)
  }
  cout << '\n';

  return 0;                                               //obligatory return statement of main()
}


/* LOG OF OPTIMIZATION
readings based on this worst-case sudoku and without any optimization flags:
000000010000002003000400000000000500401600000007100000050000200000080040030910000

0m51.620s	base reading

1m10.074s	check order: box, col, row

0m56.600s	check order: box, row, col

0m53.362s	check order: row, col, box

0m43.493s	move col, row_m and box calculations from function to main
		//doing the same thing 9 times over at worst is indeed unnecessary

0m34.340s	reworked conditions for duplicate while loop and following if, save one additional loop over all conditions
		//apparently, one additional loop = 1/4 extra time

0m33.300s	define col, row_m and box as global variables to prevent overhead by passing variables
		//interestingly does not change much

0m32.536s	move relocation of rel_sudoku to if-cases, prevent extra assignment per loop

0m30.394s	reworked evaluation of current cell to not test within array, instead uses global variable value
		//not a big improvement

0m26.683s	box-check: remove modulo operation and use if cases
		//quite significant difference

0m20.375s	row-check: use if cases instead of for-loop
		//and someone said assignments were cheap

0m16.750s	col-check: use if cases instead of for-loop
		//well

0m15.776s	move assignment of row, column and box pointers from function to main
		//same thing performed at worst 9 times over is also unnecessary here

0m12.853s	instead of comparing each new value of dowhile-loop to all row/column/box elements again, gather all existing values first and instead compare against that collection
		//kind of self-evident

0m10.187s	row, column and box of each blank cell is now calculated upon reading the cell from file (this info obviously doesn't change), instead of each time anew upon consideration of a blank cell
		//interesting how such a detail accounts for about 1/4 of computation time

0m8.986s	instead of collecting the existing values each time a new blank is considered, keep track of them for every row, column and box
		//impact not as big as expected, introduced a lot of new assignments

0m8.190s	instead of pointing to two-dimensional arrays for storing existing values for each row, column and box, use one-dimensional arrays with offset
		//that one exam exercise pays off

0m7.636s	store row, column and box number as multiple of 12 to remove additional operations from while loop, data type changed to unsigned short int

0m7.541s	instead of storing row, column and box information as well as empty cells as char, change data type to unsigned short int
		//maybe prevents implicit casts to int upon use for offsets (?)

0m7.326s	in output to terminal, use '\n' instead of endl to prevent the expensive emptying of the buffer

0m7.013s	precalculate relative position of pointers to row, column and box outside of dowhile-loop to save three additions per loop iteration

0m6.782s	if a blank is succesfully filled, the next blank's value has to be 0. So it is unnecessary to read 0 from the array
                //this fact was even described in that line's comment

0m6.499s	reworked conditions of dowhile loop once more; saves one loop if terminating due to not finding a possible value to fill in

0m6.213s	reworked logic for evaluation of duplicates. By use of AND instead of OR, short-circuit property prevents further evaluation if number is already used in a previous test case

0m6.192s	arrays containing the existence information of numbers for every row, column and box are now type bool

0m6.116s	invert pointer update ifclause to prevent additional negation
*/
