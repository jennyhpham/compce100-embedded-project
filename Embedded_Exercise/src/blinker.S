
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
    push {r0, r1, r2, r3, r4, lr}   // save registers

    LDR  r0, =0x41200000             // LED hardware address

    // Load current LED state
    LDR  r1, =state
    LDRB r1, [r1]

    // Output to LEDs
    STRB r1, [r0]

    // Load direction
    LDR  r2, =dir
    LDRB r3, [r2]

    CMP  r3, #0
    BEQ  go_left
    B    go_right

go_left:
    LSL  r1, r1, #1
    CMP  r1, #0x10
    BNE  write_back_left
    MOV  r1, #8
    MOV  r3, #1
    STRB r3, [r2]       // update dir
write_back_left:
    B    write_back

go_right:
    LSR  r1, r1, #1
    CMP  r1, #0
    BNE  write_back_right
    MOV  r1, #1
    MOV  r3, #0
    STRB r3, [r2]       // update dir
write_back_right:

write_back:
    // Save new state
    LDR  r4, =state     // use r4 to avoid r2 conflict
    STRB r1, [r4]

    pop  {r0, r1, r2, r3, r4, lr}
    bx   lr

