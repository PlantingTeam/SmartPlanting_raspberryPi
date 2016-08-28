/*
Author:KwangEun
board 2
*/
void setup(){
 Serial.begin(9600); 
}
void loop(){
 Serial.print("BOARD-2:");
 for (int i=0;i<6;i++){
  Serial.print(analogRead(i));
  Serial.print("_");
 } 
 Serial.println();
}
