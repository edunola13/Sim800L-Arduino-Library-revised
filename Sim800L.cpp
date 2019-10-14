/*
 *  This library was written by Vittorio Esposito
 *    https://github.com/VittorioEsposito
 *
 *  Designed to work with the GSM Sim800L.
 *
 *  ENG
 *      This library uses NeoSWSerial, you can define RX and TX pins
 *      in the header "Sim800L.h", by default pins are RX=10 and TX=11.
 *      Be sure that GND is connected to arduino too.
 *      You can also change the RESET_PIN as you prefer.
 *
 *  ESP
 *      Esta libreria usa NeoSWSerial, se pueden cambiar los pines de RX y TX
 *      en el archivo header, "Sim800L.h", por defecto los pines vienen configurado en
 *      RX=10 TX=11.
 *      Tambien se puede cambiar el RESET_PIN por otro que prefiera
 *
 *  ITA
 *      Questa libreria utilizza la NeoSWSerial, si possono cambiare i pin di RX e TX
 *      dall' intestazione "Sim800L.h", di default essi sono impostati come RX=10 RX=11
 *      Assicurarsi di aver collegato il dispositivo al pin GND di Arduino.
 *      E' anche possibile cambiare il RESET_PIN.
 *
 *
 *   DEFAULT PINOUT:
 *        _____________________________
 *       |  ARDUINO UNO >>>   Sim800L  |
 *        -----------------------------
 *            GND      >>>   GND
 *        RX  10       >>>   TX
 *        TX  11       >>>   RX
 *       RESET 2       >>>   RST
 *
 *   POWER SOURCE 4.2V >>> VCC
 *
 *
 *  SOFTWARE SERIAL NOTES:
 *
 *      PINOUT
 *      The library has the following known limitations:
 *      1. If using multiple software serial ports, only one can receive data at a time.
 *      2. Not all pins on the Mega and Mega 2560 support change interrupts, so only the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), A15 (69).
 *      3. Not all pins on the Leonardo and Micro support change interrupts, so only the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
 *      4. On Arduino or Genuino 101 the current maximum RX speed is 57600bps
 *      5. On Arduino or Genuino 101 RX doesn't work on Pin 13
 *
 *      BAUD RATE
 *      Supported baud rates are 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 31250, 38400, 57600, and 115200.
 *
 *
 *  Edited on:  December 24, 2016
 *    Editor:   Vittorio Esposito
 *
 *  Original version by:   Cristian Steib
 *
 *
*/

#include "Arduino.h"
#include "Sim800L.h"
#include <NeoSWSerial.h>

//NeoSWSerial SIM(RX_PIN,TX_PIN);
//String _buffer;

Sim800L::Sim800L(void) : NeoSWSerial(DEFAULT_RX_PIN, DEFAULT_TX_PIN)
{
    RX_PIN      = DEFAULT_RX_PIN;
    TX_PIN      = DEFAULT_TX_PIN;
    RESET_PIN   = DEFAULT_RESET_PIN;
    LED_PIN     = DEFAULT_LED_PIN;
    LED_FLAG    = DEFAULT_LED_FLAG;
}

Sim800L::Sim800L(uint8_t rx, uint8_t tx) : NeoSWSerial(rx, tx)
{
    RX_PIN      = rx;
    TX_PIN      = tx;
    RESET_PIN   = DEFAULT_RESET_PIN;
    LED_PIN     = DEFAULT_LED_PIN;
    LED_FLAG    = DEFAULT_LED_FLAG;
}

Sim800L::Sim800L(uint8_t rx, uint8_t tx, uint8_t rst) : NeoSWSerial(rx, tx)
{
    RX_PIN      = rx;
    TX_PIN      = tx;
    RESET_PIN   = rst;
    LED_PIN     = DEFAULT_LED_PIN;
    LED_FLAG    = DEFAULT_LED_FLAG;
}

