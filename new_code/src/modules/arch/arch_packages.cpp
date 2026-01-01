#include "../../module.hpp"
#include "../../builder.hpp"

class ArchPackagesModule : public BuildModule {
public:
    std::string name() const override {
        return "arch-packages";
    }

    void run(const std::string& rootfs) override {
        // Flame, here we essentially execute the following:
        // `chroot rootfs` and then we execute the commands.
        // They're defined within the run thingy. //ok
        std::string chroot = "chroot " + rootfs + " ";

        DistroBuilder::run(chroot + "pacman -Syu --noconfirm");
        DistroBuilder::run(
            chroot +
            "pacman -S --noconfirm systemd-sysv sudo netplan networkmanager plasma-meta"
            "netplan.io network-manager"
        );
    }
};

std::unique_ptr<BuildModule> createArchPackagesModule() {
    return std::make_unique<ArchPackagesModule>();
}