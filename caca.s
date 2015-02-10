.text						# start of code indicator.
.globl _main					# make the main function visible to the outside.
_main:							# actually label this spot as the start of our main function.
	pushl	%ebp				# save the base pointer to the stack.
	movl	%esp, %ebp			# put the previous stack pointer into the base pointer.
	subl	$8, %esp			# Balance the stack onto a 16-byte boundary.
	movl	$0, %eax			# Stuff 0 into EAX, which is where result values go.
	leave						# leave cleans up base and stack pointers again.
	ret							# returns to whoever called us.


	.text
.globl _doSomething				# Our doSomething function.
_doSomething:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	nop							# does nothing.
	leave
	ret
.globl _main
_main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp			# 8 to align, 16 for our 4-byte parameter and padding.
	movl	$3, (%esp)			# write our parameter at the end of the stack (i.e. padding goes first).
	call	_doSomething		# call doSomething.
	movl	$0, %eax
	leave
	ret



; Sample x64 Assembly Program
; Chris Lomont 2009 www.lomont.org
extrn ExitProcess: PROC   ; external functions in system libraries
extrn MessageBoxA: PROC
.data
caption db '64-bit hello!', 0
message db 'Hello World!', 0
.code
Start PROC
  sub    rsp,28h      ; shadow space, aligns stack
  mov    rcx, 0       ; hWnd = HWND_DESKTOP
  lea    rdx, message ; LPCSTR lpText
  lea    r8,  caption ; LPCSTR lpCaption
  mov    r9d, 0       ; uType = MB_OK
  call   MessageBoxA  ; call MessageBox API function
  mov    ecx, eax     ; uExitCode = MessageBox(...)
  call ExitProcess
Start ENDP
End
