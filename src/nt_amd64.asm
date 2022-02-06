bits 64

global WinMain

extern exit
extern putchar
extern getchar
extern printf
extern puts
extern memset
extern scanf

section .rdata
	ifmt db "%lld",0ah, 0
	
section .bss
	mem_base resb MEMSIZE
	
section .text
	
_puti:	
	sub rsp, 32
	mov rcx, ifmt
	mov rdx, [r14]
	call printf

	add rsp, 32
	ret

_readi:
	sub rsp, 32
	mov rcx, ifmt
	mov rdx, r14
	call scanf

	add rsp, 32
	ret
	
_ascii_io:
	sub rsp, 32
	mov rax, [r14]
	test rax, rax
	jz .read_c

	mov rcx, rax
	call putchar

	add rsp, 32
	ret
.read_c:
	sub rsp, 32
	call getchar
	mov [r14], rax

	add rsp, 32
	ret
	
_bye:
	xor ecx, ecx
	call exit
	
WinMain:
	; Zero memory
	mov rcx, mem_base
	mov rdx, MEMSIZE
	xor r8, r8
	call memset

	mov r14, mem_base	; memory ptr
	xor r12, r12		; register 1
	xor r13, r13		; register 2

	xor ebx, ebx		; register information
