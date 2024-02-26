#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <format>
#include <iostream>
#include <sstream>

using namespace std;

constexpr std::array<int, 21> attributeEnums = {
        FILE_ATTRIBUTE_HIDDEN,
        FILE_ATTRIBUTE_SYSTEM,
        FILE_ATTRIBUTE_DIRECTORY,
        FILE_ATTRIBUTE_ARCHIVE,
        FILE_ATTRIBUTE_DEVICE,
        FILE_ATTRIBUTE_NORMAL,
        FILE_ATTRIBUTE_TEMPORARY,
        FILE_ATTRIBUTE_SPARSE_FILE,
        FILE_ATTRIBUTE_REPARSE_POINT,
        FILE_ATTRIBUTE_COMPRESSED,
        FILE_ATTRIBUTE_OFFLINE,
        FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
        FILE_ATTRIBUTE_ENCRYPTED,
        FILE_ATTRIBUTE_INTEGRITY_STREAM,
        FILE_ATTRIBUTE_VIRTUAL,
        FILE_ATTRIBUTE_NO_SCRUB_DATA,
        FILE_ATTRIBUTE_EA,
        FILE_ATTRIBUTE_PINNED,
        FILE_ATTRIBUTE_UNPINNED,
        FILE_ATTRIBUTE_RECALL_ON_OPEN,
        FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS
};


std::map<int, std::string> attributeMap = {
        {FILE_ATTRIBUTE_HIDDEN,                "Hidden"},
        {FILE_ATTRIBUTE_SYSTEM,                "Part of OS"},
        {FILE_ATTRIBUTE_DIRECTORY,             "Directory"},
        {FILE_ATTRIBUTE_ARCHIVE,               "Marked for backup or removal"},
        {FILE_ATTRIBUTE_DEVICE,                "Reserved for system use"},
        {FILE_ATTRIBUTE_NORMAL,                "Normal"},
        {FILE_ATTRIBUTE_TEMPORARY,             "Temporary"},
        {FILE_ATTRIBUTE_SPARSE_FILE,           "Sparse file"},
        {FILE_ATTRIBUTE_REPARSE_POINT,         "Reparse point or symbolic link"},
        {FILE_ATTRIBUTE_COMPRESSED,            "Compressed"},
        {FILE_ATTRIBUTE_OFFLINE,               "Offline"},
        {FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,   "Not content indexed"},
        {FILE_ATTRIBUTE_ENCRYPTED,             "Encrypted"},
        {FILE_ATTRIBUTE_INTEGRITY_STREAM,      "Integrity stream"},
        {FILE_ATTRIBUTE_VIRTUAL,               "Virtual"},
        {FILE_ATTRIBUTE_NO_SCRUB_DATA,         "No scrub data"},
        {FILE_ATTRIBUTE_EA,                    "Extended attributes"},
        {FILE_ATTRIBUTE_PINNED,                "Pinned"},
        {FILE_ATTRIBUTE_UNPINNED,              "Unpinned"},
        {FILE_ATTRIBUTE_RECALL_ON_OPEN,        "Recall on open"},
        {FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS, "Recall on data access"}
};

map<char, string> disks;

void prepareDisksSet() {
    for (int i = 0; i < 26; ++i) {
        disks['A' + i] = string(1, 'A' + i) + ":\\";
    }
}

string GetLastErrorAsString() {
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0) {
        return {}; //No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &messageBuffer, 0, nullptr);

    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
}

void printMenu() {
    std::cout
            << "1) ls disks\n"
            << "2) print disk info\n"
            << "3) create dir\n"
            << "4) rm dir\n"
            << "5) create file\n"
            << "6) copy/move file\n"
            << "7) get file attributes\n"
            << "8) set file attributes\n"
            << "9) get file time\n"
            << "10) set file time\n"
            << "11) exit\n";
    std::cout << "--------------------------\n";
}

void showDisks() {
    DWORD dwDrives = 0, dwMask = 1;
    CHAR chDrive;
    dwDrives = GetLogicalDrives();
    cout << "Currently available Disk Drives" << endl;
    for (UINT i = 0; i < 26; i++) {
        if (dwDrives & dwMask) {
            chDrive = 'A' + i;
            cout << chDrive << ": " << endl;
        }
        dwMask <<= 1;
    }
}

