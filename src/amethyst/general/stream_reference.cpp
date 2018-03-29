#include "stream_reference.hpp"
#include "fd_stream.hpp"

namespace amethyst
{
    output_stream_ref stream_stdout(new fd_ostream(get_std_handle(1)));
    output_stream_ref stream_stderr(new fd_ostream(get_std_handle(2)));
}

