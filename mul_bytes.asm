                .ORIG x3000
                LEA R2, NUM1
                LDB R2, R2, #0

                LEA R3, NUM2
                LDB R3, R3, #0

                LEA R0, MULTIPLE
                LDW R0, R0, #0
                STB R2, R0, #0
                LDB R0, R0, #0

                LEA R1, MULTIPLIER
                LDW R1, R1, #0
                STB R3, R1, #0
                LDB R1, R1, #0

                AND R2, R2, #0  ; Result
                AND R3, R3, #0  ; Overflow

LOOP            AND R4, R1, #1

                BRz SHIFT

                ADD R2, R2, R0 ; Add multiplicand to the result

SHIFT           LSHF R0, R0, #1 ; Multiplicand * 2
                RSHFL R1, R1, #1 ; Right shift multiplier

                BRp LOOP

                LEA R4, MASK
                LDW R4, R4, #0

                AND R4, R4, R2 ; Check for overflow

                BRz STORE
                
                ADD R3, R3, #1 ; Set overflow bit

STORE           LEA R4, RESULT
                LDW R4, R4, #0
                STB R2, R4, #0

                LEA R4, OVERFLOW
                LDW R4, R4, #0
                STB R3, R4, #0

                HALT

NUM1            .FILL x0001
NUM2            .FILL x0003
MASK            .FILL xFF00
MULTIPLE        .FILL x3100
MULTIPLIER      .FILL x3101
RESULT          .FILL x3102
OVERFLOW        .FILL x3103

                .END