Sim800L::Sim800L(uint8_t rx, uint8_t tx, uint8_t rst, uint8_t led) : NeoSWSerial(rx, tx)
{
    RX_PIN      = rx;
    TX_PIN      = tx;
    RESET_PIN   = rst;
    LED_PIN     = led;
    LED_FLAG    = true;
}

void Sim800L::begin()
{

    pinMode(RESET_PIN, OUTPUT);

    _baud = DEFAULT_BAUD_RATE;          // Default baud rate 9600
    this->NeoSWSerial::begin(_baud);

    _sleepMode = 0;
    _functionalityMode = 1;

    if (LED_FLAG) pinMode(LED_PIN, OUTPUT);

    // _buffer.reserve(BUFFER_RESERVE_MEMORY); // Reserve memory to prevent intern fragmention
}

void Sim800L::begin(uint32_t baud)
{

    pinMode(RESET_PIN, OUTPUT);

    _baud = baud;
    this->NeoSWSerial::begin(_baud);

    _sleepMode = 0;
    _functionalityMode = 1;

    if (LED_FLAG) pinMode(LED_PIN, OUTPUT);

    // _buffer.reserve(BUFFER_RESERVE_MEMORY); // Reserve memory to prevent intern fragmention
}


/*
 * AT+CSCLK=0   Disable slow clock, module will not enter sleep mode.
 * AT+CSCLK=1   Enable slow clock, it is controlled by DTR. When DTR is high, module can enter sleep mode. When DTR changes to low level, module can quit sleep mode
 */
bool Sim800L::setSleepMode(bool state)
{

    _sleepMode = state;

    if (_sleepMode) this->NeoSWSerial::print(F("AT+CSCLK=1\r\n "));
    else            this->NeoSWSerial::print(F("AT+CSCLK=0\r\n "));

    if ( (_readSerial().indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}

bool Sim800L::getSleepMode()
{
    return _sleepMode;
}

/*
 * AT+CFUN=0    Minimum functionality
 * AT+CFUN=1    Full functionality (defualt)
 * AT+CFUN=4    Flight mode (disable RF function)
*/
bool Sim800L::setFunctionalityMode(uint8_t fun)
{

    if (fun==0 || fun==1 || fun==4)
    {

        _functionalityMode = fun;

        switch(_functionalityMode)
        {
        case 0:
            this->NeoSWSerial::print(F("AT+CFUN=0\r\n "));
            break;
        case 1:
            this->NeoSWSerial::print(F("AT+CFUN=1\r\n "));
            break;
        case 4:
            this->NeoSWSerial::print(F("AT+CFUN=4\r\n "));
            break;
        }

        if ( (_readSerial().indexOf("ER")) == -1)
        {
            return false;
        }
        else return true;
        // Error found, return 1
        // Error NOT found, return 0
    }
    return false;
}

uint8_t Sim800L::getFunctionalityMode()
{
    return _functionalityMode;
}

bool Sim800L::setMessageStorage(String value)
{
    String command;
    command  = "AT+CPMS=\"";
    command += value;
    command += "\"\r\n";

    // Can take up to 5 seconds

    this->NeoSWSerial::print(command);

    if ( (_readSerialUntil(5000).indexOf("ER")) == -1)
    {
        return true;
    }
    return false;
}

bool Sim800L::setMessageFormat(String value)
{
    String command;
    command  = "AT+CMGF=";
    command += value;
    command += "\r\n";

    // Can take up to 5 seconds

    this->NeoSWSerial::print(command);
	String _buffer = _readSerialUntil(5000);
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return true;
    }
    return false;
}

bool Sim800L::setClip(String value)
{
	// Activamos (1) o desactivamos (0) la deteccion de llamadas
    String command;
    command  = "AT+CLIP=";
    command += value;
    command += "\r\n";

    // Can take up to 5 seconds

    this->NeoSWSerial::print(command);
	String _buffer = _readSerialUntil(5000);
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return true;
    }
    return false;
}

