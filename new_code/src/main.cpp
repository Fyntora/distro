#include "builder.hpp"
#include "module.hpp"
#include <memory>

std::unique_ptr<BuildModule> createRootfsModule();
std::unique_ptr<BuildModule> createUbuntuModule();
std::unique_ptr<BuildModule> createArchModule();

int main() {
    DistroBuilder builder("rootfs");

    builder.addModule(createRootfsModule());

    // Choose ONE:
    builder.addModule(createUbuntuModule());
    // builder.addModule(createArchModule());

    builder.build();
    return 0;
}
