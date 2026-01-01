# Small TODO List

So essentially, we need to have the following done :=

- We need to have a proper versioning system, for example, we want to read the OSVERSION from a file called
`VERSION` from this file, we make a c++ loader, that reads the file and simply stores it within a variable.
After storing it we can export it and use it within other files, for example in `src/modules/ubuntu_config.cpp`, we
can use that version instead of hard coding it like we are doing now:

```
Current Implementation:
"VERSION_ID=\"1.0\"\n"
````

So this can use improvement as you can see.

This will make it so we can simply change the VERSION file and we have
thhe version bumped within the entire operating system.

- We also need a sort of config file, just call it ``config.txt``. I'll
make a loader for that one, since we will also include things like :=
    - kernel_version
    - kernel_clone_url
    - os_packages

and more.

So if you can make the config.txt, with some of the veriables like `kernel_version, kernel_clone_url, os_packages` i can make the parser.

- Also, if you didn't do it yet, play around with the `namespace`. so we can actually understand how they work.