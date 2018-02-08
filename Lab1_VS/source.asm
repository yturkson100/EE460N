.ORIG x3000
    LEA R0, FST
    LEA R1, NEG
    LDW R1, R1, #0 ; load R1 with -2
    NOT R1, R1
    ADD R1, R1, #1  ; make R1 positive
    ADD R0, R0, R1 ; add 2 to R0 which is an address
    LDW R1, R0, #0
    LEA R2, SCD
    LDW R3, R2, #0 ; load 4 into R3
    RSHFA R3, R3, #1 ; divide R3 by 2 by shifting one bit
    ADD R2, R2, R3 ; add 2 to R2 which is an address
    LDW R2, R2, #0
    ADD R3, R1, R2 ; should be 0
    BRNP WRNG ; if value isn't 0, branch to end of program
    JSR WOW ; if value is 0, branch to instructions that are created with .FILL
    LEA R6, NXT ; load R6 with intent to jump later
    LEA R1, NEG
    LDW R1, R1, #0 ; load R1 with -2
    AND R4, R3, R1 ; R4 should go from x0005 to x0000
    BRNP WRNG
    JMP R6 ; jumps to end of program if R4 is not 0
    NOP
NXT LEA R0, NEG
    STW R4, R0, #0 ; store x0000 in NEG
    LDW R0, R0, #0
    BRNP WRNG
    ADD R4, R4, #4 ; reward for getting all the way haha
WRNG HALT

FST .FILL x0005
    .FILL x0000
SCD .FILL x0004
    .FILL x0000
THD .FILL x000A
NEG .FILL xFFFE ; -2
FVE .FILL x0505
WOW .FILL xE9FE ;LEA R4, FVE
    .FILL x2900 ;LDB R4, R4, #0
    .FILL x1903 ;ADD R4, R3, R4 ; R4 should be 5 after this
    .FILL xC1C0 ;RET
.END