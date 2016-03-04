////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     StringFormat.tcc (utilities)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <iostream>
#include <iomanip>
#include <sstream>

namespace utilities
{
    template<typename ArgType, typename ... ArgTypes>
    Format::Result Format::Printf(std::ostream& os, const char* format, ArgType arg, ArgTypes ...args)
    {
        int n=0;
        const char* ptr = format;
        while(*ptr != '%' && *ptr != '\0')
        {
            ++n;
            ++ptr;
        }

        os.write(format, n);

        // if reached end of string, exit
        if(*ptr == '\0')
        {
            return Result::earlyEndOfFormat;
        }

        // if reached '%' character, print an arg
        char specifier = *(++ptr);
        auto precision = os.precision();
        auto flags = os.flags();

        switch(specifier)
        {
        case 'i':
            os.precision(0);
            os.flags(std::ios::fixed);
            os << arg;
            os.flags(flags);
            os.precision(precision);
            break;

        case 'f':
            os.flags(std::ios::fixed);
            os << arg;
            os.flags(flags);
            break;

        case 'e':
            os.flags(std::ios::scientific);
            os << arg;
            os.flags(flags);
            break;

        case 's':
            os << arg;
            break;

        case '%':
            os << '%';
            break;
        }

        // if end of string reached, exit
        ++ptr;
        return Format::Printf(os, ptr, args...);
    }

    template<typename ... ArgTypes>
    Format::Result Format::Printf(std::ostream& os, const char* format, ArgTypes ...args)
    {
        if(*format != '\0')
        {
            os << format;
        }
        return Result::success;
    }

    template<typename ... ArgTypes>
    std::string Format::Printf(const char* format, ArgTypes ...args)
    {
        std::stringstream ss;
        Format::Printf(ss, format, args...);
        return ss.str();
    }
}