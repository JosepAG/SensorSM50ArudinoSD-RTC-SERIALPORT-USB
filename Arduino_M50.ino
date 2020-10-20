#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
#include <EEPROM.h>
#include <stdlib.h>     /* strtol */
 

File myFile;
//const uint8_t CS = 10; // SD chip select
RTC_DS1307 rtc;

#define max471I  A8
#define max471V  A9
float conI;
float conV;
float pw;
int segundo,minuto,hora,dia,mes;
String dia0, mes0, minuto0,hora0;
String numero0="0";
unsigned long total;
const int LED=7;
 float Datos_final_float;
long anio; //variable año
int i;
  String Datos[15];
int totalint, total2int, total3int;
File file;

int recibido;
DateTime HoraFecha;
int Hay_Nuevo=EEPROM.read(0);
char filename[] = "0000000000.txt";
char Datos_final[]="00000000";

void Leer_bateria();
void Leer_datos();
void desconversion();
void archivo_nuevo();
void crear_nuevo();



void setup()
{
  // Wire.begin();
  pinMode(max471I, INPUT);
  pinMode(max471V, INPUT);
  pinMode(LED,OUTPUT);

  //////////
  Serial.begin(9600);
  Serial1.begin(9600);
  //Serial.begin(9600);
  //Serial.print("arranca puerto serie ...");
  //delay(3000);
  rtc.begin();
  //rtc.adjust(DateTime(2014, 10, 1, 14, 0, 0));
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  //Serial.print("Iniciando SD ...");
  
  delay(1000);
  if (!SD.begin(4)) {
    //
    int p=0;
    while(p<50){
    delay(100);
     digitalWrite(LED,HIGH);
     delay(100);
      digitalWrite(LED,LOW);
     p++;
    }
    digitalWrite(LED,LOW);
    return;
  }
 
  digitalWrite(LED,HIGH);
  
 //
  //Serial.println("inicializacion exitosa");

    
}





void loop()
{
 if (Serial.available() > 0){
   delay(100);
   
   String str = Serial.readString();  //gets one byte from serial buffer
  // readString += c; //makes the string readString
  String diaa= str.substring(0,2);
  String mess= str.substring(3,5);
   String anyoo= str.substring(6,10);
   String horra=str.substring(11,13);
   String minutoss=str.substring(14,16);
  

  rtc.adjust(DateTime(anyoo.toInt(), mess.toInt(), diaa.toInt(), horra.toInt(), minutoss.toInt(), 0));    // minutoss.toInt(), 0));
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  }
Leer_bateria();
if (Serial1.available() > 0){
      Leer_datos();  ///Lee puerto Ozono, también lo repite al Serial 2 para el programa en C
      tiempo();  //crea el tiempo actual
      posicion_dia(); //comprueba el dia de los ultimos datos      
      Hay_Nuevo=EEPROM.read (0);
      //Si hay archivo nuevo///
    // Serial.println("Hay_Nuevo");
      if(Hay_Nuevo==1){
          //  if((totalint==dia)||(totalint==(dia+1))||(totalint==(dia-1))||(((totalint==31)||(totalint==30)||(totalint==28))&&(dia==1))&&(total2int==mes+1||total2int==mes)&&(total3int==anio)){
              if((totalint==dia)||(totalint==(dia+1))||(totalint==(dia-1))){
          
              //  Serial.println("Hay Nuevo, igual");
                 archivo_total();  //Guarda en archivo donde estan todos los datos
                 desconversion(); //Lee el nombre del archivo de la EEPROM
                 archivo_nuevo(); //Guarda en ultimo archivo
                }


           //  if((totalint!=dia)&&(totalint!=(dia+1))&&(totalint!=(dia-1))&&((totalint==31)||(totalint==30)||(totalint==28))&&(dia!=1)&&((total2int!=mes+1)||(total2int!=mes))&&(total3int!=anio)){
                if((totalint!=dia)&&(totalint!=(dia+1))&&(totalint!=(dia-1))){
                  Serial.println("dia es: ");
                Serial.println( dia);
                Serial.println("totalint es: ");
                Serial.println( totalint);
                Serial.println("Hay Nuevo, no igual");
                 archivo_total(); //Guarda en archivo donde estan todos los datos
                 crear_nuevo();  //Crear un archivo nuevo para guardar
                 archivo_nuevo(); ////Guarda en ultimo archivo, el q se ha creado
                }
      }


        ////Si no hay archivo nuevo////
      if(Hay_Nuevo!=1){  
         Serial.println("No hay Nuevo");  
          archivo_total(); //Guarda en archivo donde estan todos los datos
          crear_nuevo();  //Crear un archivo nuevo para guardar
          archivo_nuevo(); ////Guarda en ultimo archivo, el q se ha creado
          Hay_Nuevo=1;
          EEPROM.write (0, 1);
          Hay_Nuevo=EEPROM.read (0);
 
 }
}
 
}
  
  
  
  


