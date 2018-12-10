/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This 
    library has basic methods of various sensors and communications so that you can make what you want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "LSM303D.h"
#include "IR.h"
#include "Beep.h"
#include "mqtt_sender.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>
#include <stdlib.h>
/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

#if 0
// Sumo-battle
void zmain(void)
{
struct accData_ data;
motor_start();
motor_forward(0,0);


IR_Start();

TickType_t start;
TickType_t end;
TickType_t time;


struct sensors_ dig;
struct sensors_ ref;

reflectance_start();
reflectance_set_threshold(11000, 11000, 11000, 11000, 11000, 11000); // set center sensor threshold to 11000 and others to 9000
Ultra_Start();

while (SW1_Read() == 1) { //Odotetaan napinpainallusta koodin aloitukseen
if (SW1_Read() == 0) {
break;
}
vTaskDelay(100); 
}

for (;;) { //Aloitus looppi, mennään aloitusviivalle ja odotetaan IR-signaalia aloitukseen
motor_forward(40, 150);

reflectance_read(&ref);
reflectance_digital(&dig);

if (dig.r1 == 1 && dig.r2==1 && dig.r3==1 && dig.l1==1 && dig.l2==1 && dig.l3==1) {
motor_forward(0,0);
print_mqtt("Zumo042/ready", "line<br>");
IR_flush();
IR_wait();
start = xTaskGetTickCount();
print_mqtt("Zumo042/start", "%d <br>", start);
motor_forward(100, 250);

break;
}
}
for(;;)
{//sisällä ringissä peruuttaa viivalle tultaessa//
motor_forward(200, 0);

reflectance_read(&ref);
reflectance_digital(&dig);
LSM303D_Read_Acc(&data);

if (abs(data.accX) > 5000) {
    print_mqtt("Zumo042/hit", "hit");
}
else if (abs(data.accY) > 5000) {
    print_mqtt("Zumo042/hit", "hit");
}

if (SW1_Read()==0) {
end=xTaskGetTickCount();
time=end-start;
print_mqtt("Zumo042/stop", "%d <br>", end);
print_mqtt("Zumo042/time", "%d <br>", time);
}

if ( dig.l3==1 || dig.l2==1) {
motor_forward(0,0);
print_mqtt("Zumo042/ready", "line");
motor_backward(200, 0);
motor_turn(255,0,600);
}
else if (dig.r3==1 || dig.r2==1){
motor_forward(0,0);
print_mqtt("Zumo042/ready", "line");
motor_backward(200, 0);
motor_turn(0,255,600);
}

if (Ultra_GetDistance()<150) {
    motor_forward(255,100);

}

}
}
#endif

#if 0
//Projektin kilparata

void zmain(void)
{
        
    motor_start();
    motor_forward(0,0);
    
    TickType_t start;
    TickType_t end;
    TickType_t currenttime;
    int time=0;
    int Ratakulkutarkistaja=1; //Määritellään onko robotti radalla vai ei, 1 = on radalla (alkuarvo)
                               //ja 0 = ei ole radalla
    
    
    IR_Start();
    
    struct sensors_ dig;
    struct sensors_ ref;

    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    while (SW1_Read() == 1) { //Odotetaan napinpainallusta koodin aloitukseen
        if (SW1_Read() == 0) {
            break;
        }
        vTaskDelay(100); 
    }
    
    for (;;) { //Aloitus looppi, mennään aloitusviivalle ja odotetaan IR-signaalia aloitukseen
        motor_forward(40, 150);
        
        reflectance_read(&ref);
        reflectance_digital(&dig);
        
        if (dig.r1 == 1 && dig.r2==1 && dig.r3==1 && dig.l1==1 && dig.l2==1 && dig.l3==1) {
            motor_forward(0,0);
            print_mqtt("Zumo042/ready", "line<br>");
            IR_flush();
            IR_wait();
            motor_forward(255, 150);
            start = xTaskGetTickCount();
            print_mqtt("Zumo042/start", "%d ms<br>", start);
            
            break;
        }
    }
    
    for (;;) { //Ajo-loop
                reflectance_read(&ref);
                reflectance_digital(&dig);
                
                if (dig.r1==0 && dig.l1==0) {
                    currenttime = xTaskGetTickCount();
                    print_mqtt("Zumo042/miss", "%d<br>", currenttime);
                    Ratakulkutarkistaja= 0;
                }
                if (dig.r1 == 1 && dig.r2==1 && dig.l1==1 && dig.l2==1 && dig.l3==1 && dig.r3==1)
                    {
                    //Pysähdys risteykseen
                    end = xTaskGetTickCount();
                    time = end - start;
                    print_mqtt("Zumo042/stop", "%d ms<br>", end);
                    print_mqtt("Zumo042/time", "%d ms<br>", time);
                    motor_stop();
                    vTaskDelay(15000);
                    }
                    
                if (dig.r1 == 1   && dig.l1==1)
                    {
                    //mene suoraan
                    motor_forward(255,0);
                    if (Ratakulkutarkistaja!= 1) {
                        currenttime= xTaskGetTickCount();
                        print_mqtt("Zumo042/line", "%d<br>", currenttime);
                        Ratakulkutarkistaja = 1;
                        
                    }
                    }
                else if (dig.l1==1)
                    {
                    //käänny oikealle
                    
                    motor_forward(0,0);
                    motor_turn(255,175,0);
                    }
        
                else if (dig.r1 == 1)
                    {
                    //käänny vasemmalle
                    motor_forward(0,0);
                    motor_turn(175,255,0);
                    }

                else if (dig.l2==1 && dig.l3==1)
                    {
                    //käänny vahvasti oikealle
                    motor_forward(0,0);
                    motor_turn(15,255,0);
                    }

                else if (dig.r3==1)
                    {
                    //käänny vahvasti vasemmalle
                    motor_forward(0,0);
                    motor_turn(255,15,0);
                    }
    }
    
}
#endif

