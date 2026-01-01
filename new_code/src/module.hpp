#pragma once
#include <string>

class BuildModule {
public:
    virtual ~BuildModule() = default;
    virtual std::string name() const = 0;
    virtual void run(const std::string& rootfs) = 0;
};