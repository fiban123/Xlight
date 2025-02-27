import serial
import time

def precise_sleep(duration):
    """Busy-wait for the given duration (in seconds) using time.perf_counter()."""
    target_time = time.perf_counter() + duration
    while time.perf_counter() < target_time:
        pass

def send_dmx_frame(ser: serial.Serial, channel_values: bytearray):
    """
    Sends a DMX512 frame over the given serial port.
    
    DMX512 frames consist of:
      - A break (a low signal for at least 88 microseconds)
      - A mark after break (a high signal for at least 8 microseconds)
      - A start code (usually 0)
      - Up to 512 channels of data (here we use 4 channels for RGBW)
    
    :param ser: An open serial.Serial object configured for DMX.
    :param channel_values: A list of channel values (0-255) for R, G, B, W.
    """
    # Construct the DMX frame: start code (0) + channel data
    packet = bytearray([0])
    packet.extend(channel_values)

    # Initiate break condition: set the line low
    ser.break_condition = True
    precise_sleep(0.1)
    ser.break_condition = False
    precise_sleep(0.1)  # 12 microseconds mark after break
    
    ser.write(packet)
    ser.flush()

def main():
    port = 'COM7'
    baud_rate = 250000  # DMX512 uses 250 kbps
    try:
        ser = serial.Serial(port, baud_rate, bytesize=8, parity='N', stopbits=2)
    except Exception as e:
        print(f"Error opening serial port {port}: {e}")
        return

    print("DMX Controller started. Input values for R, G, B, and W (0-255). Press Ctrl+C to exit.")
    
    try:
        while True:
            red   = 255
            green = 255
            blue  = 255
            white = 23

            # Clamp values to the valid range 0-255.
            
            send_dmx_frame(ser, bytearray([red, green, blue, white]))
            print(f"Sent DMX data: Red={red}, Green={green}, Blue={blue}, White={white}")
            # A short delay before sending the next frame; adjust as needed.
            #precise_sleep(0.0001);
    except KeyboardInterrupt:
        print("\nExiting DMX Controller.")
    finally:
        ser.close()
main()