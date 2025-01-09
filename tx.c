/*Before running this code, you must ensure that the can0 interface is up and running. You can configure it using these commands:
    sudo ip link set can0 type can bitrate 500000
    sudo ip link set can0 up */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    int s; // Socket descriptor
    struct ifreq ifr; // Interface request structure
    struct can_frame frame; // CAN frame structure

    // Create a raw CAN socket
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Specify the CAN interface (can0 in this case)
    strcpy(ifr.ifr_name, "can0");

    // Get the interface index
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl SIOCGIFINDEX failed");
        return 1;
    }

    // Set the CAN interface up (using ioctl)
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    // Bind the socket to the CAN interface
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Now you can send or receive CAN frames, for example, sending a test frame
    frame.can_id = 0x123; // CAN ID
    frame.can_dlc = 1;    // Data length code (1 byte of data)
    frame.data[0] = 0xFF; // Example data byte

    if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("Write failed");
        return 1;
    }

    printf("CAN frame sent!\n");

    // Close the socket
    close(s);
    return 0;
}
