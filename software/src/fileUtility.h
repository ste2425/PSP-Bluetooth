#pragma once

#include "Arduino.h"
#include <FS.h>


    class FileUtility
    {
    public:
        // static void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
        // static void createDir(fs::FS &fs, const char *path);
        // static void removeDir(fs::FS &fs, const char *path);
        static String readFile(fs::FS &fs, const char *path);
        static void writeFile(fs::FS &fs, const char *path, const char *message);
        static void appendFile(fs::FS &fs, const char *path, const char *message);
        static void renameFile(fs::FS &fs, const char *from, const char *to);
        static void deleteFile(fs::FS &fs, const char *path);
    };
