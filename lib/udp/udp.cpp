#include <mbed.h>
#include <rtos.h>
#include <EthernetInterface.h>
#include <mbed_events.h>
UDPSocket udp;
SocketAddress server("192.168.1.13",65200);

int send(char *m, size_t s) {
    nsapi_size_or_error_t r = udp.sendto( server, m, s);
    return r;
}
int receive(char *m, size_t s) {
    SocketAddress from;
    nsapi_size_or_error_t r = udp.recvfrom(&from, m,s );
    return r;
}
