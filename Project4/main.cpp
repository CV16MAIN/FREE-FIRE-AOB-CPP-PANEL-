#include <iostream> 
#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <algorithm>  
#include <iterator>

#define GREEN  "\033[32m"
#define RED     "\033[31m"
#define WHITE   "\033[0m" 






unsigned char searchBytes[]{ 0x70, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x3F, 0x0A, 0xD7,
	0xA3, 0x3B, 0x0A, 0xD7, 0xA3, 0x3B, 0x8F, 0xC2, 0x75, 0x3D, 0xAE, 0x47,
	0xE1, 0x3D, 0x9A, 0x99, 0x19, 0x3E, 0xCD, 0xCC, 0x4C, 0x3E, 0xA4, 0x70, 0xFD, 0x3E };

unsigned char replaceBytes[]{ 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x3F, 0x0A, 0xD7,
	0xA3, 0x3B, 0x0A, 0xD7, 0xA3, 0x3B, 0x8F, 0xC2, 0x75, 0x3D, 0xAE, 0x47,
	0xE1, 0x3D, 0x9A, 0x99, 0x19, 0x3E, 0xCD, 0xCC, 0x4C, 0x3E, 0xA4, 0x70, 0xFD, 0x3E };






DWORD processCheck() {

	HANDLE hSnapshot{}; // THIS IS THE HANDLE like a thicket that we need to ask the system for the exe we are serching
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // THIS IS A FUNC THAT HELP USE TO GET ALL THE SNAPPOROCESS  RUNNING AND 0 IS THAT WE WAN'T ALL THE PROCESS 
	PROCESSENTRY32 p32;  // this is to  store all the process name that we get from the system .
	p32.dwSize = sizeof(tagPROCESSENTRY32);// Windows requires us to set the size first
	BOOL porcessGot{ Process32First(hSnapshot , &p32) }; // BOOL FROM WINDOWS API it is an 32 bit int 
	bool isfoundporcess{};
	if (porcessGot) {
		
		do {

			if (strcmp(p32.szExeFile, "HD-Player.exe") == 0) {
				// so using p32 we are getting the snapshort (it mean a poin in time pic at this currect)
				// then we are using that forozen data and then asking for it's name using szexefile 
				// then szexefile return 11 56 67 45 24 these are all the c style char array ok so now to compaire 
				// we are converting it into string and then comparing it to notpad.exe string 
				// and then strcmp is using for this to compair this if it is equal it will return 0 
				std::cout << "Found It Process ID -" <<p32.th32ProcessID << '\n';
				CloseHandle(hSnapshot);
				return p32.th32ProcessID; 
				


			}
			 
		} while (Process32Next(hSnapshot, &p32)); 


	}
	if (isfoundporcess == 0) std::clog << "ProcessNotFound";
	CloseHandle(hSnapshot);
	return 0; 
}

int main() { 

	
	DWORD emmuProcessId{processCheck()};
	if (emmuProcessId == 0 ) {
		std::cout << GREEN << " No Process ID Found" << '\n';
		Beep(200, 344);
		Sleep(2000); // it mean 1000 mil sec or 1 sec
		return 0; 

	}
	else {
		std::cout << "Aob Injection START" << '\n';
		HANDLE hdplayerSnapshot{OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION , FALSE , emmuProcessId)}; // i am asking the windows for openporcess and getting write and read this is admin acess 
		// false cause it is for 
		if (hdplayerSnapshot == NULL) {
			std::cout << "Not Able To Find The HD player Snap"; 
			
		}
		else {
			std::cout << "Process opened successfully! We are in\n";
			///Main aob scan main 
			MEMORY_BASIC_INFORMATION membi;  //  The clipboard to hold the page info
			unsigned char* currectAddress{0}; //  The radar's current location, starting at 0

			while (VirtualQueryEx(hdplayerSnapshot , currectAddress , &membi, sizeof(membi))) {
				if (membi.State == MEM_COMMIT) {
					std::vector<unsigned char>  buffer(membi.RegionSize); // (Constructor) not Initializer list 
					// You saidconstrotor is some thing like we are saying that make a bucket of like 20 bytes size ok and put nothing in it but
                     //if we do same thing in list initization and size is 20 it will not store empty 20 bytes space it will just store 20 it self
					
					ReadProcessMemory(hdplayerSnapshot , currectAddress  , buffer.data() ,membi.RegionSize , nullptr); 
					
					auto match{ std::search(buffer.begin() , buffer.end() , searchBytes , searchBytes+sizeof(searchBytes)) }; // std::search is a function 

					if (match == buffer.end()) {
						std::cout << "NoT Able to Find The Patten In The Buffer \n";
					}
					else {

						auto offset{ std::distance(buffer.begin() , match) };  // this is for finding the distance b/w the buffer begin and the match the we found it can be like 500 bytes  
						// Find the global address in the emulator
						uintptr_t targateOffset{ reinterpret_cast<uintptr_t>(currectAddress) + offset };  // this is use to store pointer adress
						LPVOID targetoffsetIntoPointer{ reinterpret_cast<LPVOID>(targateOffset) }; // i am converting it into lpvoid cause win api want lpvoid 
						WriteProcessMemory(hdplayerSnapshot , targetoffsetIntoPointer, replaceBytes , sizeof(replaceBytes) , nullptr);
						std::cout << RED << "DONE AOB REPLACE DONE \n"; 

					}



				}

				currectAddress += membi.RegionSize; // this is to make the adress jump from this currect address
			}




			CloseHandle(hdplayerSnapshot);


		}


	}
	return 0; 

}