#if 0
//projekti MAZE ///

void zmain(void) {

IR_Start();

IR_flush();

Ultra_Start();

struct sensors_ dig;
struct sensors_ ref;


reflectance_start();
reflectance_set_threshold(9000, 9000, 9000, 9000, 9000, 9000); 

motor_start(); 
TickType_t start;
TickType_t intersection;
TickType_t stop;
int time=0;
float koordinaatti=0.0;
motor_forward(0,0); 

while (SW1_Read() == 1) { 
if (SW1_Read() == 0) {
break;
}
vTaskDelay(100); 
}


for (;;) { //alku risteykseen, uudelleenkäytetty//
motor_forward(40, 150);

reflectance_read(&ref);
reflectance_digital(&dig);

if (dig.r1 == 1 && dig.r2==1 && dig.r3==1 && dig.l1==1 && dig.l2==1 && dig.l3==1) {
motor_forward(0,0);
print_mqtt("Zumo042/ready", "line<br>");
IR_flush();
IR_wait();
motor_forward(255, 100);
start = xTaskGetTickCount();
print_mqtt("Zumo042/start", "%d ms<br>", start); 
break;

}
} 
for (;;) { //Ajo-loop
    reflectance_read(&ref);
    reflectance_digital(&dig); 
    Ultra_GetDistance();

    if (Ultra_GetDistance()<20) {
    motor_forward(0,0);
    motor_turn(255,175,1);

    }
    if (dig.r1 == 1 && dig.r2==1 && dig.l1==1 && dig.l2==1 && dig.l3==1 && dig.r3==1)
    {
    //Pysähdys risteykseen
    intersection = xTaskGetTickCount();
    time = intersection - start;
    print_mqtt("Zumo042/time", "%d ms<br>", time);
    /// print koordinaatit myös //
    koordinaatti +=1;

    }

    if (dig.r1 == 1 && dig.l1==1)
    {
    //mene suoraan
    motor_forward(255,1);
    }
    else if (dig.l1==1)
    {
    //käänny oikealle 
    motor_forward(0,0);
    motor_turn(255,175,1);
    }

    else if (dig.r1 == 1)
    {
    //käänny vasemmalle
    motor_forward(0,0);
    motor_turn(175,255,1);
    }

    else if (dig.l2==1 && dig.l3==1)
    {
    //käänny vahvasti oikealle
    motor_forward(0,0);
    motor_turn(25,255,1);
    }

    else if (dig.r3==1)
    {
    //käänny vahvasti vasemmalle
    motor_forward(0,0);
    motor_turn(255,25,1);
    }

    else if (SW1_Read() == 0) {
    motor_forward(0,0);
    stop = xTaskGetTickCount();
    print_mqtt("Zumo042/stop", "%d ms<br>", stop);
    break;
    }
}

} 


#endif

#if 1
//Projektin esterata

void right_turn(void);
void left_turn(void);
void full_turn(void);
void risteykseen_ajo(void);
void risteyksesta_lahto(void);

