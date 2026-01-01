#include "../module.hpp"
#include "../builder.hpp"

class CleanupModule : public BuildModule {
public:
    std::string name() const override {
        return "cleanup";
    }

    void run(const std::string& rootfs) override {
        DistroBuilder::run("umount -lf " + rootfs + "/proc || true");
        DistroBuilder::run("umount -lf " + rootfs + "/sys || true");
        DistroBuilder::run("umount -lf " + rootfs + "/dev || true");
    }
};

std::unique_ptr<BuildModule> createCleanupModule() {
    return std::make_unique<CleanupModule>();
}