void getDiskInfo() {
    char diskName;
    cout << "Enter disk name: ";
    cin >> diskName;
    int type;

    type = GetDriveTypeA(disks[diskName].c_str());

    cout << "Disk type: " << endl;
    if (type == DRIVE_UNKNOWN)
        cout << " UNKNOWN" << endl;
    if (type == DRIVE_NO_ROOT_DIR)
        cout << " DRIVE NO ROOT DIR " << endl;
    if (type == DRIVE_REMOVABLE)
        cout << " REMOVABLE" << endl;
    if (type == DRIVE_FIXED)
        cout << " FIXED" << endl;
    if (type == DRIVE_REMOTE)
        cout << " REMOTE" << endl;
    if (type == DRIVE_CDROM)
        cout << " CDROM" << endl;
    if (type == DRIVE_RAMDISK)
        cout << " RAMDISK" << endl;
    cin.get();

    char VolumeNameBuffer[100];
    char FileSystemNameBuffer[100];
    unsigned long VolumeSerialNumber;

    BOOL GetVolumeInformationFlag = GetVolumeInformationA(
            disks[diskName].c_str(),
            VolumeNameBuffer,
            100,
            &VolumeSerialNumber,
            nullptr,
            nullptr,
            FileSystemNameBuffer,
            100);

    if (GetVolumeInformationFlag != 0) {
        cout << "Volume Name is " << VolumeNameBuffer << endl;
        cout << "Volume Serial Number is " << VolumeSerialNumber << endl;
        cout << "File System is " << FileSystemNameBuffer << endl;
    } else
        cout << "Not Present (GetVolumeInformation)" << endl;

    ULARGE_INTEGER FreeBytesAvailable = {0};
    ULARGE_INTEGER TotalNumberOfBytes = {0};
    ULARGE_INTEGER TotalNumberOfFreeBytes = {0};

    BOOL GetDiskFreeSpaceFlag = GetDiskFreeSpaceExA(
            disks[diskName].c_str(),                 // directory name
            &FreeBytesAvailable,     // bytes available to caller
            &TotalNumberOfBytes,     // bytes on disk
            &TotalNumberOfFreeBytes // free bytes on disk
    );

    unsigned long long freeAvail = FreeBytesAvailable.QuadPart;
    unsigned long long total = TotalNumberOfBytes.QuadPart;
    unsigned long long totalFree = TotalNumberOfFreeBytes.QuadPart;

    if (GetDiskFreeSpaceFlag != 0) {
        cout << "Total Number Of Free Available Bytes = " << freeAvail
             << "( " << freeAvail / 1024 / 1024
             << " Mb )" << endl;
        cout << "Total Number Of Free Bytes = " << totalFree
             << "( " << totalFree / 1024 / 1024
             << " Mb )" << endl;
        cout << "Total Number Of Bytes = " << total
             << "( " << total / 1024 / 1024
             << " Mb )" << endl;
    } else
        cout << "	Not Present (GetDiskFreeSpace)" << endl;
}

void createFile() {
    std::cout << "Enter file path: ";

    std::string path;
    std::cin >> path;
    HANDLE f = CreateFileA(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,
                           nullptr);
    if (f == nullptr) {
        std::cout << std::format("error: {}\n", GetLastErrorAsString());
        return;
    }

    if (!CloseHandle(f)) {
        std::cout << std::format("error: {}\n", GetLastErrorAsString());
        return;
    }

    std::cout << std::format("Created file: {}\n", path);

}

void createDir() {
    std::cout << "Enter dir path: ";
    std::string path;
    std::cin >> path;
    if (!CreateDirectoryA(path.c_str(), nullptr)) {
        std::cout << std::format("error: {}\n", GetLastErrorAsString());
        return;
    }
    std::cout << std::format("Created directory: {}\n", path);
}

void removeDir() {
    std::cout << "Enter dir path: ";
    std::string path;
    std::cin >> path;
    if (!RemoveDirectoryA(path.c_str())) {
        std::cout << std::format("error: {}\n", GetLastErrorAsString());
        return;
    }
    std::cout << std::format("Removed directory: {}\n", path);
}

void copyOrMoveFile() {
    std::cout
            << "1) Copy\n"
            << "2) Move\n";
    int action;
    std::cin >> action;

    if (action < 1 || action > 2) {
        std::cout << "Unsopported action!\n";
        return;
    }

    std::cout << "Enter old path: ";

    std::string oldPath;
    std::cin >> oldPath;

    std::cout << "Enter new path: ";

    std::string newPath;
    std::cin >> newPath;
    // Copy
    if (action == 1) {
        std::cout << "Overwrite existent files [y/N]: ";
        std::string overwrite;
        std::cin >> overwrite;
        bool overwriteFlag = (overwrite == "Y" || overwrite == "y");

        if (!CopyFileA(oldPath.c_str(), newPath.c_str(), !overwriteFlag)) {
            std::cout << std::format("error: {}\n", GetLastErrorAsString());
            return;
        }

        std::cout << std::format("Copied to the: {}\n", newPath);
        return;
    }
        // Move
    else if (action == 2) {
        if (!MoveFileA(oldPath.c_str(), newPath.c_str())) {
            std::cout << std::format("error: {}\n", GetLastErrorAsString());
            return;
        }

        std::cout << std::format("Moved to the: {}\n", newPath);
        return;
    }


}

std::vector<std::string> getListOfAttributes(const std::string &path) {
    std::vector<std::string> result;

    auto attributesMask = GetFileAttributesA(path.c_str());
    if (attributesMask == INVALID_FILE_ATTRIBUTES) {
        std::cout << "Something went wrong!\n";
    }

    for (auto attr: attributeEnums) {
        if (attributesMask & attr) {
            result.emplace_back(attributeMap.at(attr));
        }
    }

    return result;
}

