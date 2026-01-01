#include "../module.hpp"
#include "../builder.hpp"

class BusyboxModule : public BuildModule {
public:
    std::string name() const override {
        return "busybox";
    }

    void run(const std::string& rootfs) override {
        std::system(("cp /bin/busybox " + rootfs + "/bin/").c_str());
        std::system(("chroot " + rootfs + " /bin/busybox --install -s").c_str());
    }
};

std::unique_ptr<BuildModule> createBusyboxModule() {
    return std::make_unique<BusyboxModule>();
}