void tiempo(){

  /////////SE DEFINEN VARIABLES//////////
    
    HoraFecha = rtc.now(); //obtenemos la hora y fecha actual   
    segundo=HoraFecha.second();
    minuto=HoraFecha.minute();
    hora=HoraFecha.hour();
    dia=HoraFecha.day();



    
    if(dia<10){
      dia0=numero0+dia;
    }else{
      dia0=dia;
    }
    mes=HoraFecha.month();
     if(mes<10){
      mes0=numero0+mes;
    }else{
      mes0=mes;
    }
    if(hora<10){
      hora0=numero0+hora;
    }else{
      hora0=hora;
    }
    if(minuto<10){
      minuto0=numero0+minuto;
    }else{
      minuto0=minuto;
    }
    
    anio=HoraFecha.year();
    // Serial.println(dia0);
    //  Serial.println(mes0);


}

void posicion_dia(){
   ////////LLIG LULTIM DIA DEL ARCHIVO////RECUERDO LA ULTIMA LINEA DEL ARCHIVO DEBE ESTAR EN BLANCO(ENTER) SI NO ESCRIBIRÁ EN LA MISMA LINEA O CONTARÁ MAL LOS ULTIMOS BYTES
      myFile = SD.open("archivo.txt", FILE_READ);
      if (myFile) {
           total= myFile.size();
           if (total>0||total<0){     
         
           ///Ultim dia////////    /////Cambiar substring////////////////
               myFile.seek(total-47);    
               myFile.position();     
              char numero1=myFile.read();
              int numero1int=int(numero1)-48;
      
             ////Numero 2 de la busqueda
               myFile.seek(total-46);    
               myFile.position();
               char numero2=myFile.read();
               int numero2int=int(numero2)-48;
             totalint= numero1int*10+numero2int;

             
  
          ///ultim mes/////////   
               myFile.seek(total-23);    
               myFile.position();     
              char numero3=myFile.read();
              int numero3int=int(numero3)-48;
      
             ////Numero 2 de la busqueda
               myFile.seek(total-22);    
               myFile.position();
               char numero4=myFile.read();
               int numero4int=int(numero4)-48; 
     
             total2int= numero3int*10+numero4int;

             

              ///ultim any/////////   
               myFile.seek(total-18);    
               myFile.position();     
              char numero5=myFile.read();
              int numero5int=int(numero5)-48;
      
             ////Numero 2 de la busqueda
               myFile.seek(total-17);    
               myFile.position();
               char numero6=myFile.read();
               int numero6int=int(numero6)-48; 
     
             total3int= 2000+numero5int*10+numero6int;
             
     
            }
     myFile.close();
    }else {
            //Serial.println("Error al abrir el archivo");
                 if (!SD.begin(4)) {
                  int p=0;
                while(p<50){
                delay(100);
                digitalWrite(LED,HIGH);
                delay(100);
                digitalWrite(LED,LOW);
                 p++;
              }
              digitalWrite(LED,LOW);
              //      Serial.println("No se pudo inicializar");
          return;
          }   
              
          digitalWrite(LED,HIGH);
         //Serial.println("inicializacion exitosa");
        }
}

void archivo_total(){
 myFile = SD.open("archivo.txt", FILE_WRITE);//abrimos  el archivo  /////////SIEMPRE SE GUARDA EN ARCHIVO
  if (myFile) {                
        if(isnan(Datos_final_float)){
          }else{          
        myFile.print(dia0);
        myFile.print('/');
        myFile.print(mes0);
        myFile.print('/');
        myFile.print(anio);
        myFile.print(" ");
        myFile.print(hora0);
        myFile.print(":");
        myFile.print(minuto0);
        myFile.print(",");
        myFile.print(Datos_final_float,7);
        myFile.print(",");
        myFile.print(conI,5);
        myFile.print(",");
        myFile.print(conV);
        myFile.print(",");
        myFile.println(pw);
        delay(100);
       }  
       
    myFile.close(); //cerramos el archivo
  } else {
    //Serial.println("Error al abrir el archivo");
            if (!SD.begin(4)) {
        //  Serial.println("No se pudo inicializar");
            int p=0;
            while(p<50){
            delay(100);
            digitalWrite(LED,HIGH);
            delay(100);
            digitalWrite(LED,LOW);
            p++;
            }
            digitalWrite(LED,LOW);
          //return; 
          }
            else
            {
        digitalWrite(LED,HIGH);
            }//  Serial.println("inicializacion exitosa");
       
  //}
   delay(500);
  }
 }
