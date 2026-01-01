#include "../../module.hpp"
#include "../../builder.hpp"


class ArchBootstrapModule : public BuildModule {
public:
    std::string name() const override {
        return "arch-bootstrap";
    }

    void run(const std::string& rootfs) override {
        DistroBuilder::run(
            "pacstrap -c " +
            rootfs +
            " base linux linux-firmware"
        );
    }
};

std::unique_ptr<BuildModule> createArchBootstrapModule() {
    return std::make_unique<ArchBootstrapModule>();
}