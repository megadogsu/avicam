#define BUFF 63
unsigned char randNum, ACK;

void setup(){
  Serial.begin(57600);
  randomSeed(473);
  pinMode(13, OUTPUT);
  // set led high, will go low on error
  digitalWrite(13, HIGH);
}

void loop(){
  int i;
  uint8_t buff[BUFF];
  // start of session handshake
  Serial.write('S');
  
  // wait for response
  while(Serial.available() < 1){}
  
  // get the ack signal
  ACK = Serial.read();
  
  // acknowledge flag is wrong
  if(ACK != 'A') digitalWrite(13, LOW);
  
  for(i = 0; i < BUFF; i++){
  	randNum = random(255);
  	buff[i] = randNum;
  }
  
  Serial.write(buff, BUFF);
  
  // this delay is new. avoiding back-to-back serial writes
  
}