void zmain(void) {
    motor_start();
    motor_forward(0,0);
    //int suunta = 0; //Suunta-muuttuja maalin nähden, 0 = eteenpäin, 1 = vasen, 2 = oikea, 3 = taaksepäin
    
    //TickType_t start;
    //TickType_t end;
    //int time=0;
    
    IR_Start();
    Ultra_Start();

    //struct sensors_ dig;
    //struct sensors_ ref;

    reflectance_start();
    //reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    while (SW1_Read() == 1) { //Odotetaan napinpainallusta koodin aloitukseen
        if (SW1_Read() == 0) {
            break;
        }
        vTaskDelay(100); 
    }
    
    /*for (;;) { //Aloitus looppi, mennään aloitusviivalle ja odotetaan IR-signaalia aloitukseen
        motor_forward(40, 150);
        
        reflectance_digital(&dig);
        
        if (dig.r1 == 1 && dig.r2==1 && dig.r3==1 && dig.l1==1 && dig.l2==1 && dig.l3==1) {
            motor_forward(0,0);
            print_mqtt("Zumo042/ready", "line");
            IR_flush();
            IR_wait();
            motor_forward(255, 250);
            start = xTaskGetTickCount();
            print_mqtt("Zumo042/start", "%d", start);
                    
            break;
        }
        
    }*/
   
    for (;;) {
        if (Ultra_GetDistance() < 15) { //Onko suoraan edessä este
            right_turn();               //Jos on niin kääntyy oikealle
            if (Ultra_GetDistance() < 15) { //Onko oikealla este
                full_turn();                //Jos on niin 180-asteen käännös
                if (Ultra_GetDistance() < 15) { //Onko vasemmalla este
                    left_turn();                //Käännytään taaksepäin
                    risteykseen_ajo();          //Ajeetaan taakse risteykseen
                    
                }
                else {
                    risteykseen_ajo();
                    right_turn();
                }
            }
            else {
                risteykseen_ajo();
                left_turn();
            }
        }
        risteykseen_ajo();
        
    }
}

void right_turn(void) {
    motor_forward(0,0);
    
    struct sensors_ dig;
    struct sensors_ ref;
    
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
    reflectance_digital(&dig);
    
    while (dig.r3 ==0 && dig.l3 == 0) {
        motor_backward(70, 0);
        reflectance_digital(&dig);
    }
    motor_backward(0,0);
    motor_turn(70, 0, 100);
    while (dig.r1 != 1 && dig.l1 != 1) {
        motor_turn(70, 0, 0);
        reflectance_digital(&dig);
    }
    motor_turn(0,0,0);
    return ;
}

void left_turn(void) {
    motor_forward(0,0);
    
    struct sensors_ dig;
    struct sensors_ ref;
    
    reflectance_set_threshold(12000, 12000, 14000, 14000, 12000, 12000);
    reflectance_digital(&dig);
    while (dig.r3 == 0 && dig.l3 == 0) {
        motor_backward(70, 0);
        reflectance_digital(&dig);
    }
    motor_backward(0,0);
    motor_turn(0, 70, 100);
    while (dig.r1 != 1 && dig.l1 != 1) {
        motor_turn(0, 70, 0);
        reflectance_digital(&dig);
    }
    motor_turn(0,0,0);
    return ;
}

void full_turn(void) {
    motor_forward(0,0);
    
    struct sensors_ dig;
    struct sensors_ ref;
    
    reflectance_set_threshold(12000, 12000, 14000, 14000, 12000, 12000);
    reflectance_digital(&dig);
    while (dig.r3 == 1 && dig.l3 == 1) {
        motor_backward(70, 0);
        reflectance_digital(&dig);
    }
    motor_backward(0,0);
    motor_turn(70, 0, 100);
    while (dig.r1 != 1 && dig.l1 != 1) {
        motor_turn(70, 0, 0);
        reflectance_digital(&dig);
    }
    motor_turn(70, 0, 300);
    while (dig.r1 != 1 && dig.l1 != 1) {
        motor_turn(70, 0, 0);
        reflectance_digital(&dig);
    }
    
    motor_turn(0,0,0);
    return ;
}

void risteykseen_ajo(void) {
    struct sensors_ dig;
    struct sensors_ ref;
    
    reflectance_set_threshold(12000, 12000, 14000, 14000, 12000, 12000);
    
    
    for (;;) {
        reflectance_digital(&dig);
        
        if (dig.r1 == 1 && dig.r2==1 && dig.r3==1 && dig.l1==1 && dig.l2==1 && dig.l3==1) //Risteys havaittu
        {
            
            motor_forward(0,0);
            vTaskDelay(500);
            risteyksesta_lahto();
            break;
         }
         if (dig.r1 == 1 && dig.l1==1) //Suoraan ajo
         {
            //mene suoraan
            motor_forward(70,50);
         }
         else if (dig.r1==1) //Tasapainotus
         {
            //käänny oikealle 
            motor_forward(0,0);
            motor_turn(70,50,10);
            motor_turn(0,0,0);
         }
         else if (dig.l1 == 1) //Tasapainotus
         {
            //käänny vasemmalle
            motor_forward(0,0);
            motor_turn(50,70,10);
            motor_turn(0,0,0);
         }
    }
    return ;
}

void risteyksesta_lahto(void) {
    motor_forward(140, 150);
    return ;
}

#endif


#if 0
//Week 3 Assignment 2
void zmain(void)
{
   Ultra_Start();
   int Ultra_GetDistance();

    while (SW1_Read() == 1) {
        if (SW1_Read() == 0) {
            break;
        }
        vTaskDelay(100); }

    for (;;)
    {
        printf("%d \n",Ultra_GetDistance());
        motor_start();
        motor_forward(0,0);


        motor_forward(100,0);

    if( Ultra_GetDistance()<20)
    { 
        motor_stop();
        Beep(750,44);
        motor_start();
        motor_backward(100,1000);
        motor_turn(0,200,100);


}
} 
    
}
#endif


