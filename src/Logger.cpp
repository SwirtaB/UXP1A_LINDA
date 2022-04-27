#include "Logger.hpp"

#include "LindaTuple.hpp"

#include <bits/stdc++.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <ratio>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <variant>
namespace linda
{

void Logger::openFile() {
    if (-1 == mkdir("logFiles", 0777) && errno != EEXIST) {
        std::cerr << "Error :  " << std::strerror(errno) << std::endl;
        exit(1);
    }
    if (chdir("logFiles") == -1) {
        std::cerr << "Error :  " << std::strerror(errno) << std::endl;
        exit(1);
    }
    log_file_.open(dateNow(), std::ofstream::out | std::ofstream::app);
}
std::ofstream &Logger::log() {
    log_file_ << dateNow() << " " << timeNow() << " [INFO] ";
    return log_file_;
}
std::string Logger::dateNow() {
    time_t      now = time(0);
    tm         *ltm = localtime(&now);
    std::string result;

    result += std::to_string(1900 + ltm->tm_year) + "-";
    result += std::to_string(1 + ltm->tm_mon) + "-";
    result += std::to_string(ltm->tm_mday);
    return result;
}
std::string Logger::timeNow() {
    time_t      now = time(0);
    tm         *ltm = localtime(&now);
    std::string result;

    result += std::to_string(ltm->tm_hour) + ":";
    result += std::to_string(ltm->tm_min) + ":";
    result += std::to_string(ltm->tm_sec) + ":";
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
                  .count();
    result += std::to_string(ms % 1000);
    return result;
}
std::string Logger::toString(const std::vector<TupleValue> values) {
    std::string result;
    for (auto value : values) {
        if (std::holds_alternative<std::string>(value))
            result += std::get<std::string>(value) + "| ";
        else if (std::holds_alternative<int>(value))
            result += std::to_string(std::get<int>(value)) + "| ";
        else if (std::holds_alternative<float>(value))
            result += std::to_string(std::get<float>(value)) + "| ";
        else
            throw std::runtime_error("Logger::toString[TupleValue] - variant check not exhaustive");
    }
    return result;
}

std::string Logger::toString(const std::vector<TupleRequirement> &requirements_) {
    std::string result;
    for (auto requirement : requirements_) {
        if (requirement.has_value()) {
            if (requirement.value().first == RequirementTypeSerializable::Any)
                result += "Any ";
            else if (requirement.value().first == RequirementTypeSerializable::Eq)
                result += "Eq " + to_string(requirement.value().second) + "| ";
            else if (requirement.value().first == RequirementTypeSerializable::Less)
                result += "Less " + to_string(requirement.value().second) + "| ";
            else if (requirement.value().first == RequirementTypeSerializable::LessEq)
                result += "LessEq " + to_string(requirement.value().second) + "| ";
            else if (requirement.value().first == RequirementTypeSerializable::More)
                result += "More " + to_string(requirement.value().second) + "| ";
            else if (requirement.value().first == RequirementTypeSerializable::MoreEq)
                result += "MoreEq " + to_string(requirement.value().second) + "| ";
            else
                throw std::runtime_error("Logger::toString[TupleRequirement] - variant check not exhaustive");
        }
    }
    return result;
}

std::string Logger::to_string(linda::TupleValue value) {
    if (std::holds_alternative<std::string>(value))
        return std::get<std::string>(value) + " ";
    else if (std::holds_alternative<int>(value))
        return std::to_string(std::get<int>(value)) + " ";
    else if (std::holds_alternative<float>(value))
        return std::to_string(std::get<float>(value)) + " ";
    else
        throw std::runtime_error("TuplePattern::to_string - variant check not exhaustive");
}

} // namespace linda
