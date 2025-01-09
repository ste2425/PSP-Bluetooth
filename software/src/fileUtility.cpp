#include "fileUtility.h"

// void FileUtility::listDir(fs::FS &fs, const char *dirname, uint8_t levels)
// {
//     Serial.printf("Listing directory: %s\n", dirname);

//     File root = fs.open(dirname);
//     if (!root)
//     {
//         Serial.println("Failed to open directory");
//         return;
//     }
//     if (!root.isDirectory())
//     {
//         Serial.println("Not a directory");
//         return;
//     }

//     File file = root.openNextFile();
//     while (file)
//     {
//         if (file.isDirectory())
//         {
//             Serial.print("  DIR : ");
//             Serial.println(file.name());
//             if (levels)
//             {
//                 listDir(fs, file.path(), levels - 1);
//             }
//         }
//         else
//         {
//             Serial.print("  FILE: ");
//             Serial.print(file.name());
//             Serial.print("  SIZE: ");
//             Serial.println(file.size());
//         }
//         file = root.openNextFile();
//     }
// }

// void FileUtility::createDir(fs::FS &fs, const char *path)
// {
//     Serial.printf("Creating Dir: %s\n", path);
//     if (fs.mkdir(path))
//     {
//         Serial.println("Dir created");
//     }
//     else
//     {
//         Serial.println("mkdir failed");
//     }
// }

// void FileUtility::removeDir(fs::FS &fs, const char *path)
// {
//     Serial.printf("Removing Dir: %s\n", path);
//     if (fs.rmdir(path))
//     {
//         Serial.println("Dir removed");
//     }
//     else
//     {
//         Serial.println("rmdir failed");
//     }
// }

String FileUtility::readFile(fs::FS &fs, const char *path)
{
    File file = fs.open(path);

    if (!file)
    {
        return "";
    }

    auto contents = file.readString();

    file.close();

    return contents;
}

void FileUtility::writeFile(fs::FS &fs, const char *path, const char *message)
{
    File file = fs.open(path, FILE_WRITE);

    if (!file)
    {
        return;
    }

    if (!file.print(message))
    {
        Serial.println("Write failed");
    }

    file.close();
}

 void FileUtility::appendFile(fs::FS &fs, const char *path, const char *message)
 {
     Serial.printf("Appending to file: %s\n", path);

     File file = fs.open(path, FILE_APPEND);
     if (!file)
     {
         Serial.println("Failed to open file for appending");
         return;
     }
     if (file.print(message))
     {
         Serial.println("Message appended");
     }
     else
     {
         Serial.println("Append failed");
     }
     file.close();
 }

 void FileUtility::renameFile(fs::FS &fs, const char *from, const char *to)
 {
     Serial.printf("Renaming file %s to %s\n", from, to);
     if (fs.rename(from, to))
     {
         Serial.println("File renamed");
     }
     else
     {
         Serial.println("Rename failed");
     }
 }

void FileUtility::deleteFile(fs::FS &fs, const char *path)
{
   if (!fs.remove(path))
   {
       Serial.println("Delete failed");
   }
}
