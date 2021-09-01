
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN); //rfid 객체 생성

#include <Servo.h>    // 서보 라이브러리를 지정
#define servoPin 8     // 서보 모터핀을 지정
Servo servo;      // 서보 라이브러리 변수를 선언
int pos=0;      // 현재 각도를 저장할 변수를 지정한다
int RLED=7;     // 빨간색 LED단자를 아두이노 7번과 연결
int GLED=6;     // 초록색 LED단자를 아두이노 6번과 연결

void setup() 
{
  pinMode(RLED, OUTPUT);      // RED LED를 출력으로 지정
  pinMode(GLED, OUTPUT);      // GREEN LED를 출력으로 지정
  servo.attach(servoPin);     // 서보모터 핀을 설정한다
  servo.write(0);       // 서보모터 0도 위치로 초기화
  Serial.begin(9600);   // 시리얼 통신 시작
  SPI.begin();      // SPI 통신 시작
  rfid.PCD_Init();   // rfid(MFRC522) 초기화
  Serial.println("리더기에 카드를 태그해주세요.");
  Serial.println();
}

void loop() 
{
  // 새카드 접촉이 있을 때만 다음 단계로 넘어감
  if ( ! rfid.PICC_IsNewCardPresent()) 
  {     
    return;
  }
  // 카드 읽힘이 제대로 되면 다음으로 넘어감
  if ( ! rfid.PICC_ReadCardSerial()) 
  {
    return;
  }
  // UID 값을 16진 값으로 읽고 저장한 후 시리얼 모니터로 표시함
  Serial.print("카드 UID :");
  String content= "";
  byte letter;
  for (byte i = 0; i < rfid.uid.size; i++) 
  {
     Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(rfid.uid.uidByte[i], HEX);
     content.concat(String(rfid.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(rfid.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("메세지 : ");
  content.toUpperCase();
  if (content.substring(1) == "2C 54 F9 38")// 엑세스 승인 하고자하는 UID 기록
  { 
  // 인증이 되면 Green LED와 함께 서보모터를 작동시킨다.
    Serial.println("등록된 카드가 맞습니다.");
    Serial.println();
    digitalWrite(GLED,HIGH); 
    delay(2000);
    digitalWrite(GLED,LOW); // 시간지연 후 LED 끈다
    Serial.print("OPEN!");
    Serial.println();
    servo.write(180);   // 서보모터의 각도를 변경한다      
    delay(3000); // 서보 모터의 각도가 변하는 것을 기다려 준다. 
    servo.write(0);     // 시간지연 후 문을 닫는다  
  }
  
  else   {    // 서보모터는 작동 없이 Red LED만 켜고 끈다
    Serial.println("등록된 카드가 아닙니다. 다시 확인해주세요."); // 그외 UID는 승인 거부 표시
    digitalWrite(RLED,HIGH); 
    delay(3000);
    digitalWrite(RLED,LOW); 
  }
} 
