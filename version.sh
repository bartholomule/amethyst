#!/bin/sh
# This is a simple script to extract the version from the configure script.

./configure --version | head -n1 | cut -f 3 -d' '
