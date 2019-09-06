BITS 64

; Author : Abhinav Thakur
; Email  : compilepeace@gmail.com

; Description : Shellcode prints a message and transfers control to a specified address


global _start


section .text


_start:

	; Save register state
	push rax
	push rcx
	push rdx
	push rsi
	push rdi
	push r11

	
	jmp	parasite
	message:	db	"-x-x-x-x- COMPILEPEACE : Cute little virus ^_^ -x-x-x-x-", 0xa


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


	; Restoring register state
	pop r11
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rax

	
	; jmp to original host entry point (to be patched by kaal bhairav)
	mov	rax, 0xAAAAAAAAAAAAAAAA		
	jmp	rax

