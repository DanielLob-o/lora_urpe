#include "Arduino.h"
#include "WiFi.h"
#include "images.h"
#include "LoRaWan_APP.h"
#include <Wire.h>  
#include "HT_SSD1306Wire.h"
#include "CSV_Parser.h"
/********************************* lora  *********************************************/
#define RF_FREQUENCY                                868000000 // Hz

#define TX_OUTPUT_POWER                             10        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 50 // Define the payload size here

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

static RadioEvents_t RadioEvents;
void OnTxDone( void );
void OnTxTimeout( void );
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

char *csv_str = 
  "order_id,input1,input2,input3,input4,description\n"
  "1,10,20,30,40,First order\n"
  "2,11,21,31,41,Second order\n"
  "3,12,22,32,42,Third order\n"
  "4,13,23,33,43,Fourth order\n"
  "5,14,24,34,44,Fifth order\n"
  "6,15,25,35,45,Sixth order\n"
  "7,16,26,36,46,Seventh order\n"
  "8,17,27,37,47,Eighth order\n"
  "9,18,28,38,48,Ninth order\n"
  "10,19,29,39,49,Tenth order\n"
  "11,20,30,40,50,Eleventh order\n"
  "12,21,31,41,51,Twelfth order\n"
  "13,22,32,42,52,Thirteenth order\n"
  "14,23,33,43,53,Fourteenth order\n"
  "15,24,34,44,54,Fifteenth order\n"
  "16,25,35,45,55,Sixteenth order\n"
  "17,26,36,46,56,Seventeenth order\n"
  "18,27,37,47,57,Eighteenth order\n"
  "19,28,38,48,58,Nineteenth order\n"
  "20,29,39,49,59,Twentieth order\n"
  "21,30,40,50,60,Twenty-first order\n";
CSV_Parser csv_f(csv_str, "sLLLLs");

int currentRow = 0;  // Manual row tracker
int rows = csv_f.getRowsCount();
bool moreData = true;  // Flag to indicate if there's more data to send

/*Define the columns (or the csv structure coming from the CSV saved file*/

char **order_ids = (char**)csv_f["order_id"];
int32_t *input1s = (int32_t*)csv_f["input1"];
int32_t *input2s = (int32_t*)csv_f["input2"];
int32_t *input3s = (int32_t*)csv_f["input3"];
int32_t *input4s = (int32_t*)csv_f["input4"];
char **descriptions = (char**)csv_f["description"];

typedef enum
{
    LOWPOWER,
    STATE_RX,
    STATE_TX
}States_t;

int16_t txNumber;
int16_t rxNumber;
States_t state;
bool sleepMode = false;
int16_t Rssi,rxSize;

String rssi = "RSSI --";
String packSize = "--";
String packet;
String send_num;
String show_lora = "lora data show";

unsigned int counter = 0;
bool receiveflag = false; // software flag for LoRa receiver, received data makes it true.
long lastSendTime = 0;        // last send time
int interval = 1000;          // interval between sends
uint64_t chipid;
int16_t RssiDetection = 0;


void OnTxDone( void )
{
	Serial.print("TX done......");
	state=STATE_RX;

}

void OnTxTimeout( void )
{
  Radio.Sleep( );
  Serial.print("TX Timeout......");
	state=STATE_TX;
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
	rxNumber++;
  Rssi=rssi;
  rxSize=size;
  memcpy(rxpacket, payload, size );
  rxpacket[size]='\0';
  Radio.Sleep( );
  Serial.printf("\r\nreceived packet \"%s\" with Rssi %d , length %d\r\n",rxpacket,Rssi,rxSize);
  Serial.println("wait to send next packet");
	receiveflag = true;
  state=STATE_TX;
}


void lora_init(void)
{
  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);
  txNumber=0;
  Rssi=0;
  rxNumber = 0;
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxDone = OnRxDone;

  Radio.Init( &RadioEvents );
  Radio.SetChannel( RF_FREQUENCY );
  Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                 LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                 LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                 true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

  Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                 LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                 LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                 0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
	state=STATE_TX;
}