#if 0
// Hello World!
void zmain(void)
{
    printf("\nHello, World!\n");

    while(true)
    {
        vTaskDelay(100); // sleep (in an infinite loop)
    }
 }   
#endif

#if 0
//Assigment 2, week 2
void zmain(void)
{
    int age, time;
    TickType_t start;
    TickType_t end;
    
    for (;;) {
    
    printf("What is your age?");
    start = xTaskGetTickCount();
    scanf("%d", &age);
    end = xTaskGetTickCount();
    
    time = end - start;
    
    if (age <= 21) {
        if (time <  3000) {
        printf("Super fast dude!\n");
        }
        else if (time > 3000 && time < 5000) {
        printf("So mediocre\n");
        }
        else if (time > 5000) {
        printf("My granny is faster than you!\n");
        }
    }
    else if (age > 21 && age <= 50) {
        if (time <  3000) {
        printf("Be quick or be dead\n");
        }
        else if (time > 3000 && time < 5000) {
        printf("You're so average\n");
        }
        else if (time > 5000) {
        printf("Have you been smoking something illegal?\n");
        }
    }
    else if (age > 50) {
        if (time <  3000) {
        printf("Still going strong!\n");
        }
        else if (time > 3000 && time < 5000) {
        printf("You are doing ok for your age\n");
        }
        else if (time > 5000) {
        printf("Do they still allow you to drive?\n");
        }
    }
    }
}
#endif

#if 0
// Name and age
void zmain(void)
{
    char name[32];
    int age;
    
    
    printf("\n\n");
    
    printf("Enter your name: ");
    //fflush(stdout);
    scanf("%s", name);
    printf("Enter your age: ");
    //fflush(stdout);
    scanf("%d", &age);
    
    printf("You are [%s], age = %d\n", name, age);

    while(true)
    {
        BatteryLed_Write(!SW1_Read());
        vTaskDelay(100);
    }
 }   
#endif


#if 0
//battery level//
void zmain(void)
{
    ADC_Battery_Start();        

    int16 adcresult =0;
    float volts = 0;

    
    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    //BatteryLed_Write(0); // Switch led off 
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read(0) returns zero when button is pressed
    // SW1_Read(1) returns one when button is not pressed

    for(;;)
    {
        
        ADC_Battery_StartConvert(); // start sampling
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for ADC converted value
            adcresult = ADC_Battery_GetResult16(); // get the ADC value (0 - 4095)
            // convert value to Volts
            
            volts = adcresult / 1000; 
            // you need to implement the conversion
             
            // Print both ADC results and converted value
            printf("%d %.2f\n",adcresult, volts);
            
            while (volts < 2) {
                if (SW1_Read()==0) {
                    break;
                }
                BatteryLed_Write(1);
                vTaskDelay(100);
                BatteryLed_Write(0);
                vTaskDelay(100);
                
            }
            
            
        }
            vTaskDelay(500);
            
            }
       
        }
           

#endif

#if 0
// button
void zmain(void)
{
    while(1) {
        printf("Press button within 5 seconds!\n");
        int i = 50;
        while(i > 0) {
            if(SW1_Read() == 0) {
                break;
            }
            vTaskDelay(100);
            --i;
        }
        if(i > 0) {
            printf("Good work\n");
            while(SW1_Read() == 0) vTaskDelay(10); // wait until button is released
        }
        else {
            printf("You didn't press the button\n");
        }
    }
}
#endif

#if 0
// button
void zmain(void)
{
    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    // SW1_Read() returns zero when button is pressed
    // SW1_Read() returns one when button is not pressed
    
    bool led = false;
    
    for(;;)
    {
        // toggle led state when button is pressed
        if(SW1_Read() == 0) {
            led = !led;
            BatteryLed_Write(led);
            if(led) printf("Led is ON\n");
            else printf("Led is OFF\n");
            while(SW1_Read() == 0) vTaskDelay(10); // wait while button is being pressed
        }        
    }
 }   
#endif


#if 0
//ultrasonic sensor//
void zmain(void)
{
    Ultra_Start();                          // Ultra Sonic Start function
    
    while(1) {
        int d = Ultra_GetDistance();
        // Print the detected distance (centimeters)
        printf("distance = %d\r\n", d);
        vTaskDelay(200);
    }
}   
#endif

#if 0
//IR receiverm - how to wait for IR remote commands
void zmain(void)
{
    IR_Start();
    
    printf("\n\nIR test\n");
    
    IR_flush(); // clear IR receive buffer
    printf("Buffer cleared\n");
    
    bool led = false;
    // Toggle led when IR signal is received
    for(;;)
    {
        IR_wait();  // wait for IR command
        led = !led;
        BatteryLed_Write(led);
        if(led) printf("Led is ON\n");
        else printf("Led is OFF\n");
    }    
 }   
#endif



