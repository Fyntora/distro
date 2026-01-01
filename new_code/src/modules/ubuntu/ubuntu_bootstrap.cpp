#include "../../module.hpp"
#include "../../builder.hpp"


class UbuntuBootstrapModule : public BuildModule {
public:
    std::string name() const override {
        return "ubuntu-bootstrap";
    }

    void run(const std::string& rootfs) override {
        DistroBuilder::run(
            "debootstrap --arch=amd64 jammy " +
            rootfs +
            " http://archive.ubuntu.com/ubuntu/"
        );
    }
};

std::unique_ptr<BuildModule> createUbuntuBootstrapModule() {
    return std::make_unique<UbuntuBootstrapModule>();
}