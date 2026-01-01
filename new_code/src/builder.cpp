#include "builder.hpp"
#include "module.hpp"
#include <iostream>
#include <cstdlib>

DistroBuilder::DistroBuilder(std::string root)
    : rootfs(std::move(root)) {}

void DistroBuilder::addModule(std::unique_ptr<BuildModule> module) {
    modules.push_back(std::move(module));
}

void DistroBuilder::run(const std::string& cmd) {
    std::cout << "[>] " << cmd << "\n";
    if (std::system(cmd.c_str()) != 0) {
        std::cerr << "[!] Command failed\n";
        std::exit(1);
    }
}

void DistroBuilder::build() {
    for (auto& m : modules) {
        std::cout << "[+] " << m->name() << "\n";
        m->run(rootfs);
    }
}