#if 0
//IR receiver - read raw data
void zmain(void)
{
    IR_Start();
    
    uint32_t IR_val; 
    
    printf("\n\nIR test\n");
    
    IR_flush(); // clear IR receive buffer
    printf("Buffer cleared\n");
    
    // print received IR pulses and their lengths
    for(;;)
    {
        if(IR_get(&IR_val, portMAX_DELAY)) {
            int l = IR_val & IR_SIGNAL_MASK; // get pulse length
            int b = 0;
            if((IR_val & IR_SIGNAL_HIGH) != 0) b = 1; // get pulse state (0/1)
            printf("%d %d\r\n",b, l);
        }
    }    
 }   
#endif

#if 0
//Week 3 assigment 1
void zmain(void)
{
    struct sensors_ ref;
    struct sensors_ dig;
    

    reflectance_start();
    reflectance_set_threshold(7000, 7000, 11000, 11000, 7000, 7000); // set center sensor threshold to 11000 and others to 9000
    motor_start();  
    
    while (SW1_Read() == 1) {
        if (SW1_Read() == 0) {
            break;
        }
        vTaskDelay(100); 
    }
    for (;;) {
        // read raw sensor values
        reflectance_read(&ref);
        // print out each period of reflectance sensors
        printf("%5d %5d %5d %5d %5d %5d\r\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);       
        
        // read digital values that are based on threshold. 0 = white, 1 = black
        // when blackness value is over threshold the sensors reads 1, otherwise 0
        reflectance_digital(&dig); 
        //print out 0 or 1 according to results of reflectance period
        printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);        
        
        if (dig.l1==1 && dig.r1==1)
        {
        //mene suoraan
        motor_forward(125,1);
        }

        else if (dig.l1==1 && dig.l2==1)
        {
        //käänny oikealle
        
        motor_forward(0,0);
        motor_turn(125,0,1);
        reflectance_digital(&dig);
        }
        
        else if (dig.r1==1 && dig.r2==1)
        {
        //käänny vasemmalle
        motor_forward(0,0);
        motor_turn(125,0,1);
        }

        else if (dig.l3==1 && dig.l2==1)
        {
        //käänny vahvasti oikealle
        motor_forward(0,0);
        motor_turn(0,150,1);
        }

        else if (dig.r3==1)
        {
        //käänny vahvasti vasemmalle
        motor_forward(0,0);
        motor_turn(150,0,1);
        }

        else if (dig.r1==1 && dig.l2==1)
        {
        //käänny heikosti vasemmalle
        motor_forward(0,0);
        motor_turn(100,0,1);
        }

        else if (dig.l1==1 && dig.r1==1 && dig.r2==1 && dig.r3==1)
        {
        //käänny heti oikealle
        motor_forward(0,0);
        motor_turn(0,125,1);
        }
        else if (dig.l3==1 && dig.r3==1)
        {
        //vasemmalle tai oikealle
        motor_forward(0,0);
        motor_turn(0,125,1);
        }
        
    }
    
}
#endif

