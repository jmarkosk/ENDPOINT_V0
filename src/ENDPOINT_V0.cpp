/*
 * Project ENDPOINT_V0
 * Description:
 * Author:
 * Date:
 */
#include "Particle.h"
#include <limits.h>
#include <math.h>


#include "JsonParserGeneratorRK.h"
#include "HX711ADC.h"
#include "Mydata.h"

void setup();
void loop();
#line 16 "c:/Users/jasonma.Vecima/Documents/particle/xenon/ENDPOINT_V0/src/ENDPOINT_V0.ino"
SYSTEM_THREAD(ENABLED);
//SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_MODE(MANUAL);

#define Serial_DEBUG

#define TEMP_UPDATE_INTERVAL 1000
#define UPDATE_TERMINAL_INTERVAL 2000
//Function Prototypes
void getTempANALOG(void);



const int gLm35_pin = A3;
u64_t gTemp_read_interval = 0;
//float gTemp_val = 0.0;
bool gUPDATE_temp = 0;
float gTemp_val[3];
float gWeight_val[3];

//Global variables --- prefixed with lower case 'g'
String gDeviceID;
String gDeviceID4;
//char gDeviceIDa[];
bool gUPDATE_Terminal = 0;
bool gSend_MESH_MESSAGE = 0;
u64_t gUpdate_Termainal_interval = 0;
bool gMESHready = 1;
bool gTEMP_ready = 1;
bool gTERMINAL_ready = 1;

//JSON-MESH Message varible
/*
   Each endpoint will have:
    - 4 Temperature sensors
    - 4 Weight sensors
      - Name type data
      i.e. Blue: Temp 29.26 :: Weight 100 (in lbs)
           White Temp 29.26 :: Weight 75 (in lbs)
           Green Temp 29.26 :: Weight 50 (in lbs)
           Yellow Temp 29.26 :: Weight 60 (in lbs)

JSON format will be:
"{\"BLUE\": \"TEMP\":\"%0.2f\",\"Weight\":\"%0.2f\"}",



*/

//Data structure
/*
struct sHiveData
{
 //data
 String HiveName; //corresponds to color of cable to the hive
 float HiveTemp;
 float HiveWeight;

}HiveData[] = {
  {"BLUE", 0.0, 0.0},
  {"WHITE", 0.0, 0.0},
  {"GREEN", 0.0, 0.0},
  {"BLACK", 0.0, 0.0},
};
*/


// setup() runs once, when the device is first turned on.
void setup() {
  // Put initialization like pinMode and begin functions here.
   Serial.begin(9600);
   int len=0;
   // Wait for a USB serial connection for up to 30 seconds
   waitFor(Serial.isConnected, 30000);

#ifdef Serial_DEBUG
   Serial.println("DEBUG - ENDNODE: ***Connecting to MESH network**");
#endif   
   
   Mesh.connect();
   
   //System.deviceID().toCharArray(id, sizeof(id));
   gDeviceID = System.deviceID();
   len = strlen(gDeviceID);
   gDeviceID4 = gDeviceID.substring(len - 4, len);
   

#ifdef Serial_DEBUG
  //This was to get last 4 chars of a device ID for a unique identifyer
  Serial.println(sizeof(gDeviceID));
  Serial.println(strlen(gDeviceID));
  Serial.println(gDeviceID.substring(len - 4, len));
  
#endif

   
   
}

// loop() runs over and over again, as quickly as it can execute.

