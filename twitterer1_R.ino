#include <iSdio.h>
#include <utility/Sd2CardExt.h>

const int chipSelectPin = 10;
Sd2CardExt card;
uint8_t buffer[512];
char mes[280];
int menum=0;
uint32_t nextSequenceId = 0;
int but1 = 0, but2 = 0, but3 = 0, but4 = 0;
const int button1 = 4;
const int button2 = 7;
const int button3 = 5;
const int button4 = 6;
const int led = 2;



void setup()
{
  pinMode(led, OUTPUT);
  pinMode(button1,INPUT_PULLUP);
  pinMode(button2,INPUT_PULLUP);
  pinMode(button3,INPUT_PULLUP);
  pinMode(button4,INPUT_PULLUP);  
 
  card.init(SPI_HALF_SPEED, chipSelectPin);
  if (card.readExtMemory(1, 1, 0x420, 0x34, buffer)) {
    if (buffer[0x20] == 0x01) {
      nextSequenceId = get_u32(buffer + 0x24);
      iSDIO_waitResponse(nextSequenceId);
      nextSequenceId++;
    } else {
      nextSequenceId = 0; 
    }
  } else {
    nextSequenceId = 0; 
  }
  
  iSDIO_connect(nextSequenceId, "INSERT YOUR SSID", "INSERT YOUR WLAN PASSWORD");
  iSDIO_waitResponse(nextSequenceId);
} 


void loop(){
  
  but1 = digitalRead(button1);
  but2 = digitalRead(button2);
  but3 = digitalRead(button3);
  but4 = digitalRead(button4);
  
  if (but1 == LOW) {
    digitalWrite(led, LOW);
    twit("寿限無 寿限無 五劫の擦り切れ 海砂利水魚の水行末 雲来末 風来末 食う寝る処に住む処""藪ら柑子の藪柑子 パイポパイポ パイポのシューリンガン シューリンガンのグーリンダイ ""グーリンダイのポンポコピーのポンポコナーの 長久命の長助");
    delay(60000);  /// taking one minute break
  } 
  else {
    digitalWrite(led, HIGH); 
  }
  
  if (but2 == LOW) {
    digitalWrite(led, LOW);
    twit("Degas, are we not drawn onward, we freer few, drawn onward to new eras aged?");
    delay(60000);  
  } 
  else {
    digitalWrite(led, HIGH); 
  }
  if (but3 == LOW) {
    digitalWrite(led, LOW);
    twit("We cannot become what we need by remaining what we are. - J. C. Maxwell");
    delay(60000);  
  } 
  else {
    digitalWrite(led, HIGH); 
  }
  if (but4 == LOW) {
    digitalWrite(led, LOW);
    twit("In the twenty-first century, the robot will take the place which slave labor occupied in ancient civilization. -- Nocola Tesla");
    delay(60000);  
  } 
  else {
    digitalWrite(led, HIGH); 
  }
  
}

void twit(char* message){
  iSDIO_http(nextSequenceId,message);
  delay(1000);
  iSDIO_waitResponse(nextSequenceId);
  delay(500);
  iSDIO_httpResponse();
}


int iSDIO_waitResponse(uint32_t sequenceId) {
  uint8_t prev = 0xFF;
  for (int i = 0; i < 20; ++i) {
    memset(buffer, 0, 0x14);

    if (!card.readExtMemory(1, 1, 0x440, 0x14, buffer)) {
      return false;
    }

    uint8_t resp = get_u8(buffer + 8);
    if (sequenceId == get_u32(buffer + 4)) {
     if (prev != resp) {
        switch (resp) {
          case 0x00:
//            Serial.print(F("\n  Initial"));
            break;
          case 0x01:
//            Serial.print(F("\n  Command Processing"));
            break;
          case 0x02:
//            Serial.println(F("\n  Command Rejected"));
            return false;
          case 0x03:
 //           Serial.println(F("\n  Process Succeeded"));
            return true;
          case 0x04:
 //           Serial.println(F("\n  Process Terminated"));
            return false;
          default:
 //           Serial.print(F("\n  Process Failed "));
 //           Serial.println(resp, HEX);
            return false;
        }
        prev = resp;
      }
    }
    //Serial.print(F("."));
    //delay(1000);
  }
  return false;
}

  int iSDIO_connect(uint32_t sequenceId, const char* ssid, const char* networkKey) {
  //Serial.print(F("\nConnect command: \n"));
  memset(buffer, 0, 512);
  uint8_t* p = buffer;
  p = put_command_header(p, 1, 0);
  p = put_command_info_header(p, 0x02, sequenceId, 2);
  p = put_str_arg(p, ssid);
  p = put_str_arg(p, networkKey);
  put_command_header(buffer, 1, (p - buffer));
 // printHex(buffer, (p - buffer));
  return card.writeExtDataPort(1, 1, 0x000, buffer) ? true : false;
}


int iSDIO_http(uint32_t sequenceId, const char* message) {
 // Serial.print(F("\nhttp command: \n"));
  int len = strlen(message) + 40;
  char leng[3];
  const char *s1 = "POST /api/post/ HTTP/1.0\r\nHost:stewgate-u.appspot.com\r\nContent-Length:";
  // and length of message
  const char *s2 = "\r\n\r\n_t=INSERT_YOUR_STEWGATE_TOKEN_HERE&msg=";
  // and message
  const char *s3 = "\r\n\r\n";
  size_t  total_len = strlen(s1) + strlen(s2)+strlen(message)+strlen(s3);
  char *buf = (char *)malloc(total_len + 4);
  itoa(len,leng,10);
  buf[total_len] = 0;
  strcpy(buf, s1);
  strcat(buf, leng);
  strcat(buf, s2);
  strcat(buf, message);
  strcat(buf, s3);
  memset(buffer, 0, 512);
  uint8_t* p = buffer;
  p = put_command_header(p, 1, 0);
  p = put_command_info_header(p, 0x23, sequenceId, 2);
  p = put_str_arg(p, "74.125.204.141");  // Argument #1.
  p = put_str_arg(p, buf);
  put_command_header(buffer, 1, (p - buffer));
 // printHex(buffer, (p - buffer));
  return card.writeExtDataPort(1, 1, 0x000, buffer) ? true : false;
  free(buf);
}

int iSDIO_httpResponse() {
  // Read header and data.
  if (!card.readExtDataPort(1, 1, 0x200, buffer)) {
    return false;
  }
  uint32_t totalSize = get_u32(buffer + 20);
  uint32_t availableSize = totalSize > 488 ? 488 : totalSize;
  uint32_t pos = 24;
  for (;;) {
    for (uint32_t i = 0; i < availableSize; ++i) {
      Serial.print((char)buffer[pos + i]);
    }
    totalSize -= availableSize;
    
    // Have we read all data?
    if (totalSize == 0) break;
    
    // Read next data.
    if (!card.readExtDataPort(1, 1, 0x200, buffer)) {
      return false;
    }
    availableSize = totalSize > 512 ? 512 : totalSize;
    pos = 0;
  }
  return true;
}



