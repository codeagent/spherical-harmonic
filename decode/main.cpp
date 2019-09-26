#include <iostream>
#include <string>
#include <stdexcept>

#include <glm/glm.hpp>

#include <sh.h>

using namespace std;
using namespace sh;
using namespace sh::math;
using namespace sh::input;


int main(int argc, char **argv) {

    stbi_ldr_to_hdr_gamma(1.0f);

    const map<string, FileFormat> FORMAT_LOOKUP = {
            {"png"s, FileFormat::Png},
            {"bmp"s, FileFormat::Bmp},
            {"tga"s, FileFormat::Tga},
            {"jpg"s, FileFormat::Jpg},
            {"hdr"s, FileFormat::Hdr}
    };

    CliInput cliInput;
    cliInput.addArgument(InputArgument("i", ArgumentType::String, "Path to source encoded data", true));
    cliInput.addArgument(InputArgument("o", ArgumentType::String, "Destination output folder", true));
    cliInput.addArgument(InputArgument("format", ArgumentType::String, "Image format to output to. (png, bmp, tga, jpg, hdr)", false, "png"));
    cliInput.addArgument(InputArgument("size", ArgumentType::Integer, "Resolution of generated cubemap images", false, "64"));

    string commandLine;
    for (int i = 0; i < argc; i++) {
        commandLine += argv[i] + " "s;
    }

    ArgumentMap arguments = cliInput.parse(commandLine);
    const string input = arguments["i"].value.asString;
    const string output = arguments["o"].value.asString;


    if (FORMAT_LOOKUP.find(arguments["format"].value.asString) == FORMAT_LOOKUP.end()) {
        throw runtime_error("No such file format found. Available: png, bmp, tga, jpg, hdr");
    }
    const FileFormat format = FORMAT_LOOKUP.at(arguments["format"].value.asString);
    const int size = arguments["size"].value.asInteger;

    const ShCoefficients<RGBFloat> coefficients = readRgb(input);
    auto cubemap = decode(coefficients, size);
    write(output, format, cubemap);

    return 0;
}