#!/bin/sh
# $Id: do_all_tests.sh,v 1.1 2003/09/16 09:04:06 kpharris Exp $
# This is a script to perform the tests which are provided in this directory.
# To be displayed correctly, the last two lines of each test source file must
# contain a C++ comment (description), and a newline. If this is not present,
# at least a blank line is required.


# Variable to hold if everything worked or not (0 -- success)
failed_something=0
missing_stuff=0
quiet=0

while [ $# -ge 1 ]
do
  # Check to see if a quiet parameter '-q' or '--quiet' was given.
  if [ "$1" = "-q" ] || [ "$1" = "--quiet" ]
  then
    # Set a flag for later use.
    quiet=1
  elif [ "$1" = "-h" ] || [ "$1" = "--help" ]
  then
cat <<EOF
  usage: do_all_tests.sh [flags]
   where flags can be:
   -h or --help     -- Display this help text.
   -q or --quiet    -- Be much more quiet when executing.
   -v or --version  -- Display version/license information.
EOF
    exit 0
  elif [ "$1" = "-v" ] || [ "$1" = "--version" ]
  then
cat <<EOF
  do_all_tests.sh automated test execution script
  Version \$Revision: 1.1 $
  Copyright (C) 2003 Kevin Harris

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version. 

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA 

EOF
    exit 0
  else
    echo "  Unknown flag '$1'. Use -h or --help for help."
    exit 1
  fi
  shift
done

echo "--------------------------------------------"
echo "|  Beginning tests for project 'manygames' |"
echo "--------------------------------------------"

for test_cpp in *.cpp
do
  # Extract an executable name
  test_name=`basename $test_cpp .cpp`
  # Cut the description out of the cpp file.
  description=`tail -n1 $test_cpp | cut -c3-`

  # Fix to *potentially* allow it to run the test in windows...
  if [ -e $test_name.exe ]
  then
    test_name=$test_name.exe
  fi

  # If the executable exists... Display junk and run it.
  if [ -x $test_name ]
  then
    # If we have a description, print more descriptive stuff.
    if [ -n "$description" ]
    then
      echo "Performing test $test_name: $description"
    else
      echo "Performing test $test_name"
    fi
    # Handle execution, redirecting output if a quiet flag was given.
    if [ $quiet -eq 0 ]
    then
      $test_name
    else
      $test_name > /dev/null 2>/dev/null
    fi

    # Check the return code, printing a success or failure.
    if [ $? -ne 0 ]
    then 
      failed_something=1
      echo "-------------------------------------------"
      echo " Test $test_name failed!"
      echo "-------------------------------------------"
    else
      echo "-------------------------------------------"
      echo " Test $test_name succeeded."
      echo "-------------------------------------------"
    fi
  else
    # The executable was not there... Print a warning and set a flag.
    echo "*** Skipping test \"$test_name\", executable missing"
    missing_stuff=1
  fi
done


# Check to see if any tests were failed, printing out a useful message.
if [ $failed_something -eq 0 ]
then
  echo "--------------------------------------------"
  echo "|           All tests passed!              |"
  echo "--------------------------------------------"
else
  echo "--------------------------------------------"
  echo "|   WARNING: One or more tests failed!     |"
  echo "--------------------------------------------"
fi

# Check to see if any tests (executables) were missing.
if [ $missing_stuff -ne 0 ]
then
  echo "| WARNING: One or more tests were skipped! |"
  echo "--------------------------------------------"
fi

exit $failed_something