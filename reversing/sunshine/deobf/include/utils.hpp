#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include <stdio.h>
#include <stdarg.h>
#include <exception>
#include <stdexcept>
#include <stdint.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <memory.h>

using namespace std;

class Utils
{
public:

    // Format a message and use it to throw a std::runtime_error.
    static void throwf( const char *fmt, ... ) {
        char 	buffer[0xFFFF] = {0};
        va_list ap;

        va_start( ap, fmt );
        vsnprintf( buffer, 0xFFFF, fmt, ap );
        va_end(ap);

        throw std::runtime_error(buffer);
    }
};

#endif