void crear_nuevo(){
   //Serial.println("crear nuevo");
      //filename1=String{dia0+mes0+anio+ hora0+ minuto0+".txt"}; ///////////////////CREAMOS ARCHIVO Y GUARDAMOS EN ÉL
           
      sprintf(filename, "%02d%02d%d.txt", dia, mes, anio);                 
      //Serial.println(filename);
      for(int i=0; i<13;i++){
        char x=filename[i];      
        int y= int(x);     
        EEPROM.write(i+1,y);//=y;
      }
     
       tiempo();
       myFile = SD.open(filename, FILE_WRITE);
       if (myFile){
              myFile.println("Timestamp, Ozono(ppb), Current(A), Voltage(V), Current(W)");
              delay(100);
               myFile.close();
       }else {        
            if (!SD.begin(4)) {
            int p=0;
            while(p<50){
            delay(100);
            digitalWrite(LED,HIGH);
            delay(100);
            digitalWrite(LED,LOW);
            p++;
          }
          digitalWrite(LED,LOW);
          //return;
          }
          else{
          //}
       digitalWrite(LED,HIGH);
          }
       // Serial.println("inicializacion exitosa");
  }
   delay(500);
}


 
void archivo_nuevo(){


 /////////////SI HAY MÁS DE UN DIA DE DIFERENCIA CREAMOS ARCHIVO NUEVO
 
   
       // Serial.println("archivo nuevo");
       myFile = SD.open(filename, FILE_WRITE);
         if (myFile) {   
          if(isnan(Datos_final_float)){
          }else{       
        myFile.print(dia0);
        myFile.print('/');
         myFile.print(mes0);
        myFile.print('/');
         myFile.print(anio);
         myFile.print(" ");
         myFile.print(hora0);
        myFile.print(":");
         myFile.print(minuto0);
         myFile.print(",");
         myFile.print(Datos_final_float,7);    
         myFile.print(",");
        myFile.print(conI,5);
        myFile.print(",");
        myFile.print(conV);
        myFile.print(",");
        myFile.println(pw);   
        delay(100); 
          }     
     myFile.close(); //cerramos el archivo
      } else {        
            if (!SD.begin(4)) {
            int p=0;
            while(p<50){
            delay(100);
            digitalWrite(LED,HIGH);
            delay(100);
            digitalWrite(LED,LOW);
            p++;
          }
          digitalWrite(LED,LOW);
          //return;
          }
          else{
          //}
       digitalWrite(LED,HIGH);
          }
       // Serial.println("inicializacion exitosa");
  }
   delay(500);

          
         // Serial.println("");
        
       
}

void desconversion(){

 for(int i=0; i<13;i++){
        char achar=EEPROM.read(i+1);             
      filename[i]= achar;
      }

//Serial.println(filename);

}

void Leer_datos(){

 
    for (i=0;i<15;i++){
      delay(100);
      recibido=Serial1.read();
      //byte br;
      //br = (int)recibido;
      delay(100);
      Serial.write(recibido); ////serial 2 repite lo recibido Serial1
      Datos[i]=String(recibido, HEX);
    }

    if(Datos[2].length()==1){
      Datos[2] = "0"+Datos[2];      
    }
    if(Datos[3].length()==1){
      Datos[3] = "0"+Datos[3];
    }
    if(Datos[4].length()==1){
      Datos[4] = "0"+Datos[4];
    }
    if(Datos[5].length()==1){
      Datos[5] = "0"+Datos[5]; ;
    }
    
    String Datos_final="0x"+Datos[5]+Datos[4]+Datos[3]+Datos[2]; 
    const char* Datos_final_char=Datos_final.c_str();
    unsigned long Datos_final_long =strtol(Datos_final_char, NULL, 16);
    Datos_final_float = *((float*)&Datos_final_long);
  //  Serial.println(Datos_final_float,7);
  
}
void Leer_bateria(){
  int h=0;
  float conIM=0;
  float conVM=0;
  float pwM=0;
  while(h<50){
  delay(100);
  int RawI = analogRead(max471I);
  int RawV = analogRead(max471V);
  //conI=RawI;
  //conV=RawV;
  conI=(RawI*5.0)/1023;
  conV=((RawV*5.0)/1023)*5.0;
  pw=conI*conV;
  conIM=conIM+conI;
  conVM=conVM+conV;
  pwM=pwM+pw;
  h++;
  if(h==49){
 conI=conIM/50;
 conV=conVM/50;
 pw=pwM/50;
  }
  }

}
  
