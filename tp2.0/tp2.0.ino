#include <Adafruit_MotorShield.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *motor1 = AFMS.getMotor(1);

Adafruit_DCMotor *motor2 = AFMS.getMotor(2);
//Adafruit_DCMotor *motor3 = AFMS.getMotor(3);
//Adafruit_DCMotor *motor4 = AFMS.getMotor(4);
int numb = 0;
int menu = 0;
int calibragem = 0; //0 = n foi feito calibragem 1 = calibrou o branco 2 = calibrou o preto
int rgbNovo[] = {0,0,0};
int rgbBranco[] = {0,0,0};
int rgbPreto[] = {0,0,0};

#define MAXN 5
#define LEDG 25
#define LEDR 23
#define LEDB 27
#define SENSOR 12


void setup() {
AFMS.begin(); // create with the default frequency 1.6KHz
 lcd.begin(16, 2);  
  lcd.setCursor(0,0);  
  lcd.print("BH Dynamics: ");
  pinMode(LEDR,OUTPUT); //LED vermelho
  pinMode(LEDG,OUTPUT); //LED verde
  pinMode(LEDB,OUTPUT); //LED azul
  pinMode(SENSOR,INPUT);  //Sensor
  digitalWrite(LEDR, HIGH);  
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
  digitalWrite(LEDR, LOW);
    delay(1000);
  digitalWrite(LEDR, HIGH);   

    

}
void print_rgb(){
    lcd.clear();
      char text1[17];
    lcd.setCursor(0,0);
    sprintf(text1, "R: %d", rgbNovo[0]);
    lcd.print(text1);
    char text2[17];
    lcd.setCursor(0,1);
    sprintf(text2, "G: %d B: %d ",rgbNovo[1],rgbNovo[2]);
    lcd.print(text2);
}

void calibrar(){
  int botao = 1023;
    lcd.setCursor(0,1); 
    lcd.print("medir branco");
    while(botao > 831){
      delay(250);
      botao = analogRead(0);
    }
    //podemos fazer com q a função ident_cor() faça essa parte tbm
    ident_cor();
    calibragem = 1;
    delay(5000);
    //
    botao = 1023;        
    lcd.setCursor(0,1); 
    lcd.print("medir preto");
    while(botao > 831){
      delay(250);
      botao = analogRead(0);
    }
    ident_cor();
    calibragem = 2;
    delay(5000);
}

