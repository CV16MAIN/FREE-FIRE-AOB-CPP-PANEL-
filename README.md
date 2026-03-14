🕵️‍♂️ Free Fire UB Scanner & Replacer
An Educational Deep-Dive into Windows Memory Manipulation
📖 Introduction
This project is a beginner-friendly tool designed for Computer Science students to understand how software interacts with computer memory. Using the Windows API, it demonstrates the core logic behind an AoB (Array of Bytes) search and replace.

Instead of just being a "cheat," this is a learning resource that demystifies how pointers, process handles, and virtual memory work in a real-world environment like an emulator.

🧩 The Core Concepts
If you are new to CS, think of this program as a Digital Surgeon:

Finding the Patient: The program looks for the HD-Player.exe process (the emulator).

The Scan (AoB Search): It searches the "DNA" (bytes) of the game to find a specific sequence.

The Surgery (Replacement): It swaps out a small piece of that DNA with a new sequence to change the game's behavior.

🛠️ How it Works (Step-by-Step)
1. Opening the Door (OpenProcess)
We first ask Windows for permission to read and write to the target application. This gives us a Handle—essentially a "VIP Pass" to access the program's memory.

2. Mapping the Terrain (VirtualQueryEx)
A program's memory is massive. We use this function to "scout" the memory and find the areas that are MEM_COMMIT (active and full of data). This ensures we don't try to scan empty "dead" space, which would crash our program.

3. The Photocopy (ReadProcessMemory)
To search quickly, we take a chunk of memory from the game and copy it into a Buffer (a std::vector) inside our own program.

4. The Pattern Match (std::search)
We use standard C++ algorithms to find our "Array of Bytes" inside our photocopy. Once we find a match, we calculate the Offset (the exact distance from the start) to find the real-world address in the game.

5. The Injection (WriteProcessMemory)
Finally, we reach back into the game at that exact address and overwrite the old data with our new replaceBytes.

💻 Code Highlight: The "Brain" of the Scanner
C++
// This loop is where the magic happens
while (VirtualQueryEx(processHandle, currentAddress, &membi, sizeof(membi))) {
    if (membi.State == MEM_COMMIT) {
        // 1. Create a bucket (Buffer)
        std::vector<unsigned char> buffer(membi.RegionSize);

        // 2. Scoop up the memory
        ReadProcessMemory(processHandle, currentAddress, buffer.data(), membi.RegionSize, nullptr);

        // 3. Look for the pattern
        auto match = std::search(buffer.begin(), buffer.end(), searchPattern, searchPattern + size);
        
        if (match != buffer.end()) {
            // Found it! Now we calculate the address and swap the bytes.
            uintptr_t target = reinterpret_cast<uintptr_t>(currentAddress) + std::distance(buffer.begin(), match);
            WriteProcessMemory(processHandle, (LPVOID)target, replaceBytes, replaceSize, nullptr);
        }
    }
    currentAddress += membi.RegionSize; // Move the radar forward
}
⚠️ Requirements & Safety
OS: Windows 10/11

Permissions: Must be Run as Administrator (Windows protects memory from non-admins!)

Compiler: Visual Studio 2019 or later (C++17/20)

📜 Disclaimer
This project is for educational purposes only. It was created to help students understand the Windows API and low-level memory management. Use this knowledge to build better software and understand security!
