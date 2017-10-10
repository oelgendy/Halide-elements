#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <climits>

#include "HalideRuntime.h"
#include "HalideBuffer.h"

#include "div_scalar.h"

using std::string;
using std::vector;
using Halide::Runtime::Buffer;

namespace {

template<typename... Rest>
string format(const char *fmt, const Rest&... rest)
{
    int length = snprintf(NULL, 0, fmt, rest...) + 1; // Explicit place for null termination
    vector<char> buf(length, 0);
    snprintf(&buf[0], length, fmt, rest...);
    string s(buf.begin(), std::find(buf.begin(), buf.end(), '\0'));
    return s;
}

}

int main()
{
    try {
        int ret = 0;
        
        //
        // Run
        //
        const int width = 1024;
        const int height = 768;
	const float value = 2.0;
        Buffer<uint8_t> input(width, height);
        Buffer<uint8_t> output(width, height);
        for (int y=0; y<height; ++y) {
            for (int x=0; x<width; ++x) {
                input(x, y) = (uint8_t)(x + y);
                output(x, y) = 0;
            }
        }

        div_scalar(input, value, output);

        for (int y=0; y<height; ++y) {
            for (int x=0; x<width; ++x) {
                uint8_t expect = input(x, y);
                uint8_t actual = output(x, y);
		float f = expect / value;
		if (f > (float)UCHAR_MAX) {
		    f = (float)UCHAR_MAX;
		}
		expect = (uint8_t)(f + 1.0f) & ~0x1;
                if (expect != actual) {
                    throw std::runtime_error(format("Error: expect(%d, %d) = %d, actual(%d, %d) = %d", x, y, expect, x, y, actual).c_str());
                }
            }
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    printf("Success!\n");
    return 0;
}
