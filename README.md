# Alien Shooter Game

A small shoot-em-up game that runs on a PYNQ-Z1 board and draws everything on
an 8×8 RGB LED matrix. 

## The game

You control a ship at the bottom of the screen. An alien moves left and right
along the top, and you try to shoot it before it gets you.

- **BTN0 / BTN1** move the ship right and left
- **BTN2** fires
- **BTN3** restarts the game at any time

You can have three bullets in the air at once. Each hit on the alien lights one
green LED, and three of them wins the game. If the alien reaches you, you lose a
life, shown as a red LED — three reds and it's over.

## How it works

 The program runs directly on the ARM
processor inside the Zynq chip and talks to the hardware through memory
addresses.

The LED matrix can only show one row at a time, so the display is refreshed by
hand. A timer interrupt fires 800 times per second and draws the next row each
time; the eye blends the eight rows into a full picture. A second timer, running
at 5 Hz, moves the alien and updates the bullets. Keeping these two tasks on
separate timers is what makes the picture stable while the game is still
playable.

Buttons are handled by interrupts as well, so a press is never missed while the
display is being redrawn.

## Files

```
src/main.c              main program, interrupt handlers, button mapping
src/Pixel.c / Pixel.h   LED matrix driver and all the game logic
src/Interrupt_setup.c   timer and button interrupt setup
src/blinker.S           small assembly routine that drives the board LEDs
src/lscript.ld          linker script
src/platform.c          board startup code
```

## Building and running

The project is a Xilinx SDK/Vitis application for the `PYNQ-Z1-matrix`
platform (ARM Cortex-A9, standalone).

1. Open the project in Xilinx SDK or Vitis.
2. Build it.
3. Program the FPGA with the PYNQ-Z1 matrix design.
4. Run the application on the board.

The matrix and the timer are set up by the platform design, so the hardware
project has to be loaded before the software will show anything.

