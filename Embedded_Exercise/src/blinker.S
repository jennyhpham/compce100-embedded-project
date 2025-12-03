
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
	dir: .byte 1		//8-bit variable for direction
pos: .byte 0        // 8-bit variable for current LED position (0..3)


.text

.global blinker


blinker:

	// Push used registers to stack
	push {r0-r4, lr}

blink_loop:
	// r0 = base address of LED register
	LDR  r0, =0x41200000

	// Load current position (0..3) from 'pos'
	LDR  r1, =pos
	LDRB r2, [r1]          // r2 = pos

	// Compute LED mask: (1 << pos) into r3
	MOV  r3, #1
	LSL  r3, r3, r2

	// Write mask to LED register (only LD0..LD3 used)
	STRB r3, [r0]

	// -----------------------
	// Simple delay loop
	// -----------------------
	// Outer loop count in r4 (increase this if the blinking is too fast)
	MOV  r4, #0x40        // adjust for visible speed

delay_outer:
	MOV  r2, #0xFF        // inner loop counter

delay_inner:
	SUBS r2, r2, #1
	BNE  delay_inner

	SUBS r4, r4, #1
	BNE  delay_outer

	// -----------------------
	// Update position based on direction
	// -----------------------
	// Load dir (0 = right, 1 = left)
	LDR  r0, =dir
	LDRB r2, [r0]         // r2 = dir

	// Reload pos
	LDR  r1, =pos
	LDRB r3, [r1]         // r3 = pos

	CMP  r2, #0
	BNE  move_left

	// ---- Move right: pos++ ----
	ADD  r3, r3, #1
	STRB r3, [r1]

	// If pos < 3, keep going right
	CMP  r3, #3
	BLT  blink_loop

	// Reached LD3 (pos == 3), change direction to left
	MOV  r2, #1
	STRB r2, [r0]
	B    blink_loop

move_left:
	// ---- Move left: pos-- ----
	SUB  r3, r3, #1
	STRB r3, [r1]

	// If pos > 0, keep going left
	CMP  r3, #0
	BGT  blink_loop

	// Reached LD0 (pos == 0), change direction to right
	MOV  r2, #0
	STRB r2, [r0]

	B    blink_loop

	// NOTE: We never reach here, blinker runs forever.
	// If you need it to return instead of infinite loop,
	// replace 'blink_loop' tail branches with 'b done' and
	// uncomment the code below.
done:
	pop {r0-r4, lr}				//Pop registers from stack
	bx lr						//Return to place where this function has been called.




