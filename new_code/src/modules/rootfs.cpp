#include "../module.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

class RootfsModule : public BuildModule {
public:
    std::string name() const override {
        return "rootfs";
    }

    void run(const std::string& rootfs) override {
        std::cout << "    Creating rootfs\n";
        fs::create_directories(rootfs + "/bin");
        fs::create_directories(rootfs + "/etc");
        fs::create_directories(rootfs + "/proc");
        fs::create_directories(rootfs + "/sys");
        fs::create_directories(rootfs + "/usr");
    }
};

// factory helper
std::unique_ptr<BuildModule> createRootfsModule() {
    return std::make_unique<RootfsModule>();
}
