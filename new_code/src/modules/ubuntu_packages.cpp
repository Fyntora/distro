#include "../module.hpp"
#include "../builder.hpp"

class UbuntuPackagesModule : public BuildModule {
public:
    std::string name() const override {
        return "ubuntu-packages";
    }

    void run(const std::string& rootfs) override {
        // Flame, here we essentially execute the following:
        // `chroot rootfs` and then we execute the commands.
        // They're defined within the run thingy.
        std::string chroot = "chroot " + rootfs + " ";

        DistroBuilder::run(chroot + "apt update");
        DistroBuilder::run(
            chroot +
            "apt install -y systemd-sysv sudo "
            "netplan.io network-manager"
        );
    }
};

std::unique_ptr<BuildModule> createUbuntuPackagesModule() {
    return std::make_unique<UbuntuPackagesModule>();
}