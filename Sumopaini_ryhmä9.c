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
          print_mqtt("Zumo042/ready", "line");
          IR_flush();
          IR_wait();
          start = xTaskGetTickCount();
          print_mqtt("Zumo042/start", "%d", start);
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
          if (abs(data.accX) > 13500) {
          print_mqtt("Zumo042/hit", "hit %d", xTaskGetTickCount());
          }
          else if (abs(data.accY) > 13500) {
          print_mqtt("Zumo042/hit", "hit %d", xTaskGetTickCount());
          }

          if (SW1_Read()==0) {
          end=xTaskGetTickCount();
          time=end-start;
          print_mqtt("Zumo042/stop", "%d", end);
          print_mqtt("Zumo042/time", "%d", time);
          motor_forward(0,0);
          motor_stop();
          }

          reflectance_read(&ref);
          reflectance_digital(&dig);

          //Jos robotti havaitsee viivan, niin robotti menee taaksepäin ja kääntyy kohdan mukaan vasemmalle tai oikelle
          if ( dig.l3==1 || dig.l2==1) {
          motor_forward(0,0);

          motor_backward(200, 0);
          vTaskDelay(400);
          motor_turn(255,0,600);
          }
          else if (dig.r3==1 || dig.r2==1){
          motor_forward(0,0);

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
