Implement a proper OS versioning system by reading the version from a VERSION file via a C++ loader, storing it in a variable, and reusing it across the codebase (e.g., in ubuntu_config.cpp) instead of hardcoding values like VERSION_ID="1.0".

This change should allow bumping the OS version globally by editing a single VERSION file.

Introduce a general configuration file (config.txt) to hold parameters such as kernel_version, kernel_clone_url, os_packages, etc., with a loader/parser to be implemented.

Create an initial config.txt with these variables so it can be parsed later.

Explore and standardize namespace usage to improve code clarity and understanding.