# 🕵️‍♂️ Free Fire UB Scanner & Replacer
### *A Beginner's Gateway to Windows Memory Manipulation*

## 📝 Project Overview
This is a basic educational tool created by **cv16** to demonstrate how **UB (Utility Byte) Injection** works. This project was built as a passion project to provide a clear, simplified view of how memory manipulation functions within the Windows environment. 

The goal is to show that interacting with a process's memory isn't "magic"—it is a logical sequence of API calls. It is designed specifically for beginners in Computer Science who want to start their journey with the **Windows API**.

---

## 💡 Developer's Note
> "I created this to give a basic understanding of how memory manipulation works. It is actually pretty easy to build something like this once you understand the core flow. This is a fun project meant to help you get a better view of how low-level software interaction works." — **cv16**

---

## 🛠️ Technical Definitions
To understand this project, you should be familiar with these core concepts:

* **AoB (Array of Bytes):** A specific sequence of numbers (hexadecimal) that represents a piece of code or data. We treat this like a "digital fingerprint."
* **Process Handle:** A temporary token or "key" granted by Windows that allows our program to communicate with another running application.
* **Memory Paging:** Memory is divided into "pages." We use `VirtualQueryEx` to find "Committed" pages—areas that are actually in use—so we don't try to scan empty space.

---

## 🚀 How the Injection Works
The logic follows a simple four-step process:

1.  **Process Attachment:** We use `OpenProcess` to get permission to read and write.
2.  **Memory Mapping:** The program loops through the target's memory using `VirtualQueryEx`. 
3.  **Pattern Matching:** We "photocopy" memory chunks into a buffer and use `std::search` to find our AoB signature.
4.  **The Swap:** Once found, we calculate the exact address and use `WriteProcessMemory` to inject the new bytes.

---

## 💻 Code Architecture
The core logic is wrapped in a high-performance loop that ensures every valid memory region is checked:

```cpp
while (VirtualQueryEx(hdplayerSnapshot, currectAddress, &membi, sizeof(membi))) {
    if (membi.State == MEM_COMMIT) {
        // Create a local buffer (The "Photocopy")
        std::vector<unsigned char> buffer(membi.RegionSize);
        
        // Read the memory from the emulator
        ReadProcessMemory(hdplayerSnapshot, currectAddress, buffer.data(), membi.RegionSize, nullptr);
        
        // Perform the AoB Scan
        auto match = std::search(buffer.begin(), buffer.end(), searchBytes, searchBytes + sizeof(searchBytes));

        if (match != buffer.end()) {
            // Success! Inject the replacement bytes
            uintptr_t targetAddress = reinterpret_cast<uintptr_t>(currectAddress) + std::distance(buffer.begin(), match);
            WriteProcessMemory(hdplayerSnapshot, (LPVOID)targetAddress, replaceBytes, sizeof(replaceBytes), nullptr);
        }
    }
    currectAddress += membi.RegionSize; // Move to the next block
}
