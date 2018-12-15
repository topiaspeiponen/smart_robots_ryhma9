#if 0   
//Projektin kilparata

void zmain(void)
{
        
    motor_start();
    motor_forward(0,0);
    
    TickType_t start;
    TickType_t end;
    
    int risteyksentarkistaja=0; //Oletusarvo 0 kun koodi alkaa, ja ekan risteyksen havaittaessa sen arvo lisääntyy
                                //ja pysähtyy sitä seuraavaan risteykseen
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
            motor_forward(255, 150);
            start = xTaskGetTickCount();
            print_mqtt("Zumo042/start", "%d", start);
            
            break;
        }
    }
    
    for (;;) { //Ajo-loop
                reflectance_read(&ref);
                reflectance_digital(&dig);
                
                
                if (dig.r1 == 1 && dig.r2==1 && dig.l1==1 && dig.l2==1 && dig.l3==1 && dig.r3==1)
                    {
                        if (risteyksentarkistaja >= 1) {
                            //Pysähdys risteykseen
                            end = xTaskGetTickCount();
                            time = end - start;
                            print_mqtt("Zumo042/stop", "%d", end);
                            print_mqtt("Zumo042/time", "%d", time);
                            motor_stop();
                            vTaskDelay(15000);
                        }
                        else {
                            risteyksentarkistaja += 1;
                            motor_forward(255, 100);
                        }
                    }
                    
                if (dig.r1 == 1   && dig.l1==1)
                    {
                    //mene suoraan
                    motor_forward(255,0);
                    
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
