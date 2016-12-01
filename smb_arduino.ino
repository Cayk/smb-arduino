#include "SIM900.h"
#include <SoftwareSerial.h>
#include "inetGSM.h"
#include "gps.h"
//To change pins for Software Serial, use the two lines in GSM.cpp.

InetGSM inet;
GPSGSM gps;

char msg[50];
char inSerial[50];
char stat;
int i=0;

char rota[15] = "localizacao";
char identificador[10] = "bike0007";
char lon[15];
char lat[15];
char alt[15];
char time[20];
char vel[15];
char msg1[5];
char msg2[5];
char enviar[50];

boolean started=false;
boolean gprsStarted = false;
boolean gpsStarted = false;
boolean gpsnc = true;

int k;
int j;
int temp;
int temp2;
int temp3;

void setup()
{
     //Serial connection.
     Serial.begin(9600);
     Serial.println(F("GSM Shield testing."));
     //Start configuration of shield with baudrate.
     //For http uses is raccomanded to use 4800 or slower.
     if (gsm.begin(2400)) {
          Serial.println(F("\nstatus=READY"));
          gsm.forceON();	//To ensure that SIM908 is not only in charge mode
          started=true;
     }else 
          Serial.println(F("\nstatus=IDLE"));
   
    //gps_enviar();   
}  

void loop(){
    
    gps_enviar();
    //serialhwread();
    //serialswread();
    
    //if( !gpsnc ){
      delay(10000);
      enviarMensagem();
    //}
    delay(10000);
}

void enviarMensagem(){
     if(started) {
          //GPRS attach, put in order APN, username and password.
          //If no needed auth let them blank.
          if(!gprsStarted){
            if (inet.attachGPRS("timbrasil.br", "tim", "tim")){
                 Serial.println(F("status=ATTACHED"));
                 gprsStarted = true;
            }
            else 
                 Serial.println(F("status=ERROR"));
          }
          delay(1000);

          //Read IP address.
          gsm.SimpleWriteln("AT+CIFSR");
          delay(5000);
          
          //TCP Client GET, send a GET request to the server and
          //save the reply.
         
         enviar[0] = '/';
         
         k=0;
         j=1;
         while(rota[k] != '\0'){
           enviar[j] = rota[k];
           k++;
           j++;
         }
         
         enviar[j] = '/';
        
         k=0;
         temp = j+1;
         while(identificador[k] != '\0'){
           enviar[temp] = identificador[k];
           k++;
           temp++;
         }
         
         enviar[temp] = ':';
         
         k=0;
         temp2 = temp+1;
         while(lat[k] != '\0'){
           enviar[temp2] = lat[k];
           k++;
           temp2++;
         }
         
         enviar[temp2] = ':';
         
         k =0;
         temp3 = temp2+1;
         while(lon[k] != '\0'){
           enviar[temp3] = lon[k];
           k++;
           temp3++;
         }
         enviar[temp3] = '\0';
         Serial.println(enviar);

         inet.httpGET("35.161.237.63", 3000, enviar, msg, 50);
          
         Serial.println(F("terminei"));
         //inet.dettachGPRS();
          
     }
}

void gps_enviar(){
  if(started){
    //GPS attach
    if(gpsnc || !gpsStarted){
      if (gps.attachGPS()){
        Serial.println(F("status=GPSREADY"));
        gpsStarted = true;
        gpsnc = false;
      }
      else 
        Serial.println(F("status=ERROR"));
    }
    delay(20000);	//Time for fixing
    
    stat=gps.getStat();
	if(stat==1){
		Serial.println(F("NOT FIXED"));
                gpsnc = true;
        }
	else if(stat==0){
		Serial.println(F("GPS OFF"));
                gpsnc = true;
        }
	else if(stat==2)
		Serial.println(F("2D FIXED"));
	else if(stat==3)
		Serial.println(F("3D FIXED"));
     
	delay(5000);

	//Get data from GPS
        gps.getPar(lon,lat,alt,time,vel);
	Serial.println(lon);
	Serial.println(lat);
	//Serial.println(alt);
	//Serial.println(time);
	//Serial.println(vel);
        //gps.deattachGPS();
        
  }
}

void serialhwread(){
  i=0;
  if (Serial.available() > 0){            
    while (Serial.available() > 0) {
      inSerial[i]=(Serial.read());
      delay(10);
      i++;      
    }
    
    inSerial[i]='\0';
    if(!strcmp(inSerial,"/END")){
      Serial.println(F("_"));
      inSerial[0]=0x1a;
      inSerial[1]='\0';
      gsm.SimpleWriteln(inSerial);
    }
    //Send a saved AT command using serial port.
    if(!strcmp(inSerial,"TEST")){
//      Serial.println("BATTERY TEST 1");
//      gps.getBattInf(msg1,msg2);
//      Serial.println(msg1);
//      Serial.println(msg2);
//      Serial.println("BATTERY TEST 2");
//      gps.getBattTVol(msg1);
//      Serial.println(msg1);
        stat=gps.getStat();
	if(stat==1){
		Serial.println(F("NOT FIXED"));
                gpsnc = true;
	}else if(stat==0){
		Serial.println(F("GPS OFF"));
                gpsnc = true;
        }
	else if(stat==2)
		Serial.println(F("2D FIXED"));
	else if(stat==3)
		Serial.println(F("3D FIXED"));
    }
    //Read last message saved.
    if(!strcmp(inSerial,"MSG")){
      Serial.println(msg1);
    }
    else{
      Serial.println(inSerial);
      gsm.SimpleWriteln(inSerial);
    }    
    inSerial[0]='\0';
  }
}

void serialswread(){
  gsm.SimpleRead();
}

