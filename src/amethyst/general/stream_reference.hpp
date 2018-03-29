#include <ostream>
#include <istream>
#include <iostream>
#include <memory>

namespace amethyst
{
    typedef std::shared_ptr<std::ostream> output_stream_ref;
    extern output_stream_ref stream_stdout;
    extern output_stream_ref stream_stderr;

    // Anything that follows is not complete, and as such cannot be used.
    typedef std::shared_ptr<std::istream> input_stream_ref;
    extern input_stream_ref stream_stdin;

    typedef std::shared_ptr<std::iostream> io_stream_ref;
}
