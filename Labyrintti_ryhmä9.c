#if 1
//Projektin esterata

void right_turn(void);
void left_turn(void);
void risteykseen_ajo(void);
void risteyksesta_lahto(void);

int global_Y_KOORDINAATTI; //Koordinaatti Y, maksimi 11 (0-11)
int global_X_KOORDINAATTI; //Koordinaatti X, maksimi 6 (0-6)

void zmain(void) {
    motor_start();
    motor_forward(0,0);
    int suunta = 1; //Suunta-muuttuja maalin nähden, 0 = vasen, 1 = eteenpäin, 2 = oikea
    
    
    
    TickType_t start;
    TickType_t end;
    int time=0;
    
    IR_Start();
    Ultra_Start();

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
        
        reflectance_digital(&dig);
        
        if (dig.r1 == 1 && dig.r2==1 && dig.r3==1 && dig.l1==1 && dig.l2==1 && dig.l3==1) {
            motor_forward(0,0);
            print_mqtt("Zumo042/ready", "line");
            IR_flush();
            IR_wait();
            motor_forward(160, 300);
            start = xTaskGetTickCount();
            print_mqtt("Zumo042/start", "%d", start);
                    
            break;
        }
        
    }
   
    for (;;) {
        if (global_Y_KOORDINAATTI >= 12) { //Päästy Y-koordinaatistolla loppuun ja navigoidaan maaliin
            if (suunta==0) { //Ollaan vasemmalla ja ohjataan (erittäin karkealla tavalla) maalin
                right_turn();
                risteykseen_ajo();
                risteykseen_ajo();
                risteykseen_ajo();
                left_turn();
                risteykseen_ajo();
                risteykseen_ajo();
                risteykseen_ajo();
                motor_forward(0,0);
                
                end = xTaskGetTickCount();
                time = end - start;
                print_mqtt("Zumo042/stop", "%d", end);
                print_mqtt("Zumo042/time", "%d", time);
                break;
            }
            else if (suunta == 2) { //Ollaan oikealla ja ohjataan (erittäin karkealla tavalla) maalin
                left_turn();
                risteykseen_ajo();
                risteykseen_ajo();
                risteykseen_ajo();
                right_turn();
                risteykseen_ajo();
                risteykseen_ajo();
                risteykseen_ajo();
                motor_forward(0,0);
                
                end = xTaskGetTickCount();
                time = end - start;
                print_mqtt("Zumo042/stop", ",%d", end);
                print_mqtt("Zumo042/time", ",%d", time);
                break;
            }
        }
        
        if (Ultra_GetDistance() < 13) { //Onko suoraan edessä este
            if (suunta != 2) { //Käännytään oikealle eli suunta ei ole oikea (Joko eteen tai vasemmalle)
                right_turn();               
                suunta = 2; //oikea
                reflectance_digital(&dig);
                while (dig.r1 == 1 || dig.r2==1 || dig.r3==1 || dig.l1==1 || dig.l2==1 || dig.l3==1) { //Ajetaan eteenpäin kunnes ei havaitaan yhtään mustaa (ulos radalta)
                    risteykseen_ajo();
                    global_X_KOORDINAATTI += 1;
                    reflectance_digital(&dig);
                    if (Ultra_GetDistance() < 15) {
                        right_turn();
                        risteykseen_ajo();
                        risteykseen_ajo();
                    }
                }
                left_turn();
                suunta=2; 
            }
            else if (suunta != 0) { //Käännytään vasemmalle eli suunta ei ole vasen (Joko eteen tai oikealle)
                left_turn();               
                reflectance_digital(&dig);
                while (dig.r1 == 1 || dig.r2==1 || dig.r3==1 || dig.l1==1 || dig.l2==1 || dig.l3==1) {
                    risteykseen_ajo();
                    global_X_KOORDINAATTI -= 1;
                    reflectance_digital(&dig);
                }
                right_turn();
                suunta=0; //Suunta vasen
            }
        }    
        risteykseen_ajo();
        global_Y_KOORDINAATTI += 1;
        
  }
}

