#include "builder.hpp"
#include "module.hpp"

#include "utils/config_parser.hpp"

#include "modules/ubuntu/ubuntu_config_module.hpp"

#include <memory>

// factories
std::unique_ptr<BuildModule> createRootfsModule();
std::unique_ptr<BuildModule> createUbuntuBootstrapModule();
std::unique_ptr<BuildModule> createChrootMountsModule();
std::unique_ptr<BuildModule> createUbuntuConfigModule();
std::unique_ptr<BuildModule> createUbuntuPackagesModule();
std::unique_ptr<BuildModule> createCleanupModule();

int main() {

    VereConfigParser cfg;
    if (!cfg.load("distro.conf")) {
        return 1;
    }

    // Flame, the `rootfs` is the ouput
    // folder. Not the final name.
    DistroBuilder builder("rootfs");

    builder.addModule(createRootfsModule());
    builder.addModule(createUbuntuBootstrapModule());
    builder.addModule(createChrootMountsModule());
    builder.addModule(createUbuntuConfigModule(cfg));
    builder.addModule(createUbuntuPackagesModule());
    builder.addModule(createCleanupModule());

    builder.build();
    return 0;
}
