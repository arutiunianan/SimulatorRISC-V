#pragma once

#include <iostream>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <cassert>

#include <gelf.h>
#include <libelf.h>

#include "core/segment.hpp"

class Hart;

class ELFLoader {
private:
    std::string file_path;
    int file_size = 0;
public:
    ELFLoader (std::string path): file_path (path) {}
    void load (Hart& hart);
};