void ident_cor(){
  int num_vezes = 10;
  int leds[3];
  leds[0] = LEDR;
  leds[1] = LEDG;
  leds[2] = LEDB;
  rgbNovo[0] = 0;  
  rgbNovo[1] = 0;  
  rgbNovo[2] = 0;  
  
  for(int i = 0; i < 3; i++){
    digitalWrite(leds[i], LOW);
      
    for(int j = 0; j < num_vezes;j++ )
    {
       delay(250);
       rgbNovo[i] += analogRead(SENSOR);
    }
     rgbNovo[i] =  rgbNovo[i]/num_vezes;
    digitalWrite(leds[i], HIGH);
    if(calibragem == 2){
      float difBp = rgbBranco[i] - rgbPreto[i];
      rgbNovo[i] = (rgbNovo[i] - rgbPreto[i])/(difBp)*255;
    }
    else if(calibragem == 1){
      rgbPreto[i] = rgbNovo[i];
    }
    else{
      rgbBranco[i] = rgbNovo[i];
    }
  }
  print_rgb();

 
}
void girar(int n){
    motor2->setSpeed(150);
    motor2->run(FORWARD);
    int tempo = 725;
    tempo = tempo * n;  
    delay(tempo);
    motor2->run(RELEASE);
}
void agir(int t){
 // fazer um loop para pausa.
 lcd.setCursor(0,0);
 lcd.print("Executando: ");
 switch(t){
  case 1:
    //andar para frente
    // Nao sei como os motores estão organizados
    // Precisa ser alterado
    motor1->setSpeed(255);
    motor1->run(FORWARD);

    //troque o delay pelo tempo necessário para fazer 30cm
    delay(13000);
    motor1->run(BACKWARD);
    delay(12500);
    motor1->run(RELEASE);
        

    break;

  case 2:
      //fazer o triangulo    
    motor1->setSpeed(255);
    motor1->run(FORWARD);

    //substituir valores
    delay(13000);
    //virar para a direita 90 graus
    numb = 2;
    girar(numb);
    
    //parando de virar
    motor1->run(FORWARD);
    delay(13000);
    motor1->run(RELEASE);
    numb = 3;
    //virar direita 135 graus
    girar(numb);

    //parar de virar
    motor1->run(FORWARD);
    // delay do tempo para fazer 30cm vezes raiz de 2
    delay(18385);
    motor1->run(RELEASE);
    girar(numb);
    break;

  case 3:
    //quadrado
    numb = 2;
        motor1->setSpeed(255);
    for(int i = 0; i < 4; i++){
      //fazer o quadrado  
      motor1->run(FORWARD);
      delay(13000);
      motor1->run(RELEASE);
      girar(numb);

    } 
    break;
  case 4:
    //identificação de cor
    if(calibragem == 0){
      calibrar();
    }
    lcd.clear();
    lcd.setCursor(0,1); 
    lcd.print("medir novo");
    int botao_in;
    botao_in = 1023;
    while(botao_in > 831){
      delay(250);
      botao_in = analogRead(0);

    }
    ident_cor();
    //
    print_rgb();
    



    /**8int tmps; 
    //tmps = ident_cor();
    lcd.clear();
    lcd.setCursor(0,1);  
    switch(tmps){
      case 1:
        lcd.print("Vermelho");
        break;
      case 2:
        lcd.print("Verde");
        break;
      case 3:
        lcd.print("Azul");
        break;
      default:
        lcd.print("N identificado");      
    }**/
    break;

  case 5:
    int tempo = millis();
    motor1->setSpeed(150);
      motor1->run(FORWARD);
      motor2->setSpeed(150);
      motor2->run(FORWARD);
    while((millis()- tempo)< 10000){
      
      //int tmp = ident_cor();
      int tmp;
      if(tmp == 1){
        //vermelho
        //pare e dê um giro de 360 graus
      }
      else if(tmp == 2){
        //verde
        //vire à esquerda 90 graus e ande para frente;
      }
      else if(tmp == 3){
        //Azul
        //vire à direita 90 graus e ande para frente;
      }  
      else if(tmp == 4){
        //Amarelo - tem q implementar
        //gire 180 graus e ande para frente;
      }        
                  
    }
      motor1->run(RELEASE);
      motor2->run(RELEASE);   
 }
}


void display_menu(int t){
  lcd.clear();
  lcd.setCursor(0,0);
  char text[17];
  sprintf(text, "Escolha: %d", menu);
  lcd.print(text);  
  lcd.setCursor(0,1);  
  switch(t){
    case 1:
      lcd.print("> linha reta");
      break;
    case 2:
      lcd.print("> fazer triangulo");
      break;
    case 3:
      lcd.print("> fazer quadrado"); 
      break;
    case 4:
      lcd.print("> identificar cor");
      break;
    case 5:
      lcd.print("> multitarefa");
      break;
    default:
      lcd.print("> nao reconhecido");             
      
  }
}

void mudar_menu(int n){
  if(n == 1){
    if(menu == MAXN){
      menu = 1; 
    }
    else{
      menu++;
    }
  }
  else{
    //else é sempre 0
      if(menu == 1){
        menu = MAXN;
      }
      else{
        menu--;
      }
  }
  display_menu(menu);   
}
void start_menu(){
  //ficar atento q sempre vai dar zero, repensar - sempre na direita
  int botao = 1023;
  botao = analogRead(0);
   delay(250);
    if ((botao <49)) {  
    //lcd.print ("Direita  ");
        motor1->setSpeed(255);
    motor1->run(FORWARD);
    delay(2000);    
    motor1->run(BACKWARD);
    delay(2000);

    
  }  
  else if (botao < 175) {  
    //lcd.print ("Cima    ");
    mudar_menu(0);
  }  
  else if (botao < 331){  
    //lcd.print ("Baixo    ");
    mudar_menu(1);
  }  
  else if (botao < 523){  

    //lcd.print ("Esquerda");
    //Parar
          motor2->setSpeed(150);
    motor2->run(FORWARD);


  }  
  else if (botao < 831){  
    //select
    agir(menu);
   } 
  
}

void loop() {
  start_menu();
  //digitalWrite(LED1,LOW);
  //delay();
//  digitalWrite(LED1,HIGH);
   //delay(1000);

    
  

}

