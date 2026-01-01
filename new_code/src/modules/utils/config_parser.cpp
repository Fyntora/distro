#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "config_parser.hpp"

class VereConfigParser {
public:
    bool VereConfigParser::load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[!] Failed to open config file: " << filename << "\n";
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::string trimmed_line = line;
            trim(trimmed_line);

            if (trimmed_line.empty())
                continue;

            // skip comment lines
            if (trimmed_line.rfind("//", 0) == 0)
                continue;

            std::string key, value;
            std::istringstream ss(line);
            if (!std::getline(ss, key, '=')) continue;
            if (!std::getline(ss, value)) continue;
            trim(key);
            trim(value);

            config[key] = value;
        }

        return true;
    }

    std::string VereConfigParser::get(const std::string& key, const std::string& def = "") const {
        auto it = config.find(key);
        return it != config.end() ? it->second : def;
    }

    std::vector<std::string> VereConfigParser::getList(const std::string& key) const {
        std::vector<std::string> result;
        auto it = config.find(key);
        if (it == config.end())
            return result;

        std::istringstream ss(it->second);
        std::string item;
        while (std::getline(ss, item, ',')) {
            trim(item);
            if (!item.empty())
                result.push_back(item);
        }
        return result;
    }

    void VereConfigParser::print() const {
        std::cout << "Config values:\n";
        for (const auto& [k, v] : config) {
            std::cout << "  " << k << " = " << v << "\n";
        }
    }

private:
    std::unordered_map<std::string, std::string> config;

    static void VereConfigParser::trim(std::string& s) {
        const char* whitespace = " \t\n\r";
        s.erase(0, s.find_first_not_of(whitespace));
        s.erase(s.find_last_not_of(whitespace) + 1);
    }
};

int main() {
    VereConfigParser cfg;
    if (!cfg.load("distro.conf"))
        return 1;

    // This prints the config's
    // values.
    cfg.print();

    std::cout << "Kernel version: " << cfg.get("kernel_version", "default") << "\n";

    std::vector<std::string> packages = cfg.getList("os_packages");
    std::cout << "Packages:\n";
    for (const auto& p : packages) {
        std::cout << "  - " << p << "\n";
    }
}
