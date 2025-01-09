# CAN-

TX (Transmission) Flow:

Initialize CAN Socket:
  Open a raw CAN socket using socket(PF_CAN, SOCK_RAW, CAN_RAW).

Configure CAN Interface:
  Get the interface index using ioctl() with the SIOCGIFINDEX command.
  Configure the CAN interface (e.g., can0) and bring it up using:
    sudo ip link set can0 type can bitrate 500000
    sudo ip link set can0 up

Create CAN Frame:
  Construct the CAN frame (struct can_frame), including CAN ID, data length code (DLC), and the data to be sent.
  
Bind Socket to Interface:
  Bind the socket to the specified CAN interface using bind(). The socket will now listen for CAN frames on that interface.

Send CAN Frame:
  Use write() to send the frame to the CAN bus.

Close Socket:
  After sending the data, close the socket using close() to release resources.

******************************************************************************************************************************
RX (Reception) Flow:

Initialize CAN Socket:
  Open a raw CAN socket using socket(PF_CAN, SOCK_RAW, CAN_RAW).

Configure CAN Interface:
  Get the interface index using ioctl() with the SIOCGIFINDEX command.
  Ensure the CAN interface (can0) is up with:
    sudo ip link set can0 type can bitrate 500000
    sudo ip link set can0 up
    
Bind Socket to Interface:
  Bind the socket to the specified CAN interface (can0) using bind().

Listen for CAN Frames:
  Continuously listen for incoming CAN frames using read() or recv(). The read() function blocks until a CAN frame is received.

Process the Received Frame:
  Upon receiving a frame, extract the data (CAN ID, data length, and the payload).
  Process the received data as needed (e.g., decoding or displaying the data).
  
Close Socket:
  After processing the data, close the socket using close()

  ****************************************************************************************************************************
  
