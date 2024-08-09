
.CODE
RegGetEs PROC
	mov rax, es
	ret
RegGetEs ENDP

RegGetCs PROC
	mov rax, cs
	ret
RegGetCs ENDP

RegGetSs PROC
	mov rax, ss
	ret
RegGetSs ENDP

RegGetDs PROC
	mov rax, ds
	ret
RegGetDs ENDP

RegGetFs PROC
	mov rax, fs
	ret
RegGetFs ENDP

RegGetGs PROC
	mov rax, gs
	ret
RegGetGs ENDP

RegGetDr0 PROC
	mov rax, dr0
	ret
RegGetDr0 ENDP

RegGetDr1 PROC
	mov rax, dr1
	ret
RegGetDr1 ENDP

RegGetDr2 PROC
	mov rax, dr2
	ret
RegGetDr2 ENDP

RegGetDr3 PROC
	mov rax, dr3
	ret
RegGetDr3 ENDP

RegGetGdt PROC
	sldt rax
	ret
RegGetGdt ENDP

GetGdtBase PROC
    LOCAL	gdtr[10]:BYTE
    sgdt	gdtr
    mov		eax, dword PTR gdtr[2]
    ret
GetGdtBase ENDP

END
