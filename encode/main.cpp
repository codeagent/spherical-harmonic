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
    stbi_ldr_to_hdr_scale(255.0f);

    CliInput cliInput;
    cliInput.addArgument(InputArgument("o", ArgumentType::String, "Output filename path", true));
    cliInput.addArgument(InputArgument("px", ArgumentType::String, "Path to source cubemap POSITIVE X face texture", true, "-"));
    cliInput.addArgument(InputArgument("nx", ArgumentType::String, "Path to source cubemap NEGATIVE X face texture", true));
    cliInput.addArgument(InputArgument("py", ArgumentType::String, "Path to source cubemap POSITIVE Y face texture", true));
    cliInput.addArgument(InputArgument("ny", ArgumentType::String, "Path to source cubemap NEGATIVE Y face texture", true));
    cliInput.addArgument(InputArgument("pz", ArgumentType::String, "Path to source cubemap POSITIVE Z face texture", true));
    cliInput.addArgument(InputArgument("nz", ArgumentType::String, "Path to source cubemap NEGATIVE Z face texture", true));
    cliInput.addArgument(InputArgument("order", ArgumentType::Integer, "The order of spherical harmonics (positive number from 0)", false, "2"));
    cliInput.addArgument(InputArgument("samples", ArgumentType::Integer, "Number of samples to estimate", false, "64"));
    cliInput.addArgument(InputArgument("method", ArgumentType::String, "Algorithm used for estimating spherical harmonics. Possible values: 'spherical' 'monte-carlo' 'cubemap'", false, "monte-carlo"));
    cliInput.addArgument(InputArgument("filtering", ArgumentType::String, "Texture sample filtering, Possible values: 'linear' 'nearest'", false, "linear"));

    string commandLine;
    for (int i = 0; i < argc; i++) {
        commandLine += argv[i] + " "s;
    }

    ArgumentMap arguments = cliInput.parse(commandLine);
    const string output = arguments["o"].value.asString;
    const int order = arguments["order"].value.asInteger;
    const int samples = arguments["samples"].value.asInteger;

    SamplingMethod method;
    if (arguments["method"].value.asString == "monte-carlo"s) {
        method = SamplingMethod::MonteCarlo;
    } else if (arguments["method"].value.asString == "spherical"s) {
        method = SamplingMethod::Sphere;
    } else if (arguments["method"].value.asString == "cubemap"s) {
        method = SamplingMethod::Cubemap;
    } else {
        throw runtime_error("Unknown sampling method: '"s + arguments["method"].value.asString + "'"s);
    }

    InterpolationMethod filtering;
    if (arguments["filtering"].value.asString == "linear"s) {
        filtering = InterpolationMethod::Bilinear;
    } else if (arguments["filtering"].value.asString == "nearest"s) {
        filtering = InterpolationMethod::Nearest;
    } else {
        throw runtime_error("Unknown filtering: '"s + arguments["filtering"].value.asString + "'"s);
    }

    const string px = arguments["px"].value.asString;
    const string nx = arguments["nx"].value.asString;
    const string py = arguments["py"].value.asString;
    const string ny = arguments["ny"].value.asString;
    const string pz = arguments["pz"].value.asString;
    const string nz = arguments["nz"].value.asString;

    auto cubeMap = loadCubemapRgb(px, nx, py, ny, pz, nz);
    ShCoefficients<RGB> shCoefficients = encode<RGB>(cubeMap, (uint16_t) order, method, (uint16_t) samples, filtering);

    write(output, shCoefficients);

    return 0;
}