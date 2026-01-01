#include "../../module.hpp"
#include <iostream>
#include <cstdlib>
#include <memory>

class ArchBootstrapModule : public BuildModule {
public:
    std::string name() const override {
        return "arch-bootstrap";
    }

    void run(const std::string& rootfs) override {
        std::cout << "    Bootstrapping Arch rootfs\n";

        std::string cmd =
            "pacstrap -c "
            + rootfs +
            " base linux linux-firmware";

        if (std::system(cmd.c_str()) != 0) {
            std::cerr << "pacstrap failed\n";
            std::exit(1);
        }
    }
};

std::unique_ptr<BuildModule> createArchModule() {
    return std::make_unique<ArchBootstrapModule>();
}
