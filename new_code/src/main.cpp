#include "builder.hpp"
#include "module.hpp"
#include <memory>

// factories
std::unique_ptr<BuildModule> createRootfsModule();
std::unique_ptr<BuildModule> createUbuntuBootstrapModule();
std::unique_ptr<BuildModule> createChrootMountsModule();
std::unique_ptr<BuildModule> createUbuntuConfigModule();
std::unique_ptr<BuildModule> createUbuntuPackagesModule();
std::unique_ptr<BuildModule> createCleanupModule();

int main() {
    // Flame, the `rootfs` is the ouput
    // folder. Not the final name.
    DistroBuilder builder("rootfs");

    builder.addModule(createRootfsModule());
    builder.addModule(createUbuntuBootstrapModule());
    builder.addModule(createChrootMountsModule());
    builder.addModule(createUbuntuConfigModule());
    builder.addModule(createUbuntuPackagesModule());
    builder.addModule(createCleanupModule());

    builder.build();
    return 0;
}
