#!/bin/sh
# If this is executed, it will automatically create documentation for all of
# the source code in the tree. Assuming, of course, that it is in either
# src/SOMETHING/ or in include/manygames.  This will, however, not limit itself
# to those two directories. 

DIRECTORY=source_documentation

# If the directory doesn't exist...
if [ ! -d "$DIRECTORY" ]
then
  # If it is not a symlink.
  if [ ! -L "$DIRECTORY" ]
  then
    if [ -e "$DIRECTORY" ]
    then
      echo "ERROR: $DIRECTORY exists and is in the way."
      exit 1
    else
      echo "Creating $DIRECTORY to store generated pages."
      mkdir "$DIRECTORY"
    fi
  fi
fi

if [ ! -w "$DIRECTORY" ]
then
  echo "Cannot write files in $DIRECTORY"
  exit 1
fi

if [ ! -x "$DIRECTORY" ]
then
  echo "Cannot change to $DIRECTORY"
  exit 1
fi

DATABASE=ccdoc.db

# If the database exist, delete it.  This is to work around a bug in ccdoc
if [ -e "$DIRECTORY/$DATABASE" ]
then
  echo "Removing $DIRECTORY/$DATABASE"
  rm "$DIRECTORY/$DATABASE"
fi

cd "$DIRECTORY"

# Generate documentation using ccdoc.
ccdoc -db $DATABASE                      \
      -pkg manygames                     \
      ../../*/*/*.hpp                    \
      ../../*/*/*.h                      \
      -index                             \
      -rootfile ./index.html             \
      -html ./                  
