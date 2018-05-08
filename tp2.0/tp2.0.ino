#include <Adafruit_MotorShield.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);
//Adafruit_DCMotor *motor3 = AFMS.getMotor(3);
//Adafruit_DCMotor *motor4 = AFMS.getMotor(4);


#define MAXN 5
#define LEDG 25
#define LEDR 23
#define LEDB 27
#define SENSOR 12
#define RED 0
#define GREEN 1
#define BLUE 2
#define YELLOW 3
#define BLACK 4
#define NOT_IDENTIFIED 5
#define RIGHT 0
#define UP 1
#define DOWN 2
#define LEFT 3
#define SELECT 4
#define NO_BUTTON 5


int numb = 0;
int menu = 0;
int calibragem = 0; //0 = n foi feito calibragem 1 = calibrou o branco 2 = calibrou o preto
int rgbNovo[] = {0,0,0};
int rgbBranco[] = {255,255,255};
int rgbPreto[] = {0,0,0};
int buttons_threshold[] = {49, 175, 331, 523, 831};
int leds[] = {LEDR, LEDG, LEDB};

void turn_leds_off() {
  for (int i = RED; i<=BLUE; i++) {
    digitalWrite(leds[i], HIGH); //desliga
  }
}

void turn_on_led(int led) {
  //desliga todos os leds e acende só o que foi pedido
  turn_leds_off();
  digitalWrite(leds[led], LOW); // liga
}

int avg(int *arr, int size) {
  int sum = 0;
  for(int i=0; i<size; i++) {
    sum += arr[i];
  }
  return sum/size;
}


void wait_for_bloco() {
  int medicoes = 10;
  int sensor_values[medicoes];
  int tempo;
  int i;
  int media;
  int estado = 0; // 0 = mesa, 1 = suspeita de bloco. 2 = bloco
  char text[50];
  int led_on = GREEN;

  turn_on_led(led_on);
  
  // leitura inicial antes de começar a andar
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Leitura inicial...");
  for (int i=0; i<medicoes; i++) {
    sensor_values[i] = (float)(analogRead(SENSOR) - rgbPreto[led_on])/ ((float)(rgbBranco[led_on] - rgbPreto[led_on]))*255;
    sprintf(text, "G: %d", sensor_values[i]);
    lcd.setCursor(0, 1);
    lcd.print(text);
    delay(100);
  }

  delay(1000);
  lcd.setCursor(0, 0);
  lcd.print("Buscando bloco...");

  motor1->setSpeed(255);
  motor1->run(FORWARD);
  
  // loop para buscar o bloco
  tempo = millis();
  i = 0;
  do {
    sensor_values[i] = (float)(analogRead(SENSOR) - rgbPreto[led_on])/ ((float)(rgbBranco[led_on] - rgbPreto[led_on]))*255;
    i = (i + 1) % medicoes;
    media = avg(sensor_values, medicoes);
    lcd.setCursor(0,1);
    sprintf(text, "Media: %d       ", media);
    lcd.print(text);
    if(media < 255) {
      if (estado == 0) {
        // leitura no verde parou de estourar, confirmar com leitura no azul
        
        lcd.setCursor(0, 1);
        lcd.print("Confirmando...");
        delay(500);
        motor1->run(RELEASE);
        turn_on_led(BLUE);
        led_on = BLUE;
        for(int j=0; j<medicoes; j++) {
          sensor_values[j] = (float)(analogRead(SENSOR) - rgbPreto[led_on])/ ((float)(rgbBranco[led_on] - rgbPreto[led_on]))*255;
          delay(100);
        }
        motor1->run(FORWARD);
        estado = 1;
      } else {
        motor1->run(RELEASE);
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.print("Bloco encontrado");
        delay(1000);
        break;
      }
    }
  } while (true);

  
}

int get_pressed_button() {
  int value;
  int pressed_button = NO_BUTTON;
  
  value = analogRead(0);
  for (int i = RIGHT; i < NO_BUTTON; i++) {
    if (value < buttons_threshold[i]) {
      pressed_button = i;
      break;  
    }
  }
  return pressed_button;
}

void button_release() {
  int button;
  do {
    button = get_pressed_button();
  } while(button != NO_BUTTON);
}



void wait_for_button(int button) {
  int value;
  int pressed_button;

  do {
    pressed_button = get_pressed_button();
  } while (pressed_button != button);
  
}

void liberar(){
    motor1->setSpeed(255);
    motor1->run(BACKWARD);
    delay(750);
    motor1->run(RELEASE);
    
}

int get_color() {
  int max = -1023;
  int max_cor = 0;
  float percentual;

  for (int i = 0; i<3; i++) {
    if (rgbNovo[i] > max) {
      max = rgbNovo[i];
      max_cor = i;
    }
  }

  if (rgbNovo[GREEN] > 255 && rgbNovo[BLUE] > 255) {
    //Está lendo mesa provavelmente
    return NOT_IDENTIFIED;
  }

  if (rgbNovo[RED] > 100) {
    //vermelho ou amarelo
    percentual = (float) rgbNovo[GREEN] / (float) rgbNovo[RED];
    if (percentual > 0.7) {
      return YELLOW;
    } else {
      return RED;
    }
  } else {
    //preto, azul, verde
    if (max_cor == BLUE) {
      //preto ou azul
      percentual = (float) rgbNovo[RED] / (float) rgbNovo[BLUE];
      if (percentual > 0.2) {
        return BLACK;
      } else {
        return BLUE;
      }
    } else if (max_cor == GREEN) {
      //verde ou preto
      percentual = (float) rgbNovo[RED] / (float) rgbNovo[GREEN];
      if (percentual > 0.6) {
        return BLACK;
      } else {
        return GREEN;
      }
    } else {
      // preto ou erro max_color == RED
      if (rgbNovo[RED] < 50) {
        return BLACK;
      } else {
        return NOT_IDENTIFIED;
      }
    }
  }
  return NOT_IDENTIFIED;
}

