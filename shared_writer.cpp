#include <windows.h>
#include <iostream>

/*
First refer the coursework for this program

Compilation instructions :
x86_64-w64-mingw32-g++ shared_writer.cpp -o shared_writer.exe -O0 -g -static

The above is compiled under linux and the executable is tested in windows enterprise ltsc vm.
The program has to be kept alive till the another stub the reader reads from the shared memory.

*/

int main() {
    std::cout << "[*] Chapter 5: Memory-Mapped Files (The Writer)" << std::endl;

    // 1. Create a named shared memory object in the OS Kernel
    // INVALID_HANDLE_VALUE means we are using the paging file (RAM), not a physical file on disk.
    HANDLE hMapFile = CreateFileMappingW(
        INVALID_HANDLE_VALUE,    // Use system paging file
        NULL,                    // Default security
        PAGE_READWRITE,          // Read/Write access
        0,                       // Maximum object size (high-order DWORD)
        256,                     // Maximum object size (low-order DWORD) - 256 bytes
        L"Local\\MySharedMemory" // The exact Kernel Namespace name the Reader will look for
    );

    if (hMapFile == NULL) {
        std::cout << "[-] Could not create file mapping object. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // 2. Map that kernel object into our process's Virtual Address Space
    // This gives us a raw pointer (pBuf) that we can actually write to in C++
    LPCTSTR pBuf = (LPTSTR) MapViewOfFile(
        hMapFile,                // Handle to map object
        FILE_MAP_ALL_ACCESS,     // Read/Write permission
        0,
        0,
        256
    );

    if (pBuf == NULL) {
        std::cout << "[-] Could not map view of file. Error: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    // 3. Write our secret message directly into the physical RAM
    std::cout << "[+] Shared memory created and mapped successfully!" << std::endl;
    const char* message = "Hello from the Writer Process! This is instant shared RAM.";
    
    // CopyMemory is a Windows API macro for memcpy
    CopyMemory((PVOID)pBuf, message, strlen(message) + 1);

    std::cout << "[+] Message written to shared RAM: '" << message << "'" << std::endl;
    std::cout << "\n[*] PAUSED. Do not press Enter yet." << std::endl;
    std::cout << "[*] The process must stay alive to keep the memory active." << std::endl;
    std::cin.get();

    // 4. Clean up (Applying Chapter 2 & 4 fundamentals!)
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    std::cout << "[*] Memory unmapped and handles closed. Exiting." << std::endl;

    return 0;
}