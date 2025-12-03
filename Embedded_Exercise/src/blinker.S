
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
    push {r0,r1,r2,r3,lr}

    LDR R0, =0x41200000    // LED hardware
    LDR R1, =state
    LDRB R1, [R1]          // current LED state

    LDR R2, =dir
    LDRB R3, [R2]          // current direction

    CMP R3, #0
    BEQ shift_left
    B shift_right

shift_left:
    CMP R1, #8             // Are we at LD3?
    BEQ change_dir_right

    LSL R1, R1, #1         // shift left
    B write_back

change_dir_right:
    MOV R3, #1
    STRB R3, [R2]          // change direction
    B shift_right           // shift right immediately next

shift_right:
    CMP R1, #1             // Are we at LD0?
    BEQ change_dir_left

    LSR R1, R1, #1         // shift right
    B write_back

change_dir_left:
    MOV R3, #0
    STRB R3, [R2]
    B shift_left

write_back:
    STRB R1, [R0]          // write LED
    LDR R4, =state
    STRB R1, [R4]          // save state

    pop {r0,r1,r2,r3,lr}
    bx lr


