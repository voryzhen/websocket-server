#pragma once

#include "iostream"
#include "string"
#include "map"
#include <memory>

class cmd_executor {

    std::string name_ = "default command";

    public:

        cmd_executor() {}
        cmd_executor(std::string name) : name_(name) {}
        virtual ~cmd_executor() {}

        virtual void print_cmd_name() {
            std::cout << name_ << std::endl;
        }

        virtual void execute() { print_cmd_name(); }

};

class example_cmd_executor_1 : public cmd_executor { public: example_cmd_executor_1() : cmd_executor("cmd_1") {} };
class example_cmd_executor_2 : public cmd_executor { public: example_cmd_executor_2() : cmd_executor("cmd_2") {} };
class example_cmd_executor_3 : public cmd_executor { public: example_cmd_executor_3() : cmd_executor("cmd_3") {} };

static std::string default_cmd = "cmd_0";

std::map<std::string, std::shared_ptr<cmd_executor>> cmd_class_map = {
    {default_cmd, std::make_unique<cmd_executor>()           },
    {"cmd_1", std::make_unique<example_cmd_executor_1>() },
    {"cmd_2", std::make_unique<example_cmd_executor_2>() },
    {"cmd_3", std::make_unique<example_cmd_executor_3>() }
};

std::shared_ptr<cmd_executor> get_executor(const std::string& cmd) {
    auto executor = cmd_class_map.find(cmd);
    return (executor != cmd_class_map.end())
         ? executor -> second
         : cmd_class_map.find(default_cmd) -> second;
}