void getFileAttributes() {
    std::cout << "Enter file path: ";
    std::string path;
    std::cin >> path;

    auto attributes = getListOfAttributes(path);

    for (auto &el: attributes) {
        std::cout << std::format(" {}\n", el);
    }

}

void setFileAttributes() {

    std::cout << "Enter file path: ";
    std::string path;
    std::cin >> path;

    auto attributes = getListOfAttributes(path);

    std::cout << "Current attributes:\n";
    for (auto &el: attributes) {
        std::cout << std::format(" {}\n", el);
    }

    std::cout << "List of available attributes:\n";
    for (int i = 0; i < attributeEnums.size(); ++i) {
        std::cout << std::format(" {}){}\n", i + 1, attributeMap.at(attributeEnums[i]));
    }
    std::cout << std::endl;
    std::cout << "Enter indexes which you want to add: ";
    std::string indexesLine;

    // Disable skipping whitespace characters
    std::cin >> std::ws >> std::noskipws;

    // Read from stdin until the end of the line
    char c;
    while (std::cin >> c && c != '\n') {
        indexesLine += c;
    }

    // Enable skipping whitespace characters again
    std::cin >> std::skipws;
    std::stringstream ss(indexesLine);

    std::vector<int> indexes;

    int index;
    while (ss >> index) {
        indexes.push_back(index);
    }

    DWORD attributeMask = 0;
    for (auto el: indexes) {
        attributeMask |= attributeEnums[el - 1];
    }

    if (!SetFileAttributesA(path.c_str(), attributeMask)) {
        std::cout << std::format("error: {}\n", GetLastErrorAsString());
        return;
    }
    std::cout << "Attributes added successfully!" << std::endl;

}

void getFileTime() {
    std::string path;
    std::cout << "Enter path to file: ";
    std::cin >> path;

    HANDLE hFile1;
    FILETIME ftCreate, ftLastAccess, ftLastWrite;
    SYSTEMTIME stCreate, stAccess, stWrite;

    hFile1 = CreateFile(path.c_str(),
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        nullptr,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        nullptr);

    if (hFile1 == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to open file. Error: " << GetLastErrorAsString() << std::endl;
        return;
    }

    if (!GetFileTime(hFile1, &ftCreate, &ftLastAccess, &ftLastWrite)) {
        std::cout << "Failed to get file time. Error: " << GetLastErrorAsString() << std::endl;
        CloseHandle(hFile1);
        return;
    }

    FileTimeToSystemTime(&ftCreate, &stCreate);
    FileTimeToSystemTime(&ftLastAccess, &stAccess);
    FileTimeToSystemTime(&ftLastWrite, &stWrite);

    std::cout << "File creation time: " << stCreate.wDay << "/" << stCreate.wMonth << "/" << stCreate.wYear << " "
              << stCreate.wHour + 3 << ":" << stCreate.wMinute << std::endl;
    std::cout << "Last access time: " << stAccess.wDay << "/" << stAccess.wMonth << "/" << stAccess.wYear << " "
              << stAccess.wHour + 3 << ":" << stAccess.wMinute << std::endl;
    std::cout << "Last write time: " << stWrite.wDay << "/" << stWrite.wMonth << "/" << stWrite.wYear << " "
              << stWrite.wHour + 3 << ":" << stWrite.wMinute << std::endl;

    CloseHandle(hFile1);
}

void setFileTime() {
    string path;
    cout << "Enter path to file: ";
    cin >> path;

    cout << "Enter date {dd mm yyyy}: ";
    int day, month, year;
    cin >> day >> month >> year;

    SYSTEMTIME systemTime;

    GetSystemTime(&systemTime);

    systemTime.wDay = day;
    systemTime.wMonth = month;
    systemTime.wYear = year;

    FILETIME fileTime;

    SystemTimeToFileTime(&systemTime, &fileTime);

    HANDLE filename = CreateFile(path.c_str(),
                                 FILE_WRITE_ATTRIBUTES,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 nullptr,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 nullptr);

    if (filename != INVALID_HANDLE_VALUE) {
        SetFileTime(filename,
                    (LPFILETIME) nullptr,
                    (LPFILETIME) nullptr,
                    &fileTime);
        CloseHandle(filename);
        cout << "File time set successfully." << endl;
    } else {
        cout << "Failed to open file." << endl;
    }
}

int main() {
    int action;
    bool running = true;
    prepareDisksSet();

    while (running) {
        printMenu();
        std::cin >> action;
        switch (action) {
            case 1:
                showDisks();
                system("pause");
                break;
            case 2:
                getDiskInfo();
                system("pause");
                break;
            case 3:
                createDir();
                system("pause");
                break;
            case 4:
                removeDir();
                system("pause");
                break;
            case 5:
                createFile();
                system("pause");
                break;
            case 6:
                copyOrMoveFile();
                system("pause");
                break;
            case 7:
                getFileAttributes();
                system("pause");
                break;
            case 8:
                setFileAttributes();
                system("pause");
                break;
            case 9:
                getFileTime();
                system("pause");
                break;
            case 10:
                setFileTime();
                system("pause");
                break;
            default:
                running = false;
                break;
        }
        system("cls");
    }

    return 0;
}
