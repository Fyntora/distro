#include "../module.hpp"
#include <iostream>
#include <cstdlib>
#include <memory>


class UbuntuBootstrapModule : public BuildModule {
public:
    std::string name() const override {
        return "ubuntu-bootstrap";
    }

    void run(const std::string& rootfs) override {
        std::cout << "    Bootstrapping Ubuntu rootfs\n";

        std::string cmd =
            "debootstrap "
            "--arch=amd64 "
            "jammy "            // 22.04 LTS
            + rootfs +
            " http://archive.ubuntu.com/ubuntu/";

        if (std::system(cmd.c_str()) != 0) {
            std::cerr << "debootstrap failed\n";
            std::exit(1);
        }
    }
};

std::unique_ptr<BuildModule> createUbuntuModule() {
    return std::make_unique<UbuntuBootstrapModule>();
}
