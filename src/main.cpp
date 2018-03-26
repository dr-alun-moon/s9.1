#include <mbed.h>
#include <EthernetInterface.h>
#include <rtos.h>
#include <mbed_events.h>
#include <FXOS8700Q.h>

#include <C12832.h>

#include <udp.h>

Thread eventhandler;
EventQueue eventqueue;

/* display */
C12832 lcd(D11, D13, D12, D7, D10);

I2C i2c(PTE25, PTE24);
FXOS8700QAccelerometer acc(i2c, FXOS8700CQ_SLAVE_ADDR1);

/* YOU will have to hardwire the IP address in here */
SocketAddress server("192.168.1.13",65200);
SocketAddress dash("192.168.1.13",65201);

EthernetInterface eth;
UDPSocket udp;
char buffer[512];
char line[80];
// int send(char *m, size_t s) {
//     nsapi_size_or_error_t r = udp.sendto( server, m, s);
//     return r;
// }
// int receive(char *m, size_t s) {
//     SocketAddress from;
//     nsapi_size_or_error_t r = udp.recvfrom(&from, m,s );
//     return r;
// }
/* Input from Potentiometers */
AnalogIn  left(A0);
AnalogIn right(A1);

float constrain(float value, float lower, float upper) {
    if( value<lower ) value=lower;
    if( value>upper ) value=upper;
    return value;
}
float throttle = 0.0;
float last1 = 0;
float last2 = 0;
float integral = 0;
void controller(void) {
    char buffer[512];
    motion_data_units_t a;
    acc.getAxis(a);
    /* Down is +z */
    lcd.locate(0,0);
    float m = sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
    lcd.printf("axis x:%3.1f y:%3.1f z:%3.1f (%3.1f)\n",a.x,a.y,a.z,m);
    a.x/=m;a.y/=m;a.z/=m;
    lcd.printf("axis x:%3.1f y:%3.1f z:%3.1f (%3.1f)\n",a.x,a.y,a.z,m);
    float r = asin(a.x)*180/3.141592;
    float p = asin(a.y)*180/3.141592;
    lcd.printf("r: %.2f  p:%.2f",r,p);
    sprintf(buffer,"roll:%.2f\npitch:%.2f\n",r,p);
    udp.sendto( dash, buffer, strlen(buffer));
}

int main() {
    acc.enable();

    printf("conecting \n");
    eth.connect();
    //const char *ip = eth.get_ip_address();
    //printf("IP address is: %s\n", ip ? ip : "No IP");

    udp.open( &eth);
    SocketAddress source;
        printf("sending messages to %s/%d\n",
                server.get_ip_address(),  server.get_port() );

    eventhandler.start(callback(&eventqueue, &EventQueue::dispatch_forever));

    eventqueue.call_every(50,controller);
    callback(controller);
    while(1){

    }
}
