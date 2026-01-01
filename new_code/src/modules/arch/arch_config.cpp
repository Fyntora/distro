#include "../../module.hpp"
#include <fstream>
#include <memory>

class ArchConfigModule : public BuildModule {
public:
    std::string name() const override {
        return "arch-config";
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

std::unique_ptr<BuildModule> createArchConfigModule() {
    return std::make_unique<ArchConfigModule>();
}