#if 0
//reflectance
void zmain(void)
{
    struct sensors_ ref;
    struct sensors_ dig;

    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    

    for(;;)
    {
        // read raw sensor values
        reflectance_read(&ref);
        // print out each period of reflectance sensors
        printf("%5d %5d %5d %5d %5d %5d\r\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);       
        
        // read digital values that are based on threshold. 0 = white, 1 = black
        // when blackness value is over threshold the sensors reads 1, otherwise 0
        reflectance_digital(&dig); 
        //print out 0 or 1 according to results of reflectance period
        printf("%5d %5d %5d %5d %5d %5d \r\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);        
        
        vTaskDelay(200);
    }
}   
#endif


#if 0
//motor
void zmain(void)
{
    motor_start();              // enable motor controller
    motor_forward(0,0);         // set speed to zero to stop motors

    vTaskDelay(3000);
    
    motor_forward(100,2000);     // moving forward
    motor_turn(200,50,2000);     // turn
    motor_turn(50,200,2000);     // turn
    motor_backward(100,2000);    // moving backward
     
    motor_forward(0,0);         // stop motors

    motor_stop();               // disable motor controller
    
    for(;;)
    {

    }
}
#endif

#if 0
    
void zmain(void) 
//Week4 Assigment 1
{
    motor_start();
    motor_forward(0,0);
    
    IR_Start();
    
    uint32_t IR_val; 

    IR_flush();
  
    struct sensors_ dig;
    struct sensors_ ref;
    
   
    reflectance_start();
    reflectance_set_threshold(9000, 9000, 9000, 9000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    int muuttuja = 0;
    
    while (SW1_Read() == 1) {
        if (SW1_Read() == 0) {
            break;
        }
        vTaskDelay(100); 
    }
    for (;;) {
        
        motor_forward(40, 150);
        
        reflectance_read(&ref);
        reflectance_digital(&dig);
        
        if (dig.r1 == 1 && dig.r2==1 && dig.l1==1 && dig.l2==1 ) {
            muuttuja +=1;
            
            if (muuttuja == 1 || muuttuja > 6) {
                motor_forward(0,0);
                
                
                for (;;) {
                if(IR_get(&IR_val, portMAX_DELAY)) {
                    IR_flush();
                   break;
                }
                
                vTaskDelay(50); 
                
                }
               
            }
            
            
        }
    }
    }
    

#endif
#if 0
    //Week4 Assigment2
    void zmain(void) 
{
    motor_start();
    motor_forward(0,0);
    int muuttuja=0;
    int toinen_muuttuja=0;
    
    
    IR_Start();
    
    uint32_t IR_val; 

    IR_flush();
  
    struct sensors_ dig;
    struct sensors_ ref;

    reflectance_start();
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    
    
    while (SW1_Read() == 1) {
        if (SW1_Read() == 0) {
            break;
        }
        vTaskDelay(100); 
    }
    
    for (;;) {
        motor_forward(40, 150);
        
        reflectance_read(&ref);
        reflectance_digital(&dig);
        
        if (dig.r1 == 1 && dig.r2==1 && dig.r3==1 && dig.l1==1 && dig.l2==1 && dig.l3==1) {
            motor_forward(0,0);
            
            for (;;) {
                if(IR_get(&IR_val, portMAX_DELAY)) {
                    IR_flush();
                    muuttuja+=1;
                    motor_forward(40, 200);
                    break;
                }
                vTaskDelay(50); 
            }
            }
        if (muuttuja > 0) {
            for (;;) {
                
                reflectance_read(&ref);
                reflectance_digital(&dig);
                
                if (dig.r1 == 1 && dig.r2==1 && dig.l1==1 && dig.l2==1)
                    {
                    //risteys
                    toinen_muuttuja +=1;
                    if (toinen_muuttuja==1) {
                        motor_forward(40, 200);                    }
                    else if (toinen_muuttuja > 1 && toinen_muuttuja < 3) {
                        motor_forward(0,0);
                        motor_turn(0,150,500);
                    }
                    else if (toinen_muuttuja >= 3 && toinen_muuttuja <= 4) {
                        motor_forward(0,0);
                        motor_turn(150,0,600);
                    }
                    else if (toinen_muuttuja > 4) {
                        motor_forward(0,0);
                        vTaskDelay(50000);
                    }
                    }
                if (dig.r1 == 1   && dig.l1==1)
                    {
                    //mene suoraan
                    motor_forward(40,1);
                    }

                else if (dig.l1==1 && dig.l2==1)
                    {
                    //käänny oikealle
                    
                    motor_forward(0,0);
                    motor_turn(60,0,1);
                    }
        
                else if (dig.r1 == 1 && dig.r2==1)
                    {
                    //käänny vasemmalle
                    motor_forward(0,0);
                    motor_turn(60,0,1);
                    }

                else if (dig.l2==1 && dig.l3==1)
                    {
                    //käänny vahvasti oikealle
                    motor_forward(0,0);
                    motor_turn(0,80,1);
                    }

                else if (dig.r3==1)
                    {
                    //käänny vahvasti vasemmalle
                    motor_forward(0,0);
                    motor_turn(80,0,1);
                    }
            }
        }
    }   
}

#endif

#if 0
    //Week4Assigment3
    void zmain (void){
    
    motor_start();
    motor_forward(0,0);
    int muuttuja=0;
    int toinen_muuttuja=0;
    
    
    IR_Start();
    
    uint32_t IR_val; 

    IR_flush();
  
    struct sensors_ dig;
    struct sensors_ ref;

    reflectance_start();
    reflectance_set_threshold(9000, 9000, 9000, 9000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    
    
    while (SW1_Read() == 1) {
        if (SW1_Read() == 0) {
            break;
        }
        vTaskDelay(100); 
    }
    
    for (;;) {
        motor_forward(40, 150);
        
        reflectance_read(&ref);
        reflectance_digital(&dig);
        
        if (dig.r1 == 1 && dig.r2==1 && dig.r3==1 && dig.l1==1 && dig.l2==1 && dig.l3==1) {
            motor_forward(0,0);
            for (;;) {
                if(IR_get(&IR_val, portMAX_DELAY)) {
                    IR_flush();
                    muuttuja+=1;
                    motor_forward(40, 200);
                    break;
                }
                vTaskDelay(50); 
            }
            }
        if (muuttuja > 0) {
            for (;;) {
                
                reflectance_read(&ref);
                reflectance_digital(&dig);
                
                if (dig.r1 == 1 && dig.r2==1 && dig.l1==1 && dig.l2==1)
                    {
                    //risteys
                    toinen_muuttuja +=1;
                    if (toinen_muuttuja > 1) {
                        motor_forward(0,0);
                        vTaskDelay(50000);
                    }
                    }
                if (dig.r1 == 1   && dig.l1==1)
                    {
                    //mene suoraan
                    motor_forward(40,1);
                    }
                else if (dig.l1==1)
                    {
                    //käänny oikealle
                    
                    motor_forward(0,0);
                    motor_turn(60,0,1);
                    }
        
                else if (dig.r1 == 1)
                    {
                    //käänny vasemmalle
                    motor_forward(0,0);
                    motor_turn(60,0,1);
                    }

                else if (dig.l2==1 && dig.l3==1)
                    {
                    //käänny vahvasti oikealle
                    motor_forward(0,0);
                    motor_turn(0,80,1);
                    }

                else if (dig.r3==1)
                    {
                    //käänny vahvasti vasemmalle
                    motor_forward(0,0);
                    motor_turn(80,0,1);
                    }
            }
        }
    }   
}

#endif

#if 0
//Week 5 Assigent 1
void zmain(void)
{
    vTaskDelay(15000);
    RTC_Start(); // start real time clock
    int entered_hour=0;
    int entered_min=0;
    
    RTC_TIME_DATE now;
    
    
    print_mqtt("Zumo042/output", "asdf");
    
    // set current time
    printf("Enter the current hour:");
    scanf("%d", &entered_hour);
    
    printf("Enter the current minute:");
    scanf("%d", &entered_min);
    
    now.Hour = entered_hour;
    now.Min = entered_min;
    now.Sec = 00;
    now.DayOfMonth = 25;
    now.Month = 9;
    now.Year = 2018;
    RTC_WriteTime(&now); // write the time to real time clock

    for(;;)
    {
        if(SW1_Read() == 0) {
            // read the current time
            RTC_DisableInt(); /* Disable Interrupt of RTC Component */
            now = *RTC_ReadTime(); /* copy the current time to a local variable */
            RTC_EnableInt(); /* Enable Interrupt of RTC Component */

            // print the current time
            print_mqtt("Zumo042/output", "%2d:%02d.%02d\n", now.Hour, now.Min, now.Sec);
            printf("%2d:%02d.%02d\n", now.Hour, now.Min, now.Sec);
            // wait until button is released
            while(SW1_Read() == 0) vTaskDelay(50);
        }
        vTaskDelay(50);
    }
}   

#endif
#if 0
//Week 5 Assignment 2
void zmain(void)
{
   Ultra_Start();

    while (SW1_Read() == 1) {
        if (SW1_Read() == 0) {
            break;
        }
        vTaskDelay(100); }

    for (;;)
    {
        int random = rand() % 2;
        
        int Ultra_GetDistance();
        printf("%d \n",Ultra_GetDistance());
        motor_start();
        motor_forward(0,0);


        motor_forward(100,0);

        if( Ultra_GetDistance()<15)
        { 
            motor_forward(0,0);
            motor_backward(100,750);
            if (random == 0) {
                print_mqtt("Zumo042/output", "Turned right.");
                motor_turn(0,200,250);
            }
            else if (random ==1) {
                print_mqtt("Zumo042/output", "Turned left.");
                motor_turn(200, 0 ,250);
            }
        }
} 
    
}
#endif

#if 0
    //Week5Assigment3
    void zmain (void){
    
    motor_start();
    motor_forward(0,0);
    int muuttuja=0;
    TickType_t start;
    TickType_t end;
    int time=0;
    
    
    IR_Start();
    
    

    IR_flush();
  
    struct sensors_ dig;
    struct sensors_ ref;

    reflectance_start();
    reflectance_set_threshold(9000, 9000, 9000, 9000, 9000, 9000); // set center sensor threshold to 11000 and others to 9000
    
    while (SW1_Read() == 1) {
        if (SW1_Read() == 0) {
            break;
        }
        vTaskDelay(100); 
    }
    
    start = xTaskGetTickCount();
    
    for (;;) {
        motor_forward(40, 150);
        
        reflectance_read(&ref);
        reflectance_digital(&dig);
        
        if (dig.r1 == 1 && dig.r2==1  && dig.l1==1 && dig.l2==1) {
            motor_forward(0,0);
            end = xTaskGetTickCount();
            time = end - start;
            if (muuttuja > 0) {
                print_mqtt("Zumo042/output", "%d ms", time);
                
            }
            time = time - time;
            muuttuja += 1;
            
          
            IR_flush();
            IR_wait();
            muuttuja+=1;
            start = xTaskGetTickCount();
            motor_forward(40, 200);
                
            }
    }
}
    
#endif

#if 0

void zmain(void)
//Week3 Assigment3
{
    struct accData_ data;
    motor_start();
    motor_forward(0,0);
    
    if(!LSM303D_Start()){
        printf("LSM303D failed to initialize!!! Program is Ending!!!\n");
        vTaskSuspend(NULL);
    }
    else {
        printf("Device Ok...\n");
    }
    while (SW1_Read() == 1) {
        if (SW1_Read() == 0) {
            break;
        }
        vTaskDelay(100); 
    }
    
    for (;;) {
        int r = rand()%3;
        r += 1;
        r *= 250;
        int d = rand()%2;
             
       motor_forward(255, 50);
       
       LSM303D_Read_Acc(&data);
       printf("%8d %8d %8d\n",data.accX, data.accY, data.accZ);
        
       if (abs(data.accX) > 8000) {
        motor_backward(100,1000);
        if (d == 0) {
            motor_turn(0,255, 50);
            }
        else if (d == 1) {
            motor_turn(255,0, 50); 
            
            } 
        }
        motor_forward(255, 50);
       
       LSM303D_Read_Acc(&data);
       printf("%8d %8d %8d\n",data.accX, data.accY, data.accZ);
        
       if (abs(data.accX) > 8000) {
        motor_backward(100,1000);
        if (d == 0) {
            motor_turn(0,255, 50);
            }
        else if (d == 1) {
            motor_turn(255,0, 50); 
            
            } 
        }
        motor_forward(255, 50);
       
       LSM303D_Read_Acc(&data);
       printf("%8d %8d %8d\n",data.accX, data.accY, data.accZ);
        
       if (abs(data.accX) > 8000) {
        motor_backward(100,1000);
        if (d == 0) {
            motor_turn(0,255, 50);
            }
        else if (d == 1) {
            motor_turn(255,0, 50); 
            
            } 
        }
        if (d == 0) {
        motor_turn(255, 0, r);
        }
        else if (d == 1) {
        motor_turn(0, 255, r);
        }
    }
}

#endif

#if 0
/* Example of how to use te Accelerometer!!!*/
void zmain(void)
{
    struct accData_ data;
    
    printf("Accelerometer test...\n");

    if(!LSM303D_Start()){
        printf("LSM303D failed to initialize!!! Program is Ending!!!\n");
        vTaskSuspend(NULL);
    }
    else {
        printf("Device Ok...\n");
    }
    
    for(;;)
    {
        LSM303D_Read_Acc(&data);
        printf("%8d %8d %8d\n",data.accX, data.accY, data.accZ);
        vTaskDelay(50);
    }
 }   
#endif    

#if 0
// MQTT test
void zmain(void)
{
    int ctr = 0;

    printf("\nBoot\n");
    send_mqtt("Zumo01/debug", "Boot");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 

    for(;;)
    {
        printf("Ctr: %d, Button: %d\n", ctr, SW1_Read());
        print_mqtt("Zumo01/debug", "Ctr: %d, Button: %d", ctr, SW1_Read());

        vTaskDelay(1000);
        ctr++;
    }
 }   
#endif


#if 0
void zmain(void)
{    
    struct accData_ data;
    struct sensors_ ref;
    struct sensors_ dig;
    
    printf("MQTT and sensor test...\n");

    if(!LSM303D_Start()){
        printf("LSM303D failed to initialize!!! Program is Ending!!!\n");
        vTaskSuspend(NULL);
    }
    else {
        printf("Accelerometer Ok...\n");
    }
    
    int ctr = 0;
    reflectance_start();
    for(;;)
    {
        LSM303D_Read_Acc(&data);
        // send data when we detect a hit and at 10 second intervals
        if(data.accX > 1500 || ++ctr > 1000) {
            printf("Acc: %8d %8d %8d\n",data.accX, data.accY, data.accZ);
            print_mqtt("Zumo01/acc", "%d,%d,%d", data.accX, data.accY, data.accZ);
            reflectance_read(&ref);
            printf("Ref: %8d %8d %8d %8d %8d %8d\n", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);       
            print_mqtt("Zumo01/ref", "%d,%d,%d,%d,%d,%d", ref.l3, ref.l2, ref.l1, ref.r1, ref.r2, ref.r3);
            reflectance_digital(&dig);
            printf("Dig: %8d %8d %8d %8d %8d %8d\n", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
            print_mqtt("Zumo01/dig", "%d,%d,%d,%d,%d,%d", dig.l3, dig.l2, dig.l1, dig.r1, dig.r2, dig.r3);
            ctr = 0;
        }
        vTaskDelay(10);
    }
 }   

#endif

#if 0
void zmain(void)
{    
    RTC_Start(); // start real time clock
    
    RTC_TIME_DATE now;

    // set current time
    now.Hour = 12;
    now.Min = 34;
    now.Sec = 56;
    now.DayOfMonth = 25;
    now.Month = 9;
    now.Year = 2018;
    RTC_WriteTime(&now); // write the time to real time clock

    for(;;)
    {
        if(SW1_Read() == 0) {
            // read the current time
            RTC_DisableInt(); /* Disable Interrupt of RTC Component */
            now = *RTC_ReadTime(); /* copy the current time to a local variable */
            RTC_EnableInt(); /* Enable Interrupt of RTC Component */

            // print the current time
            printf("%2d:%02d.%02d\n", now.Hour, now.Min, now.Sec);
            
            // wait until button is released
            while(SW1_Read() == 0) vTaskDelay(50);
        }
        vTaskDelay(50);
    }
 }   
#endif

/* [] END OF FILE */
