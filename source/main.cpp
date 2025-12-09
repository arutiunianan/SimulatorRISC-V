#include "elf_loader.hpp"
#include "hart.hpp"

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "No input elf file." << std::endl;
        return -1;
    }

    bool trace = false;
    if (argc >= 3 && !strcmp(argv[2], "-trace")) {
        trace = true;
    }

    Hart hart;
    ELFLoader elfik (argv[1]);

    elfik.load (hart);
    hart.run_pipeline (trace);

    return 0;
}