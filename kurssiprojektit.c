/*#if 0
// Sumo-battle (alku)
void zmain(void)
{
    motor_start();
    motor_forward(0,0);
    
    
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
            print_mqtt("Zumo042/ready", "line");
            IR_flush();
            IR_wait();
            motor_forward(255, 250);
           
            break;
        }
    }
}
#endif
*/

#if 1
// Sumo-battle
void zmain(void)
{
struct accData_ data;
motor_start();
motor_forward(0,0);
LSM303D_Start();
IR_Start();
Ultra_Start();
int Ultra_GetDistance();
TickType_t start;


TickType_t end;
TickType_t time;


struct sensors_ dig;
struct sensors_ ref;

reflectance_start();
reflectance_set_threshold(11000, 9000, 11000, 11000, 9000, 11000); 


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

//Jos risteys havaitaan, niin robotti pysähtyy, ja sitten se tulostaa"valmis".
//Sen jälkeen se menee tilaan, missä se odottaa signaalia. Signaalin saatuaan, se menee eteenpäin ja rikkoo toistorakenteen.
if (dig.r1 == 1 && dig.r2==1 && dig.r3==1 && dig.l1==1 && dig.l2==1 && dig.l3==1) {
motor_forward(0,0);
print_mqtt("Zumo042/ready", "line<br>");
IR_flush();
IR_wait();
start = xTaskGetTickCount();
print_mqtt("Zumo042/start", "%d <br>", start);
motor_forward(100, 250);
vTaskDelay(400);

break;
}
}
for(;;)
{//sisällä ringissä peruuttaa viivalle tultaessa//

reflectance_read(&ref);
reflectance_digital(&dig);
motor_forward(100, 1);
LSM303D_Read_Acc(&data);



//Jos robottiin osutaan, niin robotti tulostaa "hit". Kun nappia painetaan, niin robotti lopettaa toimintansa ja tulotsaa "stop".
if (abs(data.accX) > 5000) {
print_mqtt("Zumo042/hit", "hit <br>");
}
else if (abs(data.accY) > 5000) {
print_mqtt("Zumo042/hit", "hit <br>");
}

if (SW1_Read()==0) {
end=xTaskGetTickCount();
time=end-start;
print_mqtt("Zumo042/stop", "%d <br>", end);
print_mqtt("Zumo042/time", "%d <br>", time);
}

reflectance_read(&ref);
reflectance_digital(&dig);

//Jos robotti havaitsee viivan, niin robotti menee taaksepäin ja kääntyy kohdan mukaan vasemmalle tai oikelle
if ( dig.l3==1 || dig.l2==1) {
motor_forward(0,0);
print_mqtt("Zumo042/ready", "line <br>");
motor_backward(200, 0);
vTaskDelay(400);
motor_turn(255,0,600);
}
else if (dig.r3==1 || dig.r2==1){
motor_forward(0,0);
print_mqtt("Zumo042/ready", "line <br>");
motor_backward(200, 0);
vTaskDelay(400);
motor_turn(0,255,600);
}
//Mikäli robotin eteen ilmestyy jotain, niin robotti työntää esteen kunnes viiva havaitaan.
if(Ultra_GetDistance()<20){
motor_forward(0,0);
motor_forward(255,1);}

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
    int time=0;
    
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
            print_mqtt("Zumo042/ready", "line");
            IR_flush();
            IR_wait();
            motor_forward(255, 100);
            start = xTaskGetTickCount();
            print_mqtt("Zumo042/start", "%d ms", start);
            
            break;
        }
    }
    
    for (;;) { //Ajo-loop
                reflectance_read(&ref);
                reflectance_digital(&dig);      
                if (dig.r1 == 1 && dig.r2==1 && dig.l1==1 && dig.l2==1 && dig.l3==1 && dig.r3==1)
                    {
                    //Pysähdys risteykseen
                    end = xTaskGetTickCount();
                    time = end - start;
                    print_mqtt("Zumo042/stop", "%d ms", end);
                    print_mqtt("Zumo042/time", "%d ms", time);
                    motor_stop();
                    vTaskDelay(15000);
                    }
                    
                if (dig.r1 == 1   && dig.l1==1)
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
    }
    
}
#endif

#if 0
//Projektin esterata
void zmain(void) {
    motor_start();
    motor_forward(0,0);
    
    TickType_t start;
    //TickType_t end;
    //int time=0;
    
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
            print_mqtt("Zumo042/ready", "line");
            IR_flush();
            IR_wait();
            motor_forward(255, 250);
            start = xTaskGetTickCount();
            print_mqtt("Zumo042/start", "%d", start);
                    
            break;
        }
        
    }
}

#endif
