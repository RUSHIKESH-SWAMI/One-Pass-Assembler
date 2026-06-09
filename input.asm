COPY    START   1000
FIRST   LDA     THREE
        ADD     FIVE
        STA     ALPHA
        JMP     NEXT
ALPHA   RESW    1
FIVE    WORD    5
THREE   WORD    3
NEXT    LDA     ALPHA
        RSUB
        END     FIRST