void right_turn(void) {
    motor_forward(0,0);
    
    struct sensors_ dig;
    struct sensors_ ref;
    
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
    reflectance_digital(&dig);
    while (dig.r3 ==0 && dig.l3 == 0) { //Peruutetan kunnes havaitaan risteys
        motor_backward(160, 0);
        reflectance_digital(&dig);
    }
    motor_backward(1,0);
    motor_forward(0,0);
    motor_turn(200, 0, 150);
    reflectance_digital(&dig);
    while (dig.r1 == 0 || dig.l1 == 0) { //Käännytään kunnes havaitaan viiva ja robotti on kohtisuorassa
        motor_turn(200, 0, 0);
        reflectance_digital(&dig);
    }
    motor_turn(0,0,0);
    return ;
}

void left_turn(void) {
    motor_forward(0,0);
    
    struct sensors_ dig;
    struct sensors_ ref;
    
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
    reflectance_digital(&dig);
    while (dig.r3 == 0 && dig.l3 == 0) { //Peruutetan kunnes havaitaan risteys
        motor_backward(160, 0);
        reflectance_digital(&dig);
    }
    motor_backward(1,0);
    motor_forward(0,0);
    motor_turn(0, 200, 150);
    reflectance_digital(&dig);
    while (dig.r1 == 0 || dig.l1 == 0) { //Käännytään kunnes havaitaan viiva ja robotti on kohtisuorassa
        motor_turn(0, 200, 0);
        reflectance_digital(&dig);
    }
    motor_turn(0,0,0);
    return ;
}

void risteykseen_ajo(void) {
    struct sensors_ dig;
    struct sensors_ ref;
    
    reflectance_set_threshold(9000, 9000, 11000, 11000, 9000, 9000);
    
    risteyksesta_lahto();
    
    for (;;) {

        reflectance_digital(&dig);
        
        if (dig.r1 == 0 && dig.r2==0 && dig.r3==0 && dig.l1==0 && dig.l2==0 && dig.l3==0) { //4-risteys havaittu
            print_mqtt("Zumo042/Coordinates", "X%d Y%d", global_X_KOORDINAATTI, global_Y_KOORDINAATTI);
            break;
        }
        if (Ultra_GetDistance() < 13) {
            break;
        }     
        if (dig.r1 == 1 && dig.r2==1 && dig.r3==1 ) //Risteys havaittu (Vasemman puoleinen)
        {
            print_mqtt("Zumo042/Coordinates", "X%d Y%d", global_X_KOORDINAATTI, global_Y_KOORDINAATTI);
            motor_forward(70, 50);
            motor_forward(0,0);
            break;
         }
        else if (dig.l1==1 && dig.l2==1 && dig.l3==1 ) //Risteys havaittu (Oikean puoleinen)
        {
            print_mqtt("Zumo042/Coordinates", "X%d Y%d", global_X_KOORDINAATTI, global_Y_KOORDINAATTI);
            motor_forward(70, 50);
            motor_forward(0,0);
            
            
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
            motor_turn(70,50,20);
            motor_turn(0,0,0);
         }
         else if (dig.l1 == 1) //Tasapainotus
         {
            //käänny vasemmalle
            motor_forward(0,0);
            motor_turn(50,70,20);
            motor_turn(0,0,0);
         }
        else if (dig.l3==1 || dig.l2==1)
        {
            //käänny vahvasti oikealle
            motor_forward(0,0);
            motor_turn(15,70,100);
        }

        else if (dig.r3==1 || dig.r2==1)
        {
            //käänny vahvasti vasemmalle
            motor_forward(0,0);
            motor_turn(70,15,100);
        }
       
    }
    return ;
}

void risteyksesta_lahto(void) { //Mennään eteenpäin risteyksestä jotta sensorit eivät havaitse nykyistä risteystä
    motor_forward(70, 200);
    return ;
}

#endif
