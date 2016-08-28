const int moisture_gro = A0; //ground
const int moisture_top = A1;
const int moisture_mid = A2;
const int moisture_bot = A3; //bottom
int gro;
int top;
int mid;
int bot;
 
void setup() {
  pinMode(moisture_gro, OUTPUT);
  pinMode(moisture_top, OUTPUT);
  pinMode(moisture_mid, OUTPUT);
  pinMode(moisture_bot, OUTPUT);
  Serial.begin(9600);
}
 
void loop() {
  int gro = analogRead(moisture_gro);
  int top = analogRead(moisture_top);
  int mid = analogRead(moisture_mid);
  int bot = analogRead(moisture_bot);
  Serial.print("ground : ");
  Serial.println(gro);
  Serial.print("top : ");
  Serial.println(top);
  Serial.print("mid : ");
  Serial.println(mid);
  Serial.print("bottom");
  Serial.println(bot);
 // Serial.print("_");
}
