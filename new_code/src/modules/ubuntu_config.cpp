#include "../module.hpp"
#include <fstream>
#include <memory>

class UbuntuConfigModule : public BuildModule {
public:
    std::string name() const override {
        return "ubuntu-config";
    }

    void run(const std::string& rootfs) override {
        std::ofstream os(rootfs + "/etc/os-release");
        os <<
            "NAME=\"VereLinux\"\n"
            "ID=verelinux\n"
            "VERSION_ID=\"1.0\"\n"
            "PRETTY_NAME=\"VereLinux\"\n";
        os.close();

        std::ofstream hn(rootfs + "/etc/hostname");
        hn << "verelinux\n";
        hn.close();
    }
};

std::unique_ptr<BuildModule> createUbuntuConfigModule() {
    return std::make_unique<UbuntuConfigModule>();
}

