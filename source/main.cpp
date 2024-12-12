#include "elf_loader.hpp"
#include "hart.hpp"

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "No input elf file." << std::endl;
        return -1;
    }

    Hart hart;
    ELFLoader elfik (argv[1]);

    elfik.load (hart);
    hart.run_pipeline ();

    return 0;
}