void print_color() {
  int cor = get_color();
  lcd.clear();
  lcd.setCursor(0,0);
  switch (cor) {
    case RED:
      lcd.print("-- VERMELHO --");
      break;
    case GREEN:
      lcd.print("-- VERDE --");
      break;
    case BLUE:
      lcd.print("-- AZUL --");
      break;
    case YELLOW:
      lcd.print("-- AMARELO --");
      break;
    case BLACK:
      lcd.print("-- PRETO --");
      break;
    case NOT_IDENTIFIED:
      lcd.print("-- ERRO --");
      break;
  }
}

void setup() {
  AFMS.begin(); // create with the default frequency 1.6KHz
  lcd.begin(16, 2);  
  lcd.setCursor(0,0);
  lcd.clear();
  pinMode(LEDR,OUTPUT); //LED vermelho
  pinMode(LEDG,OUTPUT); //LED verde
  pinMode(LEDB,OUTPUT); //LED azul
  pinMode(SENSOR,INPUT);  //Sensor
  for(int i=RED; i<=BLUE; i++) {
    turn_on_led(i);
    delay(500);
  }
  turn_leds_off();
  lcd.print("BH Dynamics: ");
}

void print_rgb(){
    char text1[50];
    lcd.setCursor(0,1);
    sprintf(text1, "R:%d G:%d B:%d\0",rgbNovo[RED], rgbNovo[GREEN],rgbNovo[BLUE]);
    lcd.print(text1);
}

void calibrar(){
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Medir branco");
  wait_for_button(SELECT);

  //podemos fazer com q a função ident_cor() faça essa parte tbm
  lcd.setCursor(0,1);
  lcd.print("Medindo...");
  ident_cor(20);
  calibragem = 1;

  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Medir preto");
  wait_for_button(SELECT);
  lcd.setCursor(0,1);
  lcd.print("Medindo...");
  ident_cor(20);
  calibragem = 2;
}

void ident_cor(int num_vezes){
  rgbNovo[0] = 0;  
  rgbNovo[1] = 0;  
  rgbNovo[2] = 0;  
  
  for(int i = 0; i < 3; i++) {
    digitalWrite(leds[i], LOW);
      
    for(int j = 0; j < num_vezes;j++ ) {
       delay(100);
       rgbNovo[i] += analogRead(SENSOR);
    }
    rgbNovo[i] =  rgbNovo[i]/num_vezes;
    digitalWrite(leds[i], HIGH);
    if(calibragem == 2){
      float difBp = rgbBranco[i] - rgbPreto[i];
      rgbNovo[i] = (rgbNovo[i] - rgbPreto[i])/ (difBp)*255;
    }
    else if(calibragem == 1){
      rgbPreto[i] = rgbNovo[i];
    }
    else{
      rgbBranco[i] = rgbNovo[i];
    }
  }
}

void girar(int n) {
    // angulo = 45 * n
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
  switch(t) {
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
    lcd.setCursor(0,0); 
    lcd.print("Medir novo");
    int botao_in;
    do {
      botao_in = get_pressed_button();
    } while (botao_in != SELECT);
    lcd.setCursor(0,1);
    lcd.print("Medindo...");
    ident_cor(10);
    for (int i = 0; i < 3; i++) {
      if(get_color() == NOT_IDENTIFIED) {
        ident_cor(10);  
      } else {
        break;
      }
    }
    
    print_color();
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
    int cor;
    while(true){
      wait_for_bloco();
      ident_cor(10);
      print_color();
      print_rgb();
  
     
      liberar();
      cor = get_color();
      
      if(cor == RED){
       
        //vermelho
        //pare e dê um giro de 360 graus
        motor1->run(RELEASE);
        numb = 8;
        girar(numb);
        delay(3000);
        break;
      }
      else if(cor == GREEN){
        //verde
        //vire à esquerda 90 graus e ande para frente;
        motor1->run(RELEASE);
        motor2->setSpeed(150);
        motor2->run(BACKWARD);
        delay(1450);
        motor2->run(RELEASE);
        motor1->run(FORWARD);
        delay(3000);
        break;
      }
      else if(cor == BLUE){
        //Azul
        //vire à direita 90 graus e ande para frente;
                
        motor1->run(RELEASE);
        numb = 2;
        girar(numb);
        motor1->run(FORWARD);
        delay(3000);
        break;
        
  
      }  
      else if(cor == YELLOW){
        //Amarelo 
        //gire 180 graus e ande para frente;
        motor1->run(RELEASE);
        numb = 4;
        girar(numb);
        motor1->run(FORWARD);
        delay(3000);
        break;
        
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
  int botao;
  do {
    botao = get_pressed_button();  
  } while(botao == NO_BUTTON);
  button_release();
  

  switch (botao) {
    case RIGHT:
      motor1->setSpeed(255);
      motor1->run(FORWARD);
      delay(2000);
      motor1->run(BACKWARD);
      delay(2000);
      break;
    case UP:
      mudar_menu(0);
      break;
    case DOWN:
      mudar_menu(1);
      break;
    case LEFT:
      motor2->setSpeed(255);
      motor2->run(FORWARD);
      break;
    case SELECT:
      agir(menu);
      break;
  }
  
}

void loop() {
  start_menu();
}

