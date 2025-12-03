
//ARM INSTRUCTIONS
//Instructions are conditional
//32-bit instruction


//How to load data from memory:
//LDR R0, =variable 	(Load address to register. You can use any global variable defined in main.c, or variable found here)
//LDR R1, [R0] 			(Load data from address, in this case 32-bit data)
//LDRB R1, [R0] 		(Load data from address, in this case 8-bit data)
//******
//LDR R0, =0x123456		(Load address to register, Use only if you know the memory address)
//LDR R1, [R0]			(load data from address, in this case 32-bit data)
//LDRB R1, [R0]			(load data from address, in this case 8-bit data)

//How to store data to memory:
//STR R1,[R0]			(R1 contains the data and R0 contains the address where  to store, 32-bit)
//STRB R1,[R0]			(R1 contains the data and R0 contains the address where  to store, 8-bit)

.data
state: .byte 1
dir:   .byte 0    

.global blinker
.text

blinker:
    push {r0, r1, r2, r3, lr}

    LDR R0, =0x41200000    // LED output register
    LDR R2, =dir           // direction variable
    LDRB R3, [R2]          // R3 = direction (0=left,1=right)

    LDR R1, =state         // load current LED state (1,2,4,8)
    LDRB R1, [R1]

    CMP R3, #0
    BEQ go_left
    B go_right

go_left:
    LSL R1, R1, #1
    CMP R1, #0x10
    BNE write_back
    MOV R1, #8
    MOV R3, #1          // change direction to right
    STRB R3, [R2]
    B write_back

go_right:
    LSR R1, R1, #1
    CMP R1, #0
    BNE write_back
    MOV R1, #1
    MOV R3, #0          // change direction to left
    STRB R3, [R2]

write_back:
    STRB R1, [R0]        // write LED (8-bit)

    LDR R4, =state       // FIX: must load address into register
    STRB R1, [R4]        // save new state

    pop {r0, r1, r2, r3, lr}
    bx lr



