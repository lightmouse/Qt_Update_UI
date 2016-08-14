import sys
sys.path.append('../../Python')
import canlib

if __name__ == '__main__':
    
    print("Initializing Canlib")
    cl = canlib.canlib()
    
    print("Opening channel 0")
    ch = cl.openChannel(0, canlib.canOPEN_ACCEPT_VIRTUAL)
    
    print("Setting bitrate to 250 kb/s")
    ch.setBusParams(canlib.canBITRATE_250K)
    
    print("Going on bus")
    ch.busOn()
    
    print("Sending a message")
    msgId = 123
    data = [1, 2, 3, 4, 5, 6, 7, 8]
    flags = 0
    ch.writeWait(msgId, data, flags, 50)
    
    print("Going off bus")
    ch.busOff()
    
    print("Closing channel")
    ch.close()
