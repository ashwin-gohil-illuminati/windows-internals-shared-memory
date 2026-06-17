#include <windows.h>
#include <iostream>

/*
It will ask the Object Manager to find the exact memory object the Writer created (Local\MySharedMemory)
and map that same physical RAM into its own separate Virtual Address Space.

First refer the coursework for this program

Compilation instructions :
x86_64-w64-mingw32-g++ shared_reader.cpp -o shared_reader.exe -O0 -g -static

The above is compiled under linux and the executable is tested in windows enterprise ltsc vm.
While this program runs, the corresponding stub shared_writer will still be running.

*/

int main() {
    std::cout << "[*] Chapter 5: Memory-Mapped Files (The Reader)" << std::endl;

    // 1. Ask the Object Manager to find the existing shared memory object
    HANDLE hMapFile = OpenFileMappingW(
        FILE_MAP_ALL_ACCESS,     // Read/Write access
        FALSE,                   // Do not inherit the name
        L"Local\\MySharedMemory" // The exact Kernel Namespace name the Writer used
    );

    if (hMapFile == NULL) {
        std::cout << "[-] Could not open file mapping object. Error: " << GetLastError() << std::endl;
        std::cout << "[-] Make sure the Writer program is currently running and paused!" << std::endl;
        return 1;
    }

    // 2. Map that same kernel object into our process's Virtual Address Space
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

    // 3. Read the secret message directly out of the shared physical RAM
    std::cout << "[+] Successfully connected to the Writer's shared memory!" << std::endl;
    
    // We cast the raw memory pointer to a char* so C++ knows to print it as text
    std::cout << "[+] Message intercepted from RAM: '" << (char*)pBuf << "'" << std::endl;

    std::cout << "\n[*] Press Enter to close the connection and exit..." << std::endl;
    std::cin.get();

    // 4. Clean up (Preventing Handle Leaks)
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    std::cout << "[*] Memory unmapped and handles closed. Exiting." << std::endl;

    return 0;
}