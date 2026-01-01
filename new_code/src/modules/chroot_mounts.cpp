#include "../module.hpp"
#include "../builder.hpp"

class ChrootMountsModule : public BuildModule {
public:
    std::string name() const override {
        return "chroot-mounts";
    }

    void run(const std::string& rootfs) override {
        DistroBuilder::run("mount --bind /dev " + rootfs + "/dev");
        DistroBuilder::run("mount -t proc proc " + rootfs + "/proc");
        DistroBuilder::run("mount -t sysfs sys " + rootfs + "/sys");
    }
};

std::unique_ptr<BuildModule> createChrootMountsModule() {
    return std::make_unique<ChrootMountsModule>();
}