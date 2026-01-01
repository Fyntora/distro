#pragma once
#include <vector>
#include <memory>
#include <string>

class BuildModule;

class DistroBuilder {
public:
    explicit DistroBuilder(std::string root);
    void addModule(std::unique_ptr<BuildModule> module);
    void build();

    static void run(const std::string& cmd);
};
