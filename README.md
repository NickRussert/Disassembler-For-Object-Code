# Disassembler-For-Object-Code

This code takes in 2 files, a .obj file and a .sym file. Its intended purpose it to take object code and turn it into SIC/XE source code. This is why it is a disassembler because it works backwards from what an assembler would do. 

example input:

obj file -
HAssign0000000005A2
T0000000A691002C61722BF022FFF
T0002C71CB400F1050000010005000001E32FFA332FFA53AFEADF2FEA031002E3
M00000105
M0002E005
E000000

sym file
Symbol  Address Flags:
----------------------
FIRST   000000  R

Name    Lit_Const  Length Address:
----------------------------------
VDEV    X'F1'      2      0002C9
WDEV    X'000001'  6      0002D0


output:
0000        Assign      START       0                      
0000        FIRST       +LDB        #02C6       691002C6   
                        BASE        02C6                   
0004                    STL         02C6        1722BF     
0007                    LDA         @02C6       022FFF     
02C7                    CLEAR       A           B400       
02C9        VDEV        BYTE        X'F1'       F1         
02CA                    LDX         #0000       050000     
02CD                    LDA         #0005       010005     
02D0        WDEV        BYTE        X'000001'   000001     
02D3                    TD          02D0        E32FFA     
02D6                    JEQ         02D3        332FFA     
02D9                    LDCH        02C6        53AFEA     
02DC                    WD          02C9        DF2FEA     
02DF                    +LDA        02E3        031002E3   
                        END         Assign                 

The last step in the completion of this project is going to be replacing the operand column which at the moment is a number, with the intended label. 