void loop() {
  // The core of your code will likely live here.

  if(millis() - gTemp_read_interval >= TEMP_UPDATE_INTERVAL)
  {
    gTemp_read_interval = millis();
    gUPDATE_temp = 1;  
  }

  if(millis() - gUpdate_Termainal_interval >= UPDATE_TERMINAL_INTERVAL)
  {
    gUpdate_Termainal_interval = millis();
    gUPDATE_Terminal = 1;
    gSend_MESH_MESSAGE = 1;  //test JSON mesage to send  
  }


  if(gUPDATE_temp && gTEMP_ready)
  {
    gTEMP_ready = 0;
    getTempANALOG();
    gTEMP_ready = 1;
  }

  if(gUPDATE_Terminal && gTERMINAL_ready) 
  {
    gUPDATE_Terminal = 0;
    gTERMINAL_ready = 0;

    Serial.print("ENDPOINT: Device iD: ");
    Serial.println(gDeviceID);  //print with linefeed




#ifdef Serial_DEBUG1   
  for(int i = 0; i<4;i++){
    Serial.print("ENDPOINT: Temperature = ");
    Serial.print(gTemp_val[i]);
    Serial.println("C");
    delay(100);

   
  #ifdef Serial_DEBUG1      
    Serial.print("ENDPOINT: Temperature From Structure = ");
    Serial.print(HiveData[i].HiveTemp);
    Serial.println("C");
    delay(100);
  #endif    


  }    
#endif     

    gTERMINAL_ready = 1;

  }

  if(gSend_MESH_MESSAGE && gMESHready)
  {
    //build JSON message
     
    float value=99;
    char sID[20];
    gSend_MESH_MESSAGE = 0;
    gMESHready = 0;
   //JsonWriterStatic<256> jw;  
    JsonWriterStatic<256> MESHmessage;  
    JsonParserStatic<256,20> parser; 

/* ****************
   BT: Blue Temp
   BW: Blue Weight

   GT: Green Temp
   GW: Green Weight

   WT: White Temp
   WW: White Weight

   YT: Yellow Temp
   YW: Yellow Weight
*********************** */   

    
   {
		  JsonWriterAutoObject obj(&MESHmessage);

       MESHmessage.setFloatPlaces(2);

             //gDeviceID4.toCharArray(sID,20);
            //Construct the MESH message
             MESHmessage.insertKeyValue("NODE:", gDeviceID4);

             MESHmessage.insertKeyValue("BT", HiveData[0].HiveTemp);
             MESHmessage.insertKeyValue("BW", HiveData[0].HiveWeight);

             MESHmessage.insertKeyValue("GT", HiveData[1].HiveTemp);
             MESHmessage.insertKeyValue("GW", HiveData[1].HiveWeight);

             MESHmessage.insertKeyValue("WT", HiveData[2].HiveTemp);
             MESHmessage.insertKeyValue("WW", HiveData[2].HiveWeight);

             MESHmessage.insertKeyValue("YT", HiveData[3].HiveTemp);
             MESHmessage.insertKeyValue("YW", HiveData[3].HiveWeight);

	 }
/*
	// Verify the results
	  if (strcmp(jw.getBuffer(), "{\"a\":true,\"b\":1234,\"c\":\"test\"}"))
    {
		  Serial.printlnf("test mismatch got %s", jw.getBuffer());
	   	return;
	  }
    Serial.printlnf("test passed! Data: %s", jw.getBuffer());
 //Printed Data: {"a":true,"b":1234,"c":"test"}        
*/	  

//if (strcmp(jw.getBuffer(), "{\"a\":true,\"b\":1234,\"c\":\"test\"}")) {
#ifdef Serial_DEBUG  
		Serial.printlnf("DEBUG: - ENDNODE: MESH JSON MESSAGE:\r\n %s", MESHmessage.getBuffer());
    
    parser.clear();
	  parser.addString(MESHmessage.getBuffer());
    //parser.
   	if (!parser.parse()) {
		Serial.println("Parsing FAILED!!!");
		return;
    
	}
    Serial.println("Parsing PASSED!!!");
    value = parser.getReference().key("BT").valueFloat();
    Serial.printlnf("");
    Serial.print("Retrieved from JSON:  ");
    Serial.print(value);
    Serial.printlnf("");
    //Serial.printlnf("Buffer size: %d", sizeof(MESHmessage.getBuffer()));
    //Serial.printlnf(MESHmessage.getBuffer());
    ///delay(2000);
#endif

/*   
    //Send MESH data.
    if(Mesh.ready()){
      Serial.println("ENDPOINT: SENDING MESH DATA");
      Mesh.publish("ENDPOINT",MESHmessage.getBuffer());
    }
    else{
      Serial.println("ENDPOINT: - ERROR - MESH NETWORK NOT READY!!!");
      Serial.printlnf("");
    }  
*/
  }   
   gMESHready = 1; 
}





void getTempANALOG(void){

    int adc_value;   
//Temp Sensor Setup
/*
Xenon Analog inputs A0 = 19, A1 = 18...etc;
#define A0          D19  TEMP BLUE
#define A1          D18  TEMP WHITE
#define A2          D17  TEMP GREEN
#define A3          D16  TEMP YELLOW
#define A4          D15  Not used
#define A5          D14  Not used
*/
   // adc_value = analogRead(gLm35_pin);	/* Read Temperature */
   // gTemp_val = ((adc_value * 3.3)/4095);	/* Convert adc value to equivalent voltage */    
   // gTemp_val = ((gTemp_val - 0.424)/0.00625);	/* LM35 gives output of 6.25mv/°C */  

    for(int i = 0, j=0; (i+19) >= 16; i--,j++)
    {
      adc_value = analogRead((i+19));	/* Read Temperature */
      gTemp_val[j] = ((adc_value * 3.3)/4095);	/* Convert adc value to equivalent voltage */    
      gTemp_val[j] = ((gTemp_val[j] - 0.424)/0.00625);	/* LM35 gives output of 6.25mv/°C */ 
      HiveData[j].HiveTemp = gTemp_val[j];
    }


}
