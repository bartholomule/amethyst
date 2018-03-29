#include "general/fd_stream.hpp"
#include "general/string_format.hpp"
#include "general/base_logger.hpp"

#include "test_framework/testinclude.hpp"

#include <iostream>
#include <ostream>
#include <fcntl.h>

struct barf
{
    barf(int val) : value(val) {
    }
    int value;
};

std::ostream& operator<<(std::ostream& o, const barf& b)
{
    o << "This is barf: " << b.value;
    return o;
}

int main(int argc, const char** argv)
{
    using std::cout;
    using std::endl;
    int error_count = 0;

    amethyst::fd_ostream foo;
    foo << "Hello." << std::endl;

    amethyst::fd_ostream bar(amethyst::get_std_handle(2));
    foo << "Goodbye." << std::endl;

    std::shared_ptr<barf> b(new barf(10));
    foo << b << std::endl;

    std::cout << "opening baz.txt" << std::endl;
#if defined(WINDOWS)
    HANDLE baz = CreateFile("baz.txt", GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
#else
    int baz = open("baz.txt", O_RDWR | O_CREAT, 0644);
#endif
    amethyst::fd_ostream bazzer(baz);
    bazzer << "Testing..." << std::endl;
#if defined(WINDOWS)
    CloseHandle(baz);
#else
    close(baz);
#endif
    std::cout << amethyst::string_format("closing baz.txt (%1)", baz) << std::endl;


    //	amethyst::logger logger(amethyst::log_formatter::create_log_formatter("test \"%m\"\\nHere's a percent: '%%'\nHere's a tab: '\\t'\nHere's an embedded null: '\\x0'\nAnd a space '\\x30'"), amethyst::stream_stdout);
    amethyst::logger logger(amethyst::log_formatter::create_log_formatter("date(%d{%H:%M:%S}) file(%F) file+line(%l) line(%L) method(%M) message(%/30.40m) runtime(%r) threadid(%t) pid(%P) percent(%%) \\xl3"), amethyst::stream_stdout);
    logger.log("Hello from a logger.");

    if (!error_count)
    {

        cout << "----------------------------------" << endl;
        cout << "*** All fd_stream tests passed. ***" << endl;
        cout << "----------------------------------" << endl;
        return 0;
    }
    else
    {
        cout << "---------------------------------" << endl;
        cout << "ERROR: Failed " << error_count << " fd_stream tests." << endl;
        cout << "---------------------------------" << endl;
        return 2;
    }
}

// class fd_stream
