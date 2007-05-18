#! /bin/sh

set -e

rm -rf config.cache ltmain.sh config.guess config.sub depcomp missing mkinstalldirs autom4te.cache

run_cmd()
{
  echo "Running $1..."
  "$@"
}

run_cmd aclocal
run_cmd autoconf
run_cmd autoheader
run_cmd libtoolize
run_cmd automake --add-missing --copy --gnu
