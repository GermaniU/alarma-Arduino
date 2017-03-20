#include <SoftwareSerial.h>
#include <Keypad.h>
#include <Password.h>

SoftwareSerial SIM900(7, 8);

//--------TECLADO MATRICIAL 4X4----------------------------------------
const byte FILAS=4; //cuatro filas
const byte COLS=4; //cuatro columnas
//definimos el keymap
char keys[FILAS][COLS]={
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[FILAS]={9,A4,A5,6};
byte colPins[COLS]={5,4,3,2};
//crear el keypad
Keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins, FILAS, COLS);

 //crear el password para la alarma
Password password=Password("1111");
//---------------------------------------------------------------------

// ---------PIN SENSORES,BUZZER,LEDS-----------------------------------
int NumeroTonos =12;
int tones[ ] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440,466, 494};

const int Ventana= 12;  
const int PuertaDelantera=11;  
const int PuertaTrasera=10;  
const int Buzzer=A2;

int band=0;
int band1=0;
int band2=0;
int ledRojo =A0;
int ledVerde=A1;

//--------EstadoInicial Sensores-----------.------------------------------
String string="";
int pirState = LOW;           // de inicio no hay movimiento
int LecturaVentana = 0;       // estado Ventana  No hay Movimiento
int LecturaPuerta=0;           // estado Puerta No hay movimiento
int LecturaPuerta2=0;    // estado puerta no hay movimiento



//------------------------------------------------------------------------

void setup(){
   //-------------SENSORES--------------------------------------
 
        pinMode(Buzzer, OUTPUT);  //Salida Buzzer
	pinMode(Ventana, INPUT);  
	pinMode(PuertaDelantera, INPUT);
        pinMode(PuertaTrasera,INPUT);
	Serial.begin(9600);
       pinMode(ledRojo,OUTPUT);
       pinMode(ledVerde,OUTPUT);
       digitalWrite(ledRojo,HIGH);

  //-----------------------SIM 900 PINES-----------------------------------------------
      SIM900.begin(19200);  //Configura velocidad del puerto serie para el SIM900
      Serial.begin(9600);  //Configura velocidad del puerto serie del Arduino
      Serial.println("OK");
      //delay (1000);
      SIM900.println("AT + CPIN = \"XXXX\"");  //Comando AT para introducir el PIN de la tarjeta
      delay(1000);  //Tiempo para que encuentre una RED
      
  
   //---------------------------CODIGO TECLADO MATRICIAL------------------------------------------------
   
    Serial.begin(9600);
    Serial.println("Intro codigo");
    keypad.addEventListener(keypadEvent);

   //------------------------------------------------------------------------------------------
  


}
void loop(){
  //-------------OBTIENE LAS TECLAS QUE SE VAN PRESIONANDO------------------------------------
   keypad.getKey();
  //----------------------SENSORES EMPIEZA LA LECTURA----------------------------------------- 
  if(band==1){
    band2==1;
        
        LecturaVentana = digitalRead(Ventana);  
        Sensores(LecturaVentana,Ventana);
        
        LecturaPuerta = digitalRead(PuertaDelantera);  
        Sensores(LecturaPuerta,PuertaDelantera);        
      
        
     delay(30); //Tiempo en que permaneceran SIN LECTURA
//
  }
//-----Realizas llamada y enviar mensaje---------------
  if(band1==1){
     llamar();  //Realiza la llamada
     mensaje_sms();
     while (1);
     band2=1;
  }

}

//----------------FUNCIONES TECLADO MATRICIAL----------------------------------------------------
//FUNCION PARA COMPROBAR SI SE PULSA EL KEYPAD

void keypadEvent(KeypadEvent eKey){
  switch(keypad.getState()){
    case PRESSED:
    Serial.println("Codigo:");
    Serial.println(eKey);;
    delay(10);
    
    switch(eKey){
      case 'A': checkPasswordA(); delay(1); break;      
      default: password.append(eKey); delay(1);
    }
    
    if(eKey=='C'){
     digitalWrite(ledVerde,LOW);
     digitalWrite(ledRojo,HIGH);
      band=0;
     }
  }
}

 
 //FUNCION DE COMPROBACION DE LA CONTRASEÑA
void checkPasswordA(){
  if(password.evaluate()){
    //si el password es correcto, activa la alarma
     Serial.println("Correcto");
    
    //----------------------MENSAJE DE ALARMA ACTIVA O DESACTIVADA-------
    Serial.print("ALARMA Activada");
    band=1;
    digitalWrite(ledVerde,HIGH);
    digitalWrite(ledRojo,LOW);
    Serial.println();
  }else{
    Serial.println("INCORRECTO");
    password.reset();
  }
 }
 

//------------------FUNCIONES SENSORES PIR-----------------------------------------------------------------
void Sensores(int LecturaSensor,int NumeroSensor){

  //-----Metodo para Mostrar que sensor se activo
  if(NumeroSensor==12){
    string="Ventana";
    }
     if(NumeroSensor==11){
       string="Puerta Delantera";
    }
    if (NumeroSensor==10){
        string="PuertaTrasera";
    }
   
   
if (LecturaSensor == HIGH)   //si algun sensor detecta movimiento pasa de 0 a 1 (HIGH,LOW)
	{ 
            delay(300); //Mergen de detectar moviento
		//digitalWrite(LEDPin, HIGH);  //LED ON
		if (pirState == LOW)  //si previamente estaba apagado
		{
		  Serial.println("Sensor activado");
                  Serial.println(string);
                  Sonido();
		  pirState = HIGH;
       band1=1;
		}
	} 
	else   //si esta desactivado
	{
              delay(300); //Para que espere 300 mls antes de detectar otro moviento
		//digitalWrite(LEDPin, LOW); // LED OFF
		if (pirState == HIGH)  //si previamente estaba encendido entonces lo desactivamos
		{
		  Serial.println("Sensor Detenido");
		  pirState = LOW;
		}
	}
}

void Sonido(){
          tone(Buzzer,250);     // tono de pulsacion
          delay(2000);          // 2s
          noTone(Buzzer);       // Deja de emitir sonido

}

//------------------------------FUNCIONES SIM 900-----------------------------------------------------------
  void llamar()
   {
      Serial.println("Realizando llamada...");
      SIM900.println("ATD9993704941;");  //Comando AT para realizar una llamada
      delay(20000);  // Espera 30 segundos mientras realiza la llamada
      SIM900.println("ATH");  // Cuelga la llamada
      delay(1000);
      Serial.println("Llamada finalizada");
   }
   void mensaje_sms()
   {
      Serial.println("Enviando SMS...");
      SIM900.print("AT+CMGF=1\r");  //Configura el modo texto para enviar o recibir mensajes
      delay(1000);
      SIM900.println("AT + CMGS = \"9993704941\"");  //Numero al que vamos a enviar el mensaje
      delay(1000);
      SIM900.println("Intruso en Casa, intentan accesar a la propiedad por "+string +"Llame a emergencias");  // Texto del SMS
      delay(100);
      SIM900.println((char)26); //Comando de finalización ^Z
      delay(100);
      SIM900.println();
      delay(5000);  // Esperamos un tiempo para que envíe el SMS
      Serial.println("SMS enviado");
      
 band1=0;
 band=1;
   }
//-------------------------------------------------FIN PROGRAMA----------------------------------
