; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

; ***BINDING***
thou EQU 0 ; 1st digit
hun EQU 4 ; 2nd digit
ten EQU 8 ; 3rd digit
one EQU 12 ; 4th digit	    

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	MOV R1, R0 ; 1's Digit
	MOV R2, #10
	UDIV R0, R0, R2
	MUL R0, R0, R2
	SUB R1, R1, R0
	UDIV R0, R0, R2
	PUSH {LR, R1}
	
	CMP R0, #0
	BEQ PrintDigit
	
	BL LCD_OutDec
	
PrintDigit
	POP {LR, R1}
	ADD R0, R1, #48
	PUSH {LR, R1}
	BL ST7735_OutChar
	POP {LR, R1}
    BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	LDR R1, =9999
	CMP R0, R1
	BGT Overflow
	CMP R0, #0
	BLT Overflow
	B OutFix
Overflow
	PUSH {R4, LR}
	MOV R0, #'*'
	BL ST7735_OutChar
	MOV R0, #'.'
	BL ST7735_OutChar
	MOV R0, #'*'
	BL ST7735_OutChar
	MOV R0, #'*'
	BL ST7735_OutChar
	MOV R0, #'*'
	BL ST7735_OutChar
	MOV R0, #' '
	BL ST7735_OutChar
	POP {R4, LR}
	BX LR
OutFix

	PUSH {R4, LR}

;***ALLOCATE***
	MOV R4, #1000
	MOV R1, R0 ; 1st Digit
	UDIV R1, R1, R4
	MUL R1, R1, R4
	
	MOV R4, #100
	MOV R2, R0 ; 2nd Digit
	UDIV R2, R2, R4
	MUL R2, R2, R4
	SUB R2, R2, R1
	
	MOV R4, #10
	MOV R3, R0 ; 3rd Digit
	UDIV R3, R3, R4
	MUL R3, R3, R4
	SUB R3, R3, R1
	SUB R3, R3, R2
	
	MOV R12, R0 ; 4th Digit
	SUB R12, R12, R3
	SUB R12, R12, R2
	SUB R12, R12, R1
	
	MOV R4, #1000
	UDIV R1, R1, R4
	MOV R4, #100
	UDIV R2, R2, R4
	MOV R4, #10
	UDIV R3, R3, R4

	PUSH {R1, R2, R3, R12}

;***ACCESS***
	ADD R0, R1, #48
	BL ST7735_OutChar
	
	MOV R0, #'.'
	BL ST7735_OutChar
	
	LDR R2, [SP, #hun]
	ADD R0, R2, #48
	BL ST7735_OutChar
	
	LDR R3, [SP, #ten]
	ADD R0, R3, #48
	BL ST7735_OutChar
	
	LDR R12, [SP, #one]
	ADD R0, R12, #48
	BL ST7735_OutChar
	
	MOV R0, #' '
	BL ST7735_OutChar
	
;***DEALLOCATE***
	ADD SP, SP, #16
	
	POP {R4, LR}
    BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
