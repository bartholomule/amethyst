
#include <iostream>
#include <iomanip>

using namespace std;

#include <manygames/raster.hpp>
#include <manygames/useful_functions.hpp>
#include "testinclude.hpp"
#include <cstring>

typedef manygames::raster<int> irast;

template <class T>
ostream& operator<<(ostream& o, const manygames::raster<T>& r)
{

  string s = "+" + string(r.get_width() * 4 + 1, '-') + "+";

  o << s << endl;

  for(unsigned y = 0; y < r.get_height(); ++y)
  {
    o << "|";
    for(unsigned x = 0; x < r.get_width(); ++x)
    {
      o << " " << setw(3) << r[y][x];
    }
    o << " |" << endl;
  }
  
  o << s << endl;
  return o;
}

// Implement the constructor for the size_mismatch constructor... To prevent
// the need to have another file linked against this one.
namespace manygames {
  size_mismatch::size_mismatch(const std::string& __arg):
    std::runtime_error(__arg)
  {
  } // size_mismatch()
}

#define ERROR_TEXT(text) ((++error_count),text)

int main(int argc, char** argv)
{
  const int width = 10;
  const int height = 12;
  int error_count = 0;
  const char* passed = "Passed";
  const char* failed = "FAILED!";
  
  try
  {
    cout << "------------------------------" << endl;
    cout << ( "Creating a " +
	      int_to_str(width) + "x" + int_to_str(height) +
	      " raster" )
	 << endl;
    cout << "------------------------------" << endl;

    irast sized(width,height);
    TEST_RESULT_NAMED(( "to see if the size of a " +
			int_to_str(width) + "x" + int_to_str(height) +
			" raster is " + int_to_str(width * height) ),
		      sized.get_numpixels() == width * height,
		      passed,
		      ERROR_TEXT(failed));
    

    TEST_EXCEPTION_NAMED("to assign within bounds",
			 for(unsigned y = 0; y < height; ++y)
                         {
			   for(unsigned x = 0; x < width; ++x)
			   {
			     sized[y][x] = (x == y)?(x):(x + y);
			   }
			 }
			 ,
			 passed,
			 ERROR_TEXT(failed));

    TEST_RESULT_NAMED("to see if assignments worked",
		      (
		       // All of the diagonal elements...
		       (sized[0][0] == 0) &&
		       (sized[1][1] == 1) &&
		       (sized[2][2] == 2) &&
		       (sized[3][3] == 3) &&
		       (sized[4][4] == 4) &&
		       (sized[5][5] == 5) &&
		       (sized[6][6] == 6) &&
		       (sized[7][7] == 7) &&
		       (sized[8][8] == 8) &&
		       (sized[9][9] == 9) &&
		       // Some randomly chosen non-diag elements.
		       (sized[4][7] == 11) &&	
		       (sized[9][6] == 15) &&
		       (sized[2][0] ==  2) &&	
		       (sized[3][9] == 12)
		      ),
		      passed,
		      ERROR_TEXT(failed));
		     

    TEST_EXCEPTION_NAMED("to copy same-sized raster scanlines",
			 sized[0] = sized[4] = sized[9];
			 sized[8] = sized[5] = sized[1];
			 ,
			 passed,
			 ERROR_TEXT(failed));

    TEST_RESULT_NAMED("to see if scanline copy worked",
		      (
		       // All of the diagonal elements not modified...
		       (sized[1][1] == 1) &&
		       (sized[2][2] == 2) &&
		       (sized[3][3] == 3) &&
		       (sized[6][6] == 6) &&
		       (sized[7][7] == 7) &&
		       (sized[9][9] == 9) &&
		       // Modified diags...
		       (sized[0][0] == 9) &&
		       (sized[4][4] == 13) &&
		       (sized[5][5] == 6) &&
		       (sized[8][8] == 9) &&
		       // Some modified elements...
		       (sized[0][0] == 9) && // copied from 9
		       (sized[4][9] == 9) && // copied from 9 (on the diag)
		       // Some randomly chosen non-diag elements.
		       (sized[9][6] == 15) &&
		       (sized[2][0] ==  2) &&	
		       (sized[3][9] == 12)
		      ),
		      passed,
		      ERROR_TEXT(failed));    
			 
    //    cout << r << endl;

    cout << "------------------------" << endl;    
    cout << "Creating an empty raster" << endl;
    cout << "------------------------" << endl;    
    irast empty;

    TEST_RESULT_NAMED("to see if the size of an empty raster is 0",
		      (empty.get_numpixels() == 0),
		      passed,
		      ERROR_TEXT(failed));

    TEST_RESULT_NAMED("to see if raster::empty() works",
		      (empty.empty() && !sized.empty()),
		      passed,
		      ERROR_TEXT(failed));

    TEST_RESULT_NAMED("to see if get_width() and get_height() work",
		      (
		       (sized.get_width() == width) &&
		       (sized.get_height() == height) &&
		       (empty.get_width() == 0) &&
		       (empty.get_height() == 0)
		      ),
		      passed,
		      ERROR_TEXT(failed));    
    
    TEST_EXCEPTION_NAMED("to assign outside bounds",
			 empty[0][0] = 100,
			 ERROR_TEXT(failed),
			 passed);

    TEST_EXCEPTION_NAMED("to see if empty rasters forbids indexing",
			 empty[0];,
			 ERROR_TEXT(failed),
			 passed);        

    cout << "---------------------------" << endl;
    cout << "Creating full-raster copies" << endl;
    cout << "---------------------------" << endl;
    const irast copied_cons(sized);

    // This is a modified copy of a test above... Just the name and variable
    // were changed.  
    TEST_RESULT_NAMED("to see if raster's copy constructor worked",
		      (
		       (copied_cons.get_width()  == width) &&
		       (copied_cons.get_height() == height) &&		       
		       // All of the diagonal elements not modified...
		       (copied_cons[1][1] == 1) &&
		       (copied_cons[2][2] == 2) &&
		       (copied_cons[3][3] == 3) &&
		       (copied_cons[6][6] == 6) &&
		       (copied_cons[7][7] == 7) &&
		       (copied_cons[9][9] == 9) &&
		       // Modified diags...
		       (copied_cons[0][0] == 9) &&
		       (copied_cons[4][4] == 13) &&
		       (copied_cons[5][5] == 6) &&
		       (copied_cons[8][8] == 9) &&
		       // Some modified elements...
		       (copied_cons[0][0] == 9) && // copied from 9
		       (copied_cons[4][9] == 9) && // copied from 9 (on the diag)
		       // Some randomly chosen non-diag elements.
		       (copied_cons[9][6] == 15) &&
		       (copied_cons[2][0] ==  2) &&	
		       (copied_cons[3][9] == 12)
		      ),
		      passed,
		      ERROR_TEXT(failed));

    irast copied_assign; copied_assign = sized;
    // This is a modified copy of a test above... Just the name and variable
    // were changed.  
    TEST_RESULT_NAMED("to see if raster's assignment operator worked",
		      (
		       (copied_assign.get_width()  == width) &&
		       (copied_assign.get_height() == height) &&
		       // All of the diagonal elements not modified...
		       (copied_assign[1][1] == 1) &&
		       (copied_assign[2][2] == 2) &&
		       (copied_assign[3][3] == 3) &&
		       (copied_assign[6][6] == 6) &&
		       (copied_assign[7][7] == 7) &&
		       (copied_assign[9][9] == 9) &&
		       // Modified diags...
		       (copied_assign[0][0] == 9) &&
		       (copied_assign[4][4] == 13) &&
		       (copied_assign[5][5] == 6) &&
		       (copied_assign[8][8] == 9) &&
		       // Some modified elements...
		       (copied_assign[0][0] == 9) && // copied from 9
		       (copied_assign[4][9] == 9) && // copied from 9 (on the diag)
		       // Some randomly chosen non-diag elements.
		       (copied_assign[9][6] == 15) &&
		       (copied_assign[2][0] ==  2) &&	
		       (copied_assign[3][9] == 12)
		      ),
		      passed,
		      ERROR_TEXT(failed));    

    TEST_EXCEPTION_NAMED("to see if raster::operator() throws properly",
			 copied_cons(100,100),
			 ERROR_TEXT(failed),
			 passed);
    
    TEST_EXCEPTION_NAMED("to see if raster::operator()const throws properly",
			 copied_assign(100,100),
			 ERROR_TEXT(failed),
			 passed);

    cout << "-----------------------" << endl;
    cout << "Creating sub-rasters..." << endl;
    cout << "-----------------------" << endl;

    irast sub_sized;

    TEST_EXCEPTION_NAMED("to see that sub rasters cannot be created out of range",
			 sub_sized = sized.sub_raster(0,0, 100,100),
			 ERROR_TEXT(failed),
			 passed);
    
    TEST_EXCEPTION_NAMED("to see if sub rasters can be created in proper range",
			 sub_sized = sized.sub_raster(1,1, 8,10),
			 passed,
			 ERROR_TEXT(failed));

    TEST_RESULT_NAMED("to see if the copied raster has a proper size",
		      ((sub_sized.get_width() == 8) &&
		       (sub_sized.get_height() == 10)),
		      passed,
		      ERROR_TEXT(failed));
    
    TEST_EXCEPTION_NAMED("to copy different-sized scanlines (disallowed)",
			 sub_sized[0] = sized[0],
			 ERROR_TEXT(failed),
			 passed);

    cout << "-------------------------------------------------------------------" << endl;
    cout << "Reinterpreting rasters (normally dangerous, if done incorrectly)..." << endl;
    cout << "-------------------------------------------------------------------" << endl;

    typedef manygames::raster<char> crast;

    char* hello_data = "|Hello...|n|.Hello..|n|..Hello.|n|...Hello|n\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    //    cout << hello_data << endl;


    unsigned cw1 = 11, ch1 = 5;
    
    crast hello_rast(cw1, ch1);

    for(unsigned y = 0; y < hello_rast.get_height(); ++y)
    {
      for(unsigned x = 0; x < hello_rast.get_width(); ++x)
      {
	hello_rast[y][x] = hello_data[y * cw1 + x];
      }
    }
    TEST_RESULT_NAMED("to see if reinterpretation gives correct results",
		      (strcmp(hello_data,
			      hello_rast.reinterpret<const char*>()) == 0),
		      passed,
		      ERROR_TEXT(failed));


    cout << "-------------------" << endl;
    cout << "Resizing rasters..." << endl;
    cout << "-------------------" << endl;

    // Get rid of the extra line full of nulls (which was used for comparison).
    hello_rast.resize(hello_rast.get_width(), hello_rast.get_height() - 1);
    

    // shift everything by 1 in both x and y, by way of a resize.
    crast char_rast01 = hello_rast;
    char_rast01.resize(char_rast01.get_width(),
		       char_rast01.get_height(),
		       (char_rast01.get_width() / 2) + 1,
		       (char_rast01.get_height() / 2) - 1,
		       '*');
    TEST_RESULT_NAMED("to see if resizing can do simple shifts",
		      ((hello_rast.get_numpixels() == char_rast01.get_numpixels()) &&
		       (char_rast01[0][0] == '*') &&
		       (char_rast01[3][0] == '*') &&
		       (char_rast01[3][10] == '*') &&
		       (char_rast01[1][6] == hello_rast[2][5]) &&
		       (char_rast01[0][3] == hello_rast[1][2])
		      ),
		      passed,
		      ERROR_TEXT(failed));

    cout << endl;
    cout << "FIXME! Insert more tests for both raster::resize functions here!" << endl;
    cout << endl;
    
    if( !error_count )
    {
      
      cout << "--------------------------------" << endl;    
      cout << "*** All raster tests passed. ***" << endl;
      cout << "--------------------------------" << endl;    
      return 0;
    }
    else
    {
      cout << "------------------------------" << endl;    
      cout << "ERROR: Failed " << error_count << " raster tests." << endl;
      cout << "------------------------------" << endl;
      return(2);
    }
  }
  catch(std::exception& e)
  {
    cerr << "TEST ERROR: An exception leaked out: " << e.what() << endl;
    return(1);
  }
}

//(templated) class raster
