// EncryptData.cpp
//
// This file uses the input data and key information to encrypt the input data
//

#include "Main.h"


void encryptData_03(char* data, int datalength)
{
	int startingIndex;
	unsigned char key;
	int hop_count;
	int index;

	__asm
	{
		//checks if file has nothing in it
		mov ecx, datalength		// use ecx to store length of the data
		cmp ecx, 0
		je EXIT_ASM

		mov edi, data			// put source/destination address in edi

		// get the starting index (M#1)
		xor edx, edx
		lea esi, gPasswordHash			// esi is pointing to the gPasswordHash array[0]
		mov dh, byte ptr[esi]			// dh has value of esi, ( gPasswordHash[0+round*4] * 256 )
		mov dl, byte ptr[esi + 1]		// starting index is in edx ( gPasswordHash[1+round*4] )
		mov startingIndex, edx          // don't really need this local variable, but for illustration purposes ( Starting_index[round] = )

		/*MILESTONE #3 GOES HERE*/
		xor eax, eax
		mov ah, byte ptr[esi + 2]		//gPasswordHash[2+round*4] * 256 
		mov al, byte ptr[esi + 3]		//gPasswordHash[3+round*4]
		mov hop_count, eax
		cmp hop_count, 0
		jne JUMPED_HOP
		mov hop_count, 0xFFFF;
	JUMPED_HOP:
		/*Milestone #3 END*/

		//mov index, edx  //index = Starting_index[round];

		mov bl, byte ptr[edx + gkey]		// bl has the value we will XOR with ( gKey[index] )
			mov key, bl                     // don't need this local variable either (bl IS the index)
			xor ecx, ecx	//DO NOT CHANGE THIS VALUE IN THE LOOP!!

			ENCRYPT_LOOP :
		xor byte ptr[edi + ecx], bl		//data[x] = data[x] ^ gKey[index]

			//MILESTONE #2
			//TEAM #11 Encode Order: DBECA

			//
			// (#D) invert bits 0,2,4,7		0x49 -> 0xDC	abcd efgh -> XbcX dXbX
			//

		xor eax, eax
			mov al, byte ptr[edi + ecx]
			xor al, 0x95
			mov byte ptr[edi + ecx], al


			// ENCRYPT
			// (#B) nibble rotate out	0xC4 -> 0x92	abcd efgh -> bcda hefg
			//

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


			//
			// (#E) rotate 3 bits left	0xDC -> 0xE6	abcd efgh -> defg habc
			//

			xor eax, eax
			mov al, byte ptr[edi + ecx]		// Move the byte at the address (edi+ecx) into al
			rol al, 3	// rotate 3 bits to the left
			mov byte ptr[edi + ecx], al


			//
			// (#C) reverse bit order	0x92 -> 0x49	abcd efgh -> hgfe dcba
			//

			xor eax, eax
			xor edx, edx
			mov dh, 0x08
			mov al, byte ptr[edi + ecx] //al is the original byte

			LOOP_REV:
		rol al, 1	//move MSB into LSB
			mov dl, 0x01	//dl is a temp, to hold LSB,  dl = 0000 0001
			and dl, al	//dl now has LSB,	dl = 0000 000a
			or ah, dl	//ah is our reversed byte, LSB is placed	ah = 0000 000a
			ror ah, 1	//rotate byte to append bits in rev order	ah = dcba 0000

			dec dh
			cmp dh, 0
			jne LOOP_REV

			mov byte ptr[edi + ecx], ah


			//
			// (#A) code table swap		0x43 -> CodeTable[0x43] == 0xC4
			//

			xor eax, eax
			xor edx, edx
			mov al, byte ptr[edi + ecx]		//al is our passed value
			lea edx, gEncodeTable
			mov ah, byte ptr[edx + eax]		//ah is now our encoded byte

			mov byte ptr[edi + ecx], ah

			//End of MS#2


			inc ecx
			cmp ecx, datalength
			jb ENCRYPT_LOOP

			EXIT_ASM :
	}

	return;
} 

