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

using namespace std;

enum ArgumentType {
    Integer,
    Float,
    String,
    Boolean
};

struct InputArgument {
    string name;
    ArgumentType type;
    string defaultValue;
    string description;
    bool required;

    InputArgument() = default;
    InputArgument(string name, ArgumentType type, string description = "", bool required = false, string defaultValue = "") :
            name(name), type(type), defaultValue(defaultValue), description(description), required(required) {}
};


struct ArgumentValue {
    ArgumentType type;
    union {
        int asInteger;
        float asFloat;
        char asString[256];
        bool asBoolean;
    } value;
};

using ArgumentMap = map<string, ArgumentValue>;

class CliInput {
protected:
    vector<InputArgument> arguments;
public:

    ArgumentMap parse(const string &commandArgs) {
        using namespace std;
        const regex attributesRegExpr(R"/(--?[a-zA-Z][a-zA-Z0-9-]*(?!\s+--?)(\s*=?(['"])[^'"]+\2))/");
        const regex attributeRegExpr(R"/(--?([a-zA-Z][a-zA-Z0-9-]*)(?!\s+--?)(\s*=?(['"])([^'"]+)\3))/");

        const regex flagsRegExpr(R"/(--?[a-zA-Z][a-zA-Z0-9-]*(?=\s+--?|$))/");
        const regex flagRegExpr(R"/(--?([a-zA-Z0-9-]+))/");

        map<string, string> args;
        smatch matches;
        string subject = commandArgs;
        while (std::regex_search (subject, matches, attributesRegExpr)) {
            string attrib = matches[0];
            smatch aMatches;
            if (std::regex_match (attrib, aMatches, attributeRegExpr)) {
                const string name = aMatches[1];
                const string value = aMatches[4];
                args[name] = value;
            }
            subject = matches.suffix().str();
        }

        subject = commandArgs;
        while (std::regex_search (subject, matches, flagsRegExpr)) {
            string flag = matches[0];
            smatch fMatches;
            if (std::regex_match (flag, fMatches, flagRegExpr)) {
                const string name = fMatches[1];
                const string value = "true";
                args[name] = value;
            }
            subject = matches.suffix().str();
        }


        ArgumentMap argumentMap;
        for (auto &a : arguments) {
            if (a.required && args.find(a.name) == args.end()) {
                throw runtime_error("Input: argument '" + a.name + "' required");
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

#endif //SH_CLIINPUT_H
