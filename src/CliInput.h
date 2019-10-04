//
// Created by Alex on 18.09.2019.
//

#ifndef SH_CLIINPUT_H
#define SH_CLIINPUT_H

#pragma once

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace sh {
    namespace input {

        enum ArgumentType {
            Integer,
            Float,
            String,
            Boolean
        };

        struct InputArgument {
            std::string name;
            ArgumentType type;
            std::string defaultValue;
            std::string description;
            bool required;

            InputArgument() = default;
            InputArgument(const std::string &name, ArgumentType type, const std::string &description = "",
                    bool required = false,
                    const std::string &defaultValue = "") :
                    name(name), type(type), defaultValue(defaultValue), description(description), required(required) {}
        };

        struct ArgumentValue {
            ArgumentType type;
            union {
                int asInteger;
                float asFloat;
                char asString[1024];
                bool asBoolean;
            } value;
        };

        using ArgumentMap = std::map<std::string, ArgumentValue>;

        class CliInput {
        protected:
            std::vector<InputArgument> arguments;
        public:
            std::string brief(const int padding = 8, const int perLine = 4) {
                using namespace std;
                ostringstream stream;
                auto c = 1;
                stream << setfill(' ');
                for (auto &argument: this->arguments) {
                    if (!argument.required) {
                        stream << "[";
                    }
                    stream << "--" << argument.name;
                    if (argument.type == ArgumentType::String) {
                        stream << "=<string>";
                    } else if (argument.type == ArgumentType::Integer || argument.type == ArgumentType::Float) {
                        stream << "=<number>";
                    }
                    if (!argument.required) {
                        stream << "]";
                    }
                    if (c % perLine == 0) {
                        stream << endl;
                        stream << setw(padding);
                    } else {
                        stream << " ";
                    }
                    c++;
                }
                stream << endl;
                return stream.str();
            }

            std::string summary() {
                using namespace std;
                ostringstream stream;

                const auto widest = std::max_element(arguments.begin(), arguments.end(), [](const InputArgument &a,
                        const InputArgument &b) {
                    return a.name.length() < b.name.length();
                });

                auto c = 0;
                const auto alignment = 4;
                const auto length = widest->name.length();
                const auto padding = (uint32_t) (length % alignment == 0 ? length + alignment :
                                                     (widest->name.length() / alignment + 1) * alignment);

                stream << std::left << std::setfill(' ');
                for (auto &argument: this->arguments) {
                    stream << setw(padding) << argument.name << argument.description << endl;
                    stream << setw(padding);
                    if (argument.type == ArgumentType::String) {
                        stream << ' ' << "Type: String";
                    } else if (argument.type == ArgumentType::Integer) {
                        stream << ' ' <<  "Type: Integer";
                    } else if (argument.type == ArgumentType::Float) {
                        stream << ' ' <<  "Type: Float";
                    } else {
                        stream << ' ' <<  "Type: Boolean";
                    }
                    stream << endl;
                    if (!argument.defaultValue.empty()) {
                        stream << setw(padding);
                        stream << ' ' << "Default: " << argument.defaultValue;
                        stream << endl;
                    }
                    if (argument.required) {
                        stream << setw(padding);
                        stream << ' ' << "Required";
                        stream << endl;
                    }
                    stream << endl;
                }

                return stream.str();
            }

            ArgumentMap parse(const std::string &commandArgs) {
                using namespace std;
                const regex attributesRegExpr(R"/(--?[a-zA-Z][a-zA-Z0-9-]*(?!\s+--?)(\s*=?(['"])[^'"]+\2))/");
                const regex attributeRegExpr(R"/(--?([a-zA-Z][a-zA-Z0-9-]*)(?!\s+--?)(\s*=?(['"])([^'"]+)\3))/");

                const regex flagsRegExpr(R"/(--?[a-zA-Z][a-zA-Z0-9-]*(?=\s+--?|\s*$))/");
                const regex flagRegExpr(R"/(--?([a-zA-Z0-9-]+))/");

                map<string, string> args;
                smatch matches;
                string subject = commandArgs;
                while (std::regex_search(subject, matches, attributesRegExpr)) {
                    string attrib = matches[0];
                    smatch aMatches;
                    if (std::regex_match(attrib, aMatches, attributeRegExpr)) {
                        const string name = aMatches[1];
                        const string value = aMatches[4];
                        args[name] = value;
                    }
                    subject = matches.suffix().str();
                }

                subject = commandArgs;
                while (std::regex_search(subject, matches, flagsRegExpr)) {
                    string flag = matches[0];
                    smatch fMatches;
                    if (std::regex_match(flag, fMatches, flagRegExpr)) {
                        const string name = fMatches[1];
                        const string value = "true";
                        args[name] = value;
                    }
                    subject = matches.suffix().str();
                }


                ArgumentMap argumentMap;
                for (auto &a : arguments) {
                    if (a.required && args.find(a.name) == args.end()) {
                        throw string("Input: argument '" + a.name + "' required");
                    }
                    const string strValue = args.find(a.name) != args.end() ? args[a.name] : a.defaultValue;
                    ArgumentValue argumentValue{};
                    argumentValue.type = a.type;
                    if (a.type == ArgumentType::Integer) {
                        argumentValue.value.asInteger = std::stoi(strValue);
                    } else if (a.type == ArgumentType::Float) {
                        argumentValue.value.asFloat = std::stof(strValue);
                    } else if (a.type == ArgumentType::Boolean) {
                        argumentValue.value.asBoolean = regex_match(strValue, regex("true"));
                    } else {
                        strcpy(argumentValue.value.asString, strValue.c_str());
                    }
                    argumentMap[a.name] = argumentValue;
                }
                return argumentMap;
            }

            void addArgument(const InputArgument &argument) {
                arguments.push_back(argument);
            }
        };
    }
}

#endif //SH_CLIINPUT_H
