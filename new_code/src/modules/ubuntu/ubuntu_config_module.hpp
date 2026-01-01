#pragma once
#include "../../module.hpp"
#include "../utils/config_parser.hpp"
#include <memory>

std::unique_ptr<BuildModule> createUbuntuConfigModule(const VereConfigParser& cfg);
