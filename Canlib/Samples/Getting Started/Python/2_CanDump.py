import sys
sys.path.append('../../Python')
import canlib

#Prints a message to screen
def dumpMessage(id, msg, dlc, flag, time):
    if (flag & canlib.canMSG_ERROR_FRAME != 0):
        print("***ERROR FRAME RECEIVED***")
    else:
        dataStr = ""
        for i in range(0, 8):
            if(i < len(msg)):
                dataStr += "{0:3} ".format(msg[i])
            else:
                dataStr += "    "
        print("{0:0>8b}  {1}   {2}   {3}".format(id, dlc, dataStr, time))

#Listens for messages on the channel and prints any received messages
def dumpMessageLoop (ch):
    finished = False
    print("   ID    DLC DATA                                Timestamp")
    
    while not finished:
        try: 
            id, msg, dlc, flag, time = ch.read(50)
            hasMessage = True
            while hasMessage: 
                dumpMessage(id, msg, dlc, flag, time)
                try:
                    id, msg, dlc, flag, time = ch.read()
                except(canlib.canNoMsg) as ex:
                    hasMessage = False
                except (canlib.canError) as ex:
                    print(ex)
                    finished = True
        except(canlib.canNoMsg) as ex:
            None
        except (canlib.canError) as ex:
            print(ex)
            finished = True


if __name__ == '__main__':
    #Initialization
    cl = canlib.canlib()
    ch = cl.openChannel(0, canlib.canOPEN_ACCEPT_VIRTUAL)
    ch.setBusParams(canlib.canBITRATE_250K)
    ch.busOn()
    
    #Start listening for messages
    dumpMessageLoop(ch)
    
    #Channel teardown
    ch.busOff()
    ch.close()
    
    
