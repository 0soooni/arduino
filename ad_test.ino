#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN); //rfid 객체 생성

#include <Servo.h>   
#define servoPin 8 
#define piezo 4   
Servo servo;     
int pos=0;      
int RLED=7;     
int GLED=6;
int BLED=5;

void setup() 
{
  pinMode(RLED, OUTPUT);      // RED LED를 출력으로 지정
  pinMode(GLED, OUTPUT);      // GREEN LED를 출력으로 지정
  pinMode(piezo, OUTPUT);
  servo.attach(servoPin);     // 서보모터 핀을 설정한다
  servo.write(0);       // 서보모터 0도 위치로 초기화
  Serial.begin(9600);   // 시리얼 통신 시작
  SPI.begin();      // SPI 통신 시작
  rfid.PCD_Init();   // rfid 초기화
  //서버에서 결제완료 신호 받으면
  Serial.println("리더기에 카드를 태그해주세요.");
  Serial.println();
  digitalWrite(BLED,HIGH); 
}

void loop() 
{
  // 새카드 접촉
  if ( ! rfid.PICC_IsNewCardPresent()) 
  {     
    return;
  }
  // 카드 읽힘
  if ( ! rfid.PICC_ReadCardSerial()) 
  {
    return;
  }
  // UID 값을 16진 값으로 읽고 저장한 후 시리얼 모니터로 표시
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
    Serial.println("등록된 카드입니다.");
    Serial.println();
    tone(piezo, 500, 1000);
    digitalWrite(BLED,LOW); 
    
    // 서버 전송 후 승인 처리가 오면 아래 코드 실행
    digitalWrite(GLED,HIGH); 
    delay(2000);
    digitalWrite(GLED,LOW); 
    Serial.println("OPEN!");
    Serial.println();
    servo.write(180);   // 서보모터 각도 변경  
    delay(3000); 
    servo.write(0);  
  }
  
  else   {    // 서보모터는 작동 없이 Red LED만 켜고 끈다
    Serial.println("등록된 카드가 아닙니다. 다시 확인해주세요."); 
    digitalWrite(BLED,LOW); 
    digitalWrite(RLED,HIGH); 
    delay(3000);
    digitalWrite(RLED,LOW); 

  /*
  if ( 서버측 대여 승인 신호) {
    Seria.println("대여가 승인되었습니다.");
    digitalWrite(GLED,HIGH); 
    delay(2000);
    digitalWrite(GLED,LOW); 
    Serial.println("OPEN!");
    Serial.println();
    servo.write(180);        
    delay(3000); 
    servo.write(0); 
    }
   else {
    Serial.println("대여 승인이 옳바르게 처리되지 않습니다."); 
    digitalWrite(BLED,LOW); 
    digitalWrite(RLED,HIGH); 
    delay(3000);
    digitalWrite(RLED,LOW); 
    } // 이후는 계속 승인이 안되면 서버측에서 알림을 보내는 건가?
   */
  }
} 
