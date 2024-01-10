// DecryptData.cpp
//
// THis file uses the input data and key information to decrypt the input data
//

#include "Main.h"

// YOU WILL DELETE ALL THIS CODE WHEN YOU DO YOUR PROJECT - THIS IS GIVING YOU EXAMPLES OF HOW TO 
// ACCESS gPasswordHash AND gKey

void decryptData_03(char* data, int sized)
{
	int startingIndex;
	unsigned char key;
	int hop_count;
	int index;

	__asm
	{
		mov ecx, sized		// use ecx to store length of the data
		cmp ecx, 0
		je EXIT_ASM

		mov edi, data			// put source/destination address in edi

		// get the starting index
		xor edx, edx
		lea esi, gPasswordHash
		mov dh, byte ptr[esi]
		mov dl, byte ptr[esi + 1]		// starting index is in edx
		mov startingIndex, edx          // don't really need this local variable, but for illustration purposes
		mov bl, byte ptr[edx + gkey]		// bl has the value we will XOR with




		mov key, bl                     // don't need this loal variable either
		xor ecx, ecx		//DONT CHANGE ECX!!

		ENCRYPT_LOOP :

		//MILESTONE #2
		//TEAM #11 Decode Order: ACEBD
		// *the functions have also been adjusted to work backwards
		//	from their encode counterparts

		//
		// (#A) code table swap		0x43 -> CodeTable[0x43] == 0xC4
		// decode: switched gEncodeTable with gDecodeTable

		xor eax, eax
			xor edx, edx
			mov al, byte ptr[edi + ecx]		//al is our passed value
			lea edx, gDecodeTable
			mov ah, byte ptr[edx + eax]		//ah is now our encoded byte

			mov byte ptr[edi + ecx], ah


			//
			// (#C) reverse bit order	0x92 -> 0x49	abcd efgh -> hgfe dcba
			// decode: nothing
			//											7654 3210 -> 0123 4567
			//									decode	0123 4567 -> 7654 3210

			xor eax, eax
			xor edx, edx
			mov dh, 0x08
			mov al, byte ptr[edi + ecx] //al is the original byte

			LOOP_REV:

		rol al, 1	//move MSB into LSB
			mov dl, 0x01	//dl is a temp, to hold LSB
			and dl, al	//dl now has LSB
			or ah, dl	//ah is our reversed byte, LSB is placed
			ror ah, 1	//rotate byte to append bits in rev order

			dec dh
			cmp dh, 0
			jne LOOP_REV

			mov byte ptr[edi + ecx], ah


			//
			// (#E) rotate 3 bits left	0xDC -> 0xE6	abcd efgh -> defg habc
			//	decode: rotate 3 bits right

			xor eax, eax
			mov al, byte ptr[edi + ecx]		// Move the byte at the address (edi+ecx) into al
			ror al, 3	// rotate 3 bits to the right
			mov byte ptr[edi + ecx], al


			// DECRYPT
			// (#B) nibble rotate out	0xC4 -> 0x92	abcd efgh -> bcda hefg
			// decode: loop function 3 more times to return to OG positions

			xor bh, bh

			LOOP_DECODE_NIB :
		xor eax, eax
			mov al, byte ptr[edi + ecx] //low nibble
			mov ah, al
			shl ah, 4
			and al, 0x0F
			or ah, al
			ror ah, 1
			and ah, 0x0F

			xor edx, edx
			mov dl, byte ptr[edi + ecx]	//high nibble
			mov dh, dl
			shr dh, 4
			and dl, 0xF0
			or dh, dl
			rol dh, 1
			and dh, 0xF0

			or ah, dh
			mov byte ptr[edi + ecx], ah

			inc bh
			cmp bh, 3
			jne LOOP_DECODE_NIB


			//
			// (#D) invert bits 0,2,4,7		0x49 -> 0xDC	abcd efgh -> XbcX dXbX
			//	decode: nothing

			xor edx, edx
			mov dl, byte ptr[edi + ecx]
			xor dl, 0x95
			mov byte ptr[edi + ecx], dl
			


			//end of MS#2
			

			xor byte ptr[edi + ecx], bl

			/*MILESTONE #3 START*/
			xor eax, eax
			mov eax, dword ptr[hop_count]
			add eax, edx
			mov dword ptr[index], eax  //index = index + hop_count[round];

			cmp index, 0x10001
			jl JUMPED_INDEX
			xor eax, eax
			mov eax, dword ptr[index]
			sub eax, 0x10001
			mov dword ptr[index], eax
			JUMPED_INDEX :
			
			/*MILESTONE #3 END*/
			
			inc ecx
			cmp ecx, sized
			jb ENCRYPT_LOOP

			EXIT_ASM :
	}

	return;
} 

