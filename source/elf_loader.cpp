#include "elf_loader.hpp"
#include "hart.hpp"

void ELFLoader::load (Hart& hart) {
    assert (elf_version (EV_CURRENT) != EV_NONE && "libelf initialization failed");

    //open the elf file
    int elf_file = open (file_path.c_str(), O_RDONLY);
    assert (elf_file != -1 && "there is no such elf-file");

    //get the elf file size
    file_size = lseek(elf_file, 0, SEEK_END);
    lseek (elf_file, 0, SEEK_SET);
    assert (file_size > 0 && "elf-file is empty");

    std::vector<uint8_t> elf_buf (file_size, 0);
    int read_num = read (elf_file, elf_buf.data(), file_size);
    assert (read_num != -1);

    //get elf-file
    Elf* elf = elf_begin (elf_file, ELF_C_READ, nullptr);
    assert (gelf_getclass (elf) != ELFCLASSNONE && "fail to get elf file");
    assert (gelf_getclass (elf) != ELFCLASS32 && "the file is 32 bit, 64 bit is required");

    //get file header
    GElf_Ehdr elf_header {};
    assert (gelf_getehdr (elf, &elf_header) && "can't get the file header");

    hart.set_pc_val (elf_header.e_entry);

    //process segments that are intended to be loaded into memory
    int segment_num = elf_header.e_phnum;
    bool first_load_segment_flag = true;

    for (int i = 0; i < segment_num; ++i) {
        GElf_Phdr seg_header{};
        assert (gelf_getphdr (elf, i, &seg_header) && "can't get the file segment");

        if (seg_header.p_type == PT_LOAD) {
            Segment seg (seg_header, elf_buf.data());

            if (first_load_segment_flag) {
                hart.set_start_addr (seg.get_vaddr());
                hart.set_sp ();
                first_load_segment_flag = false;
            }
            hart.map_seg_to_VAS (seg);
        }
    }
}