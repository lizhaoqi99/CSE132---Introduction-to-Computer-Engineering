I authored a communication protocol between Arduino and PC on both Arduino C and Java 
to display the “roly poly predator prey” game on a 5-by-7 LED screen using time-division
multiplexing. While the predator player can use the cursor keys on the PC to make a move
which is represented by a 2-by-2 grid of dots blinking at 4Hz, the other player can move 
the prey by tilting the Arduino board using the accelerometer that is represented by a 
solid dot on the screen moving at half the speed. After each round, the score is displayed 
on the board.
