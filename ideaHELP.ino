#define IR A1
#define IR2 A2
#define signal 2

int zero;
int zero2;
int score = 0;

int zeros(int sensor){
  int tot = 0;
  for (int i = 0; i < 100; i++) {
    tot += analogRead(sensor);
  }
  zero = tot / 100;
  return zero;
} 

void setup() {

  pinMode(signal, OUTPUT);
  digitalWrite(signal, LOW);
  Serial.begin(9600);

  zero = zeros(IR);
  Serial.print("zero: ");
  Serial.println(zero);
  zero2 = zeros(IR2);

  delay(1000);
}

void loop() {

  int dist = analogRead(IR) - zero;
  int dist2 = analogRead(IR2) - zero2;
  Serial.println(dist);
  //Serial.println("Dist 2" + dist2);

  if (dist > 200 || dist2 > 200) {
    
    digitalWrite(signal, HIGH);
    score++;
    Serial.println("score!");
    delay(1000);
    digitalWrite(signal, LOW);
    
  }
}