bool Sim800L::setPIN(String pin)
{
    String command;
    command  = "AT+CPIN=";
    command += pin;
    command += "\r";

    // Can take up to 5 seconds

    this->NeoSWSerial::print(command);

    if ( (_readSerial(5000).indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}

String Sim800L::getProductInfo()
{
    this->NeoSWSerial::print("ATI\r");
    return (_readSerial());
}


String Sim800L::getOperatorsList()
{

    // Can take up to 45 seconds

    this->NeoSWSerial::print("AT+COPS=?\r");

    return _readSerialUntil(45000);

}

String Sim800L::getOperator()
{

    this->NeoSWSerial::print("AT+COPS ?\r");

    return _readSerialUntil();

}

bool Sim800L::calculateLocation()
{
    /*
        Type: 1  To get longitude and latitude
        Cid = 1  Bearer profile identifier refer to AT+SAPBR
    */

    uint8_t type = 1;
    uint8_t cid = 1;
    
    String tmp = "AT+CIPGSMLOC=" + String(type) + "," + String(cid) + "\r\n";
    this->NeoSWSerial::print(tmp);
    
    /*
    this->NeoSWSerial::print("AT+CIPGSMLOC=");
    this->NeoSWSerial::print(type);
    this->NeoSWSerial::print(",");
    this->NeoSWSerial::print(cid);
    this->NeoSWSerial::print("\r");
    */

    String data = _readSerialUntil(20000);

    if (data.indexOf("ER")!=(-1)) return false;

    uint8_t indexOne;
    uint8_t indexTwo;

    indexOne = data.indexOf(":") + 1;
    indexTwo = data.indexOf(",");

    _locationCode = data.substring(indexOne, indexTwo);

    indexOne = data.indexOf(",") + 1;
    indexTwo = data.indexOf(",", indexOne);

    _longitude = data.substring(indexOne, indexTwo);

    indexOne = data.indexOf(",", indexTwo) + 1;
    indexTwo = data.indexOf(",", indexOne);

    _latitude = data.substring(indexOne, indexTwo);

    return true;

}

String Sim800L::getLocationCode()
{
    return _locationCode;
    /*
     Location Code:
     0      Success
     404    Not Found
     408    Request Time-out
     601    Network Error
     602    No Memory
     603    DNS Error
     604    Stack Busy
     65535  Other Error
    */
}

String Sim800L::getLongitude()
{
    return _longitude;
}

String Sim800L::getLatitude()
{
    return _latitude;
}


//
//PUBLIC METHODS
//

void Sim800L::reset()
{
    if (LED_FLAG) digitalWrite(LED_PIN,1);

    digitalWrite(RESET_PIN,1);
    delay(1000);
    digitalWrite(RESET_PIN,0);
    delay(1000);
    // wait for the module response

    this->NeoSWSerial::print(F("AT\r\n"));
    while (_readSerial().indexOf("OK")==-1 )
    {
        this->NeoSWSerial::print(F("AT\r\n"));
    }

    //wait for sms ready
    while (_readSerial().indexOf("SMS")==-1 );

    if (LED_FLAG) digitalWrite(LED_PIN,0);

}

void Sim800L::setPhoneFunctionality()
{
    /*AT+CFUN=<fun>[,<rst>]
    Parameters
    <fun> 0 Minimum functionality
    1 Full functionality (Default)
    4 Disable phone both transmit and receive RF circuits.
    <rst> 1 Reset the MT before setting it to <fun> power level.
    */
    this->NeoSWSerial::print (F("AT+CFUN=1\r\n"));
}


String Sim800L::signalQuality()
{
    /*Response
    +CSQ: <rssi>,<ber>Parameters
    <rssi>
    0 -115 dBm or less
    1 -111 dBm
    2...30 -110... -54 dBm
    31 -52 dBm or greater
    99 not known or not detectable
    <ber> (in percent):
    0...7 As RXQUAL values in the table in GSM 05.08 [20]
    subclause 7.2.4
    99 Not known or not detectable
    */
    this->NeoSWSerial::print (F("AT+CSQ\r\n"));
    return(_readSerialUntil());
}


void Sim800L::activateBearerProfile()
{
    this->NeoSWSerial::print (F(" AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" \r\n" ));
    String _buffer=_readSerial();  // set bearer parameter
    this->NeoSWSerial::print (F(" AT+SAPBR=3,1,\"APN\",\"internet\" \r\n" ));
    _buffer=_readSerial();  // set apn
    this->NeoSWSerial::print (F(" AT+SAPBR=1,1 \r\n"));
    delay(1200);
    _buffer=_readSerial();          // activate bearer context
    this->NeoSWSerial::print (F(" AT+SAPBR=2,1\r\n "));
    delay(3000);
    _buffer=_readSerial();          // get context ip address
}


void Sim800L::deactivateBearerProfile()
{
    this->NeoSWSerial::print (F("AT+SAPBR=0,1\r\n "));
    delay(1500);
}



bool Sim800L::answerCall()
{
    this->NeoSWSerial::print (F("ATA\r\n"));
    //Response in case of data call, if successfully connected
    String _buffer=_readSerialUntil();
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}


bool Sim800L::callNumber(char* number)
{
    this->NeoSWSerial::print (F("ATD"));
    this->NeoSWSerial::print (number);
    this->NeoSWSerial::print (F(";\r\n"));
	String _buffer=_readSerialUntil();
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}



uint8_t Sim800L::getCallStatus()
{
    /*
      values of return:

     0 Ready (MT allows commands from TA/TE)
     2 Unknown (MT is not guaranteed to respond to tructions)
     3 Ringing (MT is ready for commands from TA/TE, but the ringer is active)
     4 Call in progress

    */
    this->NeoSWSerial::print (F("AT+CPAS\r\n"));
    String _buffer=_readSerialUntil();
    return _buffer.substring(_buffer.indexOf("+CPAS: ")+7,_buffer.indexOf("+CPAS: ")+9).toInt();

}



bool Sim800L::hangoffCall()
{
    this->NeoSWSerial::print (F("ATH\r\n"));
    String _buffer=_readSerialUntil();
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}


bool Sim800L::sendSms(char* number,char* text)
{

    // Can take up to 60 seconds

    this->NeoSWSerial::print (F("AT+CMGF=1\r"));     //set sms to text mode
    String _buffer=_readSerialUntil();
    if ((_buffer.indexOf("ER")) == -1)
    {
        this->NeoSWSerial::print (F("AT+CMGS=\""));      // command to send sms
        this->NeoSWSerial::print (number);
        this->NeoSWSerial::print(F("\"\r"));
        _buffer=_readSerial();
        this->NeoSWSerial::print (text);
        this->NeoSWSerial::print ("\r");
        _buffer=_readSerial();
        this->NeoSWSerial::print((char)26);
        _buffer=_readSerialUntil(60000);
        //expect CMGS:xxx   , where xxx is a number,for the sending sms.
        if (_buffer.indexOf("CMGS:") != -1)
        //if ( (_buffer.indexOf("ER")) == -1)
        {
            // IS OK
            return false;
        }
        else return true;
    }
    return true;
    // Error found, return 1
    // Error NOT found, return 0
}


String Sim800L::getNumberSms(uint8_t index)
{
    String _buffer=readSms(index);
    //Serial.println(_buffer.length());
    if (_buffer.length() > 10) //avoid empty sms
    {
        uint8_t _idx1=_buffer.indexOf("+CMGR:");
        _idx1=_buffer.indexOf("\",\"",_idx1+1);
        return _buffer.substring(_idx1+3,_buffer.indexOf("\",\"",_idx1+4));
    }
    else
    {
        return "";
    }
}



String Sim800L::readSms(uint8_t index)
{

    // Can take up to 5 seconds

    this->NeoSWSerial::print (F("AT+CMGF=1\r"));
	String _buffer = _readSerialUntil();
    if (( _buffer.indexOf("ER")) ==-1)
    {
        this->NeoSWSerial::print (F("AT+CMGR="));
        this->NeoSWSerial::print (index);
        this->NeoSWSerial::print ("\r");
        _buffer=_readSerialUntil();
        if (_buffer.indexOf("CMGR:")!=-1)
        {
            return _buffer;
        }
        else return "";
    }
    else {
        return "ERROR";
    }
}

void Sim800L::readSmsReference(uint8_t index, String& buffer, int max=1000)
{

    // Can take up to 5 seconds

    this->NeoSWSerial::print (F("AT+CMGF=1\r"));
	_readSerialUntilReference(buffer, max);
    if (( buffer.indexOf("ER")) ==-1)
    {
        this->NeoSWSerial::print (F("AT+CMGR="));
        this->NeoSWSerial::print (index);
        this->NeoSWSerial::print ("\r");
		buffer = "";
        _readSerialUntilReference(buffer, max);
        if (buffer.indexOf("CMGR:")!=-1)
        {
            return buffer;
        }
        else {
			buffer = "";
			return "";
		}
    }
    else {
		buffer = "ERROR";
        return "ERROR";
    }
}


bool Sim800L::delSms(uint8_t index, uint8_t option)
{
    // Can take up to 25 seconds

    this->NeoSWSerial::print(F("at+cmgd="));
	this->NeoSWSerial::print (index);
	this->NeoSWSerial::print (",");
	this->NeoSWSerial::print (option);
	this->NeoSWSerial::print ("\r");
    String _buffer=_readSerialUntil(25000);
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}

bool Sim800L::delAllSms()
{
    // Can take up to 25 seconds

    this->NeoSWSerial::print(F("at+cmgda=\"del all\"\n\r"));
    String _buffer=_readSerialUntil(25000);
    if ( (_buffer.indexOf("ER")) == -1)
    {
        return false;
    }
    else return true;
    // Error found, return 1
    // Error NOT found, return 0
}


void Sim800L::RTCtime(int *day,int *month, int *year,int *hour,int *minute, int *second)
{
    this->NeoSWSerial::print(F("at+cclk?\r\n"));
    // if respond with ERROR try one more time.
    String _buffer=_readSerial();
    if ((_buffer.indexOf("ERR"))!=-1)
    {
        delay(50);
        this->NeoSWSerial::print(F("at+cclk?\r\n"));
    }
    if ((_buffer.indexOf("ERR"))==-1)
    {
        _buffer=_buffer.substring(_buffer.indexOf("\"")+1,_buffer.lastIndexOf("\"")-1);
        *year=_buffer.substring(0,2).toInt();
        *month= _buffer.substring(3,5).toInt();
        *day=_buffer.substring(6,8).toInt();
        *hour=_buffer.substring(9,11).toInt();
        *minute=_buffer.substring(12,14).toInt();
        *second=_buffer.substring(15,17).toInt();
    }
}

//Get the time  of the base of GSM
String Sim800L::dateNet()
{
    this->NeoSWSerial::print(F("AT+CIPGSMLOC=2,1\r\n "));
    String _buffer=_readSerial();

    if (_buffer.indexOf("OK")!=-1 )
    {
        return _buffer.substring(_buffer.indexOf(":")+2,(_buffer.indexOf("OK")-4));
    }
    else
        return "0";
}

// Update the RTC of the module with the date of GSM.
bool Sim800L::updateRtc(int utc)
{

    activateBearerProfile();
    String _buffer=dateNet();
    deactivateBearerProfile();

    _buffer=_buffer.substring(_buffer.indexOf(",")+1,_buffer.length());
    String dt=_buffer.substring(0,_buffer.indexOf(","));
    String tm=_buffer.substring(_buffer.indexOf(",")+1,_buffer.length()) ;

    int hour = tm.substring(0,2).toInt();
    int day = dt.substring(8,10).toInt();

    hour=hour+utc;

    String tmp_hour;
    String tmp_day;
    //TODO : fix if the day is 0, this occur when day is 1 then decrement to 1,
    //       will need to check the last month what is the last day .
    if (hour<0)
    {
        hour+=24;
        day-=1;
    }
    if (hour<10)
    {

        tmp_hour="0"+String(hour);
    }
    else
    {
        tmp_hour=String(hour);
    }
    if (day<10)
    {
        tmp_day="0"+String(day);
    }
    else
    {
        tmp_day=String(day);
    }
    //for debugging
    //Serial.println("at+cclk=\""+dt.substring(2,4)+"/"+dt.substring(5,7)+"/"+tmp_day+","+tmp_hour+":"+tm.substring(3,5)+":"+tm.substring(6,8)+"-03\"\r\n");
    this->NeoSWSerial::print("at+cclk=\""+dt.substring(2,4)+"/"+dt.substring(5,7)+"/"+tmp_day+","+tmp_hour+":"+tm.substring(3,5)+":"+tm.substring(6,8)+"-03\"\r\n");
    if ( (_readSerial().indexOf("ER"))!=-1)
    {
        return true;
    }
    else return false;


}



//
//NO MORE PRIVATE METHODS
//
String Sim800L::_readSerialUntil(uint32_t timeout = TIME_OUT_READ_SERIAL)
{

    uint64_t timeOld = millis();
    String str;

    while (!str.endsWith("OK\r\n") && !str.endsWith("ERROR\r\n") && !(millis() > timeOld + timeout))
    {
        while(this->NeoSWSerial::available())
        {
            if (this->NeoSWSerial::available()>0)
            {
                str += (char) this->NeoSWSerial::read();
            }
        }       
    }

    return str;

}

void Sim800L::_readSerialUntilReference(String& buffer, int max=1000, uint32_t timeout = TIME_OUT_READ_SERIAL)
{

    uint64_t timeOld = millis();
	
	while (!buffer.endsWith("OK\r\n") && !buffer.endsWith("ERROR\r\n") && !(millis() > timeOld + timeout) && buffer.length() < max)
    {
        while(this->NeoSWSerial::available() && buffer.length() < max)
        {
            if (this->NeoSWSerial::available()>0)
            {
                buffer += (char) this->NeoSWSerial::read();
            }
        }       
    }
	
    String str = buffer.substring(buffer.length() - 6);
    while (!str.endsWith("OK\r\n") && !str.endsWith("ERROR\r\n") && !(millis() > timeOld + timeout))
    {
        while(this->NeoSWSerial::available())
        {
            if (this->NeoSWSerial::available()>0)
            {
                str += (char) this->NeoSWSerial::read();
            }
			if (str.length() > 50) {
				str = str.substring(str.length() - 6);
			}
        }       
    }

}

// After read a char reset the millis. It is for read line in the first try
// timeout can be a low number, example = 10
String Sim800L::_readSerialUntilTime(uint32_t timeout)
{

    uint64_t timeOld = millis();
    String str;

    while (!(millis() > timeOld + timeout))
    {
        while(this->NeoSWSerial::available())
        {
            if (this->NeoSWSerial::available()>0)
            {
                str += (char) this->NeoSWSerial::read();
            }
			timeOld = millis();
        }		
    }

    return str;

}

String Sim800L::_readSerial(uint32_t timeout = TIME_OUT_READ_SERIAL)
{

    uint64_t timeOld = millis();

    while (!this->NeoSWSerial::available() && !(millis() > timeOld + timeout))
    {
        delay(13);
    }

    String str;

    while(this->NeoSWSerial::available())
    {
        if (this->NeoSWSerial::available()>0)
        {
            str += (char) this->NeoSWSerial::read();
        }
    }

    return str;

}

