#include <Adafruit_MotorShield.h>
#include <LiquidCrystal.h>  
  
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);  
  

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *motor1 = AFMS.getMotor(1);

Adafruit_DCMotor *motor2 = AFMS.getMotor(2);
//Adafruit_DCMotor *motor3 = AFMS.getMotor(3);
//Adafruit_DCMotor *motor4 = AFMS.getMotor(4);

int menu = 0;
#define MAXN 5
#define LED1 1
#define LED2 2
#define LED3 3
#define SENSOR 4


void setup() {
AFMS.begin(); // create with the default frequency 1.6KHz
 lcd.begin(16, 2);  
  lcd.setCursor(0,0);  
  lcd.print("BH Dynamics: ");
  //pinMode(LED1,OUTPUT); //LED vermelho
  //pinMode(LED2,OUTPUT); //LED verde
  //pinMode(LED3,OUTPUT); //LED azul
  //pinMode(SENSOR,INPUT);  //Sensor

    

}
//
int ident_cor(){
  int val = 0;
  for(int i = 1; i <= 3; i++){
    //n sei se pode fazer isso no arduino, vamos testar.
    // digitalWrite('LED' + i,HIGH);
     val = digitalRead(SENSOR);
     if(val == HIGH){
     // digitalWrite('LED' + i,LOW);
    return i;
    }
   // digitalWrite('LED' + i,LOW);
  }
  return 0;   
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
    motor2->setSpeed(255);
    motor2->run(FORWARD);
    delay(1275);
    motor2->run(RELEASE);
    
    //parando de virar
    motor1->run(FORWARD);
    delay(13000);
    motor1->run(RELEASE);
    
    //virar direita 135 graus
    motor2->run(FORWARD);
    delay(1450);
    motor2->run(RELEASE);

    //parar de virar
    motor1->run(FORWARD);
    // delay do tempo para fazer 30cm vezes raiz de 2
    delay(18385);
    motor1->run(RELEASE);
    motor2->run(FORWARD);
    delay(1450);
    motor2->run(RELEASE);

    break;

  case 3:
        motor1->setSpeed(255);
        motor2->setSpeed(150);
    for(int i = 0; i < 4; i++){
      //fazer o quadrado  
      motor1->run(FORWARD);
      delay(13000);
      motor1->run(RELEASE);
      motor2->run(FORWARD);
      //substituir valores
      delay(1275);
      motor2->run(RELEASE);

    }
    break;
  case 4:
    //identificação de cor
    //preciso entender melhor como funciona
    int tmps; 
    tmps = ident_cor();
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
    }
    break;

  case 5:
    int tempo = millis();
    motor1->setSpeed(150);
      motor1->run(FORWARD);
      motor2->setSpeed(150);
      motor2->run(FORWARD);
    while((millis()- tempo)< 10000){
      
      int tmp = ident_cor();
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
    if ((botao < 49)) {  
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
          motor2->setSpeed(255);
    motor2->run(FORWARD);


  }  
  else if (botao < 831){  
    //select
    agir(menu);
   } 
  
}

void loop() {
  start_menu();

}
