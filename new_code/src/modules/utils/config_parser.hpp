#pragma once
#include <string>
#include <unordered_map>
#include <vector>

class VereConfigParser {
public:
    bool load(const std::string& filename);
    std::string get(const std::string& key, const std::string& def = "") const;
    std::vector<std::string> getList(const std::string& key) const;
    void print() const;

private:
    std::unordered_map<std::string, std::string> config;
    static void trim(std::string& s);
};