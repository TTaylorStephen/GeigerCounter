double currentH;
double percentH;
double offset = 360; //maxH-minH


void moisturePercent(){
  
  double minH=600-offset;
  double maxH=240+offset;
  currentH = analogRead(0);
  
   //offset (-) the incoming value if greater than 240+(offset/2)
   if (currentH>=420){
    currentH-=360;
   }
   else{
    currentH+=360;
   }
   double scaleH = currentH/maxH;
   percentH = ((currentH-minH)/(maxH-minH))*scaleH*100;
}

void setup() {
  Serial.begin(9600); // open serial port, set the baud rate as 9600 bps
}
void loop() {
  moisturePercent();
  Serial.print(percentH);
  Serial.print("\n");
  Serial.print("\t"); //print the value to serial port 
  
  Serial.print(currentH);
  Serial.print("\n");
  delay(100);
}