/********************************* lora  *********************************************/

SSD1306Wire  factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst


void logo(){
	factory_display.clear();
	factory_display.drawXbm(0,5,logo_width,logo_height,(const unsigned char *)logo_bits);
	factory_display.display();
}

bool resendflag=false;
bool deepsleepflag=false;
bool interrupt_flag = false;
void interrupt_GPIO0()
{
	interrupt_flag = true;
}
void interrupt_handle(void)
{
	if(interrupt_flag)
	{
		interrupt_flag = false;
		if(digitalRead(0)==0)
		{
			if(rxNumber <=2)
			{
				resendflag=true;
			}
			else
			{
				deepsleepflag=true;
			}
		}
	}

}
void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
  
}

void VextOFF(void) //Vext default OFF
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}
void setup()
{
	Serial.begin(115200);
	VextON();
	delay(100);
	factory_display.init();
	factory_display.clear();
	factory_display.display();
	logo();
	delay(300);
	factory_display.clear();
  delay(300);
	attachInterrupt(0,interrupt_GPIO0,FALLING);
	lora_init();
	packet ="waiting lora data!";
  factory_display.drawString(0, 10, packet);
  factory_display.display();
  delay(100);
  factory_display.clear();
	pinMode(LED ,OUTPUT);
	digitalWrite(LED, LOW); 
}


void loop()
{
interrupt_handle();
 if(deepsleepflag)
 {
	VextOFF();
	Radio.Sleep();
	SPI.end();
	pinMode(RADIO_DIO_1,ANALOG);
	pinMode(RADIO_NSS,ANALOG);
	pinMode(RADIO_RESET,ANALOG);
	pinMode(RADIO_BUSY,ANALOG);
	pinMode(LORA_CLK,ANALOG);
	pinMode(LORA_MISO,ANALOG);
	pinMode(LORA_MOSI,ANALOG);
	esp_sleep_enable_timer_wakeup(600*1000*(uint64_t)1000);
	esp_deep_sleep_start();
 }

 if(resendflag)
 {
	state = STATE_TX;
	resendflag = false;
 }

if(receiveflag && (state==LOWPOWER) )
{
	receiveflag = false;
	packet ="R_data:";
	int i = 0;
	while(i < rxSize)
	{
		packet += rxpacket[i];
		i++;
	}
	packSize = "R_Size: ";
	packSize += String(rxSize,DEC);
	packSize += " R_rssi: ";
	packSize += String(Rssi,DEC);
	send_num = "send num: ";
	send_num += String(txNumber,DEC);
	factory_display.drawString(0, 0, show_lora);
  factory_display.drawString(0, 10, packet);
  factory_display.drawString(0, 20, packSize);
  factory_display.drawString(0, 50, send_num);
  factory_display.display();
  delay(10);
  factory_display.clear();

  if((rxNumber%2)==0)
  {
   digitalWrite(LED, HIGH);  
  }
}
switch(state)
  {
    case STATE_TX: {
      if (currentRow < rows) {  // Check if there is another row
        int jsonSize = sprintf(txpacket, 
          "{\"order_id\": \"%s\", \"input1\": %d, \"input2\": %d, \"input3\": %d, \"input4\": %d, \"description\": \"%s\"}",
          order_ids[currentRow], input1s[currentRow], input2s[currentRow], input3s[currentRow], input4s[currentRow], descriptions[currentRow]);

        Serial.printf("\r\nSending CSV packet \"%s\" , length %d\r\n", txpacket, jsonSize);
        Radio.Send((uint8_t *)txpacket, jsonSize);

        currentRow++;  // Move to the next row
      } else {
        // If no more records, perhaps reset the row counter to start over or change state
        currentRow = 0;  // Restart from the first row
      }
      state = LOWPOWER;
      break;
    }
    case STATE_RX:{
      Serial.println("into RX mode");
      Radio.Rx( 0 );
      state=LOWPOWER;
      break;}
    case LOWPOWER:{
      Radio.IrqProcess( );
      break;
    default:
      break;}
  }
}