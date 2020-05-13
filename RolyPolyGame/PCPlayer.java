import java.awt.event.KeyEvent;

import jssc.SerialPortException;


public class PCPlayer{
	
	public static void main(String[] args) throws SerialPortException{
		SerialComm port = new SerialComm("/dev/cu.usbserial-DN050YXU");
		System.out.println("ready to start");
		
		long lastTime_left = 0;
		long lastTime_right = 0;
		long lastTime_down = 0;
		long lastTime_up = 0;
		int debounce = 600;
		
		while(true) {
			if (StdDraw.isKeyPressed(KeyEvent.VK_LEFT)) {	// click left arrow key
				if ((System.currentTimeMillis()-lastTime_left) > debounce) {
					System.out.println("Left");
					port.writeByte((byte) 0);	// write to the Arduino which reads as 
												// predator going left
					lastTime_left = System.currentTimeMillis();
				}
			}
			else if(StdDraw.isKeyPressed(KeyEvent.VK_RIGHT)) {	// click right arrow key
				if ((System.currentTimeMillis()-lastTime_right) > debounce) {
					System.out.println("Right!");
					port.writeByte((byte) 1);
					lastTime_right = System.currentTimeMillis();
				}	
			}
			else if (StdDraw.isKeyPressed(KeyEvent.VK_UP)) {	// click up arrow key
				if ((System.currentTimeMillis()-lastTime_up) > debounce) {
					System.out.println("Up!");
					port.writeByte((byte) 2);
					lastTime_up = System.currentTimeMillis();
				}
			}
			else if (StdDraw.isKeyPressed(KeyEvent.VK_DOWN)) {	// click down arrow key
				if ((System.currentTimeMillis()-lastTime_down) > debounce) {
					System.out.println("Down!");
					port.writeByte((byte) 3);
					lastTime_down = System.currentTimeMillis();
				}
			}
		}
	}
}