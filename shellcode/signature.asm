; Author : Abhinav Thakur
; Email  : compilepeace@gmail.com

; Description : Shellcode prints a message and transfers control to a specified address


global _start


section .text


_start:
	
	jmp	shellcode
	message:	db	"-x-x-x-x- Cute little virus ^_^ -x-x-x-x-", 0xa


shellcode:

	; Print our message
	xor	rax, rax			; Zero out RAX
	add	rax, 0x1			; Syscall number of write() - 0x1
	mov rdi, rax			; File descriptor - 0x1 (STDOUT)
	lea rsi, [rel message]	; Addresses the label relative to RIP (Instruction Pointer), i.e. 
							; dynamically identifying the address of the 'message' label.
	xor rdx, rdx
	mov dl, 0x2b			; message size = 43 bytes (0x2b)
	syscall					

	mov	rax, 0xAAAAAAAA		; To be patched by original entry point
	jmp	rax

	; Exit peacefully
	xor	rax, rax			; Zero out RAX
	mov rdi, rax			; Exit status
	mov al, 0x3c			; exit() syscall number - 60 (0x3c)
	syscall			
