#include "../../module.hpp"
#include "../utils/config_parser.hpp"
#include "./ubuntu_config_module.hpp"


#include <fstream>
#include <memory>
#include <iostream>

class UbuntuConfigModule : public BuildModule {
public:
    UbuntuConfigModule(const VereConfigParser& cfg) : cfg(cfg) {}

    std::string name() const override {
        return "ubuntu-config";
    }

    void run(const std::string& rootfs) override {
        std::string os_name = cfg.get("os_name", "VereLinux");
        std::string os_id = cfg.get("os_id", "verelinux");
        std::string os_version = cfg.get("os_version", "1.0");
        std::string pretty_name = cfg.get("os_nick", "VereLinux");

        std::ofstream os(rootfs + "/etc/os-release");
        os << "NAME=\"" << os_name << "\"\n"
           << "ID=" << os_id << "\n"
           << "VERSION_ID=\"" << os_version << "\"\n"
           << "PRETTY_NAME=\"" << pretty_name << "\"\n";
        os.close();

        std::ofstream hn(rootfs + "/etc/hostname");
        hn << os_id << "\n";
        hn.close();

        std::cout << "[✓] Ubuntu config written\n";
    }

private:
    const VereConfigParser& cfg;
};

// Factory
std::unique_ptr<BuildModule> createUbuntuConfigModule(const VereConfigParser& cfg) {
    return std::make_unique<UbuntuConfigModule>(cfg);
}