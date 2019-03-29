#include <stdint.h>
#include <unistd.h>

#include <iostream>
#include <fstream>
#include <string>

#include <cstdlib>

namespace util
{

inline std::string loadProgram(std::string input)
{
    std::ifstream stream(input.c_str());
    if (!stream.is_open())
    {
        std::cout << "Cannot open file: " << input << std::endl;
        exit(1);
    }

    return std::string(
        std::istreambuf_iterator<char>(stream),
        (std::istreambuf_iterator<char>()));
}

} // namespace util
