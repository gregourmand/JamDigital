
long s = 0;
long sec = 0;
long hrs = 0;
long minutes = 0;
long initialHours = 11;//set jam awal
long initialMins = 59;//set menit awal
long initialSecs = 40;//set detik awal

signed char jam_satuan, jam_puluhan, menit_satuan, menit_puluhan, detik_satuan, detik_puluhan;

int digsegment[] ={2,3,4,5,6,7,8};     // pin a, b, c, d, e, f, g, dp
int digit[]  = {9,10,11,12,13,19};               // Selector 7-segment
int state=0;

//first loop variable
boolean start = true;

int count = 0;

//this method is for seconds
long seconds()
{
  s = initialHours * 3600;
  s = s + (initialMins * 60);
  s = s + initialSecs;
  s = s + (millis() / 1000);
  return s;
}

//this method is for hours
long hours()
{
  hrs = seconds();
  hrs = hrs / 3600;
  hrs = hrs % 24;
  return hrs;
}

//this method is for minutes
long mins()
{
  minutes = seconds();
  minutes = minutes / 60;
  minutes = minutes % 60;
  return minutes;
}

long secs()
{
  sec = seconds();
  sec = sec % 60;
  return sec;
}

void setup() {
  // initialize the digital pins as outputs.
  pinMode(digsegment[0], OUTPUT);
  pinMode(digsegment[1], OUTPUT);
  pinMode(digsegment[2], OUTPUT);
  pinMode(digsegment[3], OUTPUT);
  pinMode(digsegment[4], OUTPUT);
  pinMode(digsegment[5], OUTPUT);
  pinMode(digsegment[6], OUTPUT);
  pinMode(digit[0], OUTPUT);
  pinMode(digit[1], OUTPUT);
  pinMode(digit[2], OUTPUT);
  pinMode(digit[3], OUTPUT);
  pinMode(digit[4], OUTPUT);
  pinMode(digit[5], OUTPUT);

  // INPUT
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);

  //https://forum.arduino.cc/t/creating-a-1ms-interrupt-using-the-tccr/487329/4
  // TCCR
    // CPU Freq 16Mhz
    // Need interval of 1Ms ==> 0,001/(1/16000000) = 16.000 ticks
    //
    // Prescaler 64 ==> resolution changes from 0,0000000625 to 0,000004
    // Need interval of 1Ms ==> 0,001/((1/16000000)*64) = 250 ticks


    // Set prescaler to 64 ; (1 << CS01)|(1 << CS00)
    // Clear Timer on Compare (CTC) mode ; (1 << WGM02)
    TCCR0A = 0;
    TCCR0B |= (1 << WGM02) | (1 << CS01) | (1 << CS00);


    // set Output Compare Register to (250 - 1) ticks
    OCR0A = 0xF9;

    // TCNT0
    // Timer count = (required delay/clock time period) - 1
    // 249 = (0,001/0,000004) - 1

    // initialize counter
    TCNT0 = 0;

    // TIMSK0
    // Set Timer Interrupt Mask Register to 
    // Clear Timer on Compare channel A for timer 0
    TIMSK0 |= (1 << OCIE0A);
  Serial.begin(9600);
}

//--------- EDIT TIME START ------------
int sub_state_edt=0; // 0 menit, 1 second, 2 hour
void task_state1(){ // EDIT TIME
  //int sub_state_edt=0; // 0 menit, 1 second, 2 hour
  tampiljam(jam_puluhan, jam_satuan, menit_puluhan, menit_satuan, detik_puluhan, detik_satuan); 

  if(sub_state_edt==0 && digitalRead(15)==HIGH){ 
    delay(100);
    sub_state_edt=1;
  }else if(digitalRead(15)==HIGH && sub_state_edt==1){ 
    delay(100);
    sub_state_edt=2;
  }if(digitalRead(15)==HIGH && sub_state_edt==2){ 
    delay(100);
    sub_state_edt=0;
  }if(digitalRead(17)==HIGH){
    delay(100);
    sub_state_edt=0;
  }

  if(sub_state_edt == 0){
    if(digitalRead(16)==HIGH){
      delay(100);
      initialMins++;
      delay(100);
    }else if(digitalRead(14)==HIGH){
      
      initialMins--;
      delay(100);
    }
  }
  if(sub_state_edt == 1){
    if(digitalRead(16)==HIGH){
      delay(100);
      initialSecs++;
    }else if(digitalRead(14)==HIGH){
      delay(100);
      initialSecs--;
    }
  }
  if(sub_state_edt == 2){
    if(digitalRead(16)==HIGH){
      delay(100);
      initialHours++;
    }else if(digitalRead(14)==HIGH){
      delay(100);
      initialHours--;
    }
  }
  //Serial.println(sub_state_edt);
  //Serial.println(state);
}
//-------- EDIT TIME END ------------

//--------- STOPWATCH START----------

int sub_state_sth = 0; // 0 ketika reset, 1 ketika start, 2 ketika pause
unsigned long last=0;
int menit_plh_sth, menit_st_sth, detik_plh_sth, detik_st_sth, ms_plh_sth, ms_st_sth;
unsigned long tick,tick2, set_detik, set_menit;

ISR(TIMER0_COMPA_vect) {

    if(sub_state_sth==1){
      tick++;
      tick2++;
    }
}

void task_state2(){
  
  //int sub_state_sth = 0; // 0 ketika reset, 1 ketika start, 2 ketika pause
  
  tampiljam(menit_plh_sth, menit_st_sth, detik_plh_sth, detik_st_sth, ms_plh_sth, ms_st_sth); 

  if(digitalRead(16)==HIGH && sub_state_sth==0){ // ketika awalnya reset diteken T3, ke start substate1
    delay(100);
    sub_state_sth=1;
  }else if(digitalRead(16)==HIGH && sub_state_sth==1){ // ketika di start diteken T3, ke stop substate2
    delay(100);
    sub_state_sth=2;
  }else if(digitalRead(16)==HIGH && sub_state_sth==2){
    delay(100);
    sub_state_sth=1;
  }
  if(digitalRead(14)== HIGH && (sub_state_sth==1 || sub_state_sth==2)){
    delay(100);
    sub_state_sth=0;
  }
  if(digitalRead(17)==HIGH){
    delay(100);
    sub_state_sth=0;
  }

  // RESET
  if(sub_state_sth==0){
    menit_plh_sth = 0; menit_st_sth = 0; 
    detik_plh_sth = 0; detik_st_sth = 0;
    ms_plh_sth = 0;ms_st_sth = 0;
    tick=0; tick2=0;
  }
  // START
  if(sub_state_sth==1){
//    if(millis()-last >= 1){
//      last=millis();
//      tick++;
//    }
    //tick=0;
    //tick2=0;

    tick2=tick%1000;
    
    
    ms_plh_sth = (tick2/10) / 10;
    ms_st_sth = (tick2/10) % 10;
    Serial.print("tick: ");
    Serial.println(tick);
    Serial.print(" plh :");
    Serial.println(ms_plh_sth);
    Serial.print(" satuan: ");
    Serial.println(ms_st_sth);

    set_detik=tick/1000;
    set_detik=set_detik % 60;
    detik_plh_sth=set_detik / 10;
    detik_st_sth=set_detik % 10;
    Serial.println(detik_plh_sth);
    Serial.println(detik_st_sth);

    set_menit=(tick/1000)/60;
    set_menit=set_menit % 60 ;
    
    menit_plh_sth=set_menit / 10;
    menit_st_sth=set_menit % 10;
    Serial.println(menit_plh_sth);
    Serial.println(menit_st_sth);
    tampiljam(menit_plh_sth, menit_st_sth, detik_plh_sth, detik_st_sth, ms_plh_sth, ms_st_sth); 

  }
  // PAUSE
  else if(sub_state_sth==2){
    ms_plh_sth=ms_plh_sth;
    ms_st_sth = ms_st_sth;
    detik_plh_sth=detik_plh_sth;
    detik_st_sth=detik_st_sth;
    menit_plh_sth=menit_plh_sth;
    menit_st_sth=menit_st_sth;
  }
  Serial.println(sub_state_sth);
  
}

//--------- STOPWATCH END --------------

//--------- ALARM START ---------------
int sub_state_alarm=0;
int on=0;
void task_state3(){ // ALARM
   tampiljam(jam_puluhan, jam_satuan, menit_puluhan, menit_satuan, detik_puluhan, detik_satuan); 

  if(digitalRead(15)==HIGH && sub_state_edt==0){ 
    delay(20);
    sub_state_alarm=1;
  }else if(digitalRead(15)==HIGH && sub_state_edt==1){ 
    delay(20);
    sub_state_alarm=2;
  }if(digitalRead(15)==HIGH && sub_state_edt==2){ 
    delay(20);
    sub_state_alarm=0;
  }
  
}
//--------- ALARM END ----------------

void loop() {

  hours();
  mins();
  secs();

  jam_satuan = hrs % 10; // nilai hrs mod 10 dapat satuan
  jam_puluhan = hrs / 10;
  
  menit_satuan = minutes % 10; // nilai menit mod 10 dapat satuan menit
  menit_puluhan = minutes / 10;

  detik_satuan = sec % 10;
  detik_puluhan = sec / 10;

  if(state==0 && digitalRead(17)==HIGH ){
    
    state=1;
    delay(100);
  } else if(state==1 && digitalRead(17)==HIGH){
    
    state=2;
    delay(100);
  } else if(state==2 && digitalRead(17)==HIGH){
    
    state=3;
    delay(100);
  } else if(state==3 && digitalRead(17)==HIGH){
    
    state=0;
    delay(100);
  }  

  if(state==1){
    task_state1();
  }else if(state==2){
    task_state2();
  }else if(state==3){
    task_state3();
  }else if(state==0){
    tampiljam(jam_puluhan, jam_satuan, menit_puluhan, menit_satuan, detik_puluhan, detik_satuan); 
  }
  //Serial.println(state);
}
//------------------------------//


// menampilkan jam digital
void tampiljam(signed char a, signed char b, signed char c, signed char d, signed char e, signed char f)
{   
    
    tampil_7(15); // all low
    
    digitalWrite(digit[5],LOW);
    digitalWrite(digit[4],LOW);
    digitalWrite(digit[3],LOW);  // Digit 1
    digitalWrite(digit[2],LOW);
    digitalWrite(digit[1],LOW);
    digitalWrite(digit[0],HIGH);
    tampil_7(a);
    delay(3);  

    tampil_7(15);
    digitalWrite(digit[5],LOW);
    digitalWrite(digit[4],LOW);
    digitalWrite(digit[3],LOW);  // Digit 2
    digitalWrite(digit[2],LOW);
    digitalWrite(digit[1],HIGH);
    digitalWrite(digit[0],LOW);
    tampil_7(b);
    delay(3); 

    tampil_7(15);
    digitalWrite(digit[5],LOW);
    digitalWrite(digit[4],LOW);
    digitalWrite(digit[3],LOW);  // Digit tiga
    digitalWrite(digit[2],HIGH);
    digitalWrite(digit[1],LOW);
    digitalWrite(digit[0],LOW);
    tampil_7(c);
    delay(3); 

    tampil_7(15);
    digitalWrite(digit[5],LOW);
    digitalWrite(digit[4],LOW);
    digitalWrite(digit[3],HIGH);  // Digit empat
    digitalWrite(digit[2],LOW);
    digitalWrite(digit[1],LOW);
    digitalWrite(digit[0],LOW);
    tampil_7(d);
    delay(3); 

    tampil_7(15);
    digitalWrite(digit[5],LOW);
    digitalWrite(digit[4],HIGH);
    digitalWrite(digit[3],LOW);  // Digit lima
    digitalWrite(digit[2],LOW);
    digitalWrite(digit[1],LOW);
    digitalWrite(digit[0],LOW);
    tampil_7(e);
    delay(3); 

    tampil_7(15);
    digitalWrite(digit[5],HIGH);
    digitalWrite(digit[4],LOW);
    digitalWrite(digit[3],LOW);  // Digit Enam
    digitalWrite(digit[2],LOW);
    digitalWrite(digit[1],LOW);
    digitalWrite(digit[0],LOW);
    tampil_7(f);
    delay(3); 
}

// Menampilakn angka pada 7-segment common anode : digit nyala high, pin 7 segment nyala low
void tampil_7(int nilai) 
{
  if (nilai==0){
            digitalWrite(digsegment[0], LOW);
            digitalWrite(digsegment[1], LOW);
            digitalWrite(digsegment[2], LOW);
            digitalWrite(digsegment[3], LOW);
            digitalWrite(digsegment[4], LOW);      
            digitalWrite(digsegment[5], LOW);
            digitalWrite(digsegment[6], HIGH);
            
  }
  else if (nilai==1) {
            digitalWrite(digsegment[0], HIGH);
            digitalWrite(digsegment[1], LOW);
            digitalWrite(digsegment[2], LOW);
            digitalWrite(digsegment[3], HIGH);
            digitalWrite(digsegment[4], HIGH);
            digitalWrite(digsegment[5], HIGH);
            digitalWrite(digsegment[6], HIGH); 
            
  }
  else if (nilai==2){
            digitalWrite(digsegment[0], LOW);
            digitalWrite(digsegment[1], LOW);
            digitalWrite(digsegment[2], HIGH);
            digitalWrite(digsegment[3], LOW);
            digitalWrite(digsegment[4], LOW);
            digitalWrite(digsegment[5], HIGH);
            digitalWrite(digsegment[6], LOW);
            
  }
  else if (nilai==3)  {
          digitalWrite(digsegment[0], LOW);
          digitalWrite(digsegment[1], LOW);
          digitalWrite(digsegment[2], LOW);
          digitalWrite(digsegment[3], LOW);
          digitalWrite(digsegment[4], HIGH);
          digitalWrite(digsegment[5], HIGH);
          digitalWrite(digsegment[6], LOW);
          
  }
  else if (nilai==4){
            digitalWrite(digsegment[0], HIGH);
            digitalWrite(digsegment[1], LOW);
            digitalWrite(digsegment[2], LOW);
            digitalWrite(digsegment[3], HIGH);
            digitalWrite(digsegment[4], HIGH);
            digitalWrite(digsegment[5], LOW);
            digitalWrite(digsegment[6], LOW);
           
  }
  else if (nilai==5){
            digitalWrite(digsegment[0], LOW);
            digitalWrite(digsegment[1], HIGH);
            digitalWrite(digsegment[2], LOW);
            digitalWrite(digsegment[3], LOW);
            digitalWrite(digsegment[4], HIGH);
            digitalWrite(digsegment[5], LOW);
            digitalWrite(digsegment[6], LOW);
            
  }
  else if (nilai==6){
            digitalWrite(digsegment[0], LOW);
            digitalWrite(digsegment[1], HIGH);
            digitalWrite(digsegment[2], LOW);
            digitalWrite(digsegment[3], LOW);
            digitalWrite(digsegment[4], LOW);
            digitalWrite(digsegment[5], LOW);
            digitalWrite(digsegment[6], LOW);
            
  }
  else if (nilai==7){
            digitalWrite(digsegment[0], LOW);
            digitalWrite(digsegment[1], LOW);
            digitalWrite(digsegment[2], LOW);
            digitalWrite(digsegment[3], HIGH);
            digitalWrite(digsegment[4], HIGH);
            digitalWrite(digsegment[5], HIGH);
            digitalWrite(digsegment[6], HIGH);
            
  }
  else if (nilai==8){
            digitalWrite(digsegment[0], LOW);
            digitalWrite(digsegment[1], LOW);
            digitalWrite(digsegment[2], LOW);
            digitalWrite(digsegment[3], LOW);
            digitalWrite(digsegment[4], LOW);
            digitalWrite(digsegment[5], LOW);
            digitalWrite(digsegment[6], LOW);
            
  }
  else if (nilai==9)
  {
            digitalWrite(digsegment[0], LOW);
            digitalWrite(digsegment[1], LOW);
            digitalWrite(digsegment[2], LOW);
            digitalWrite(digsegment[3], LOW);
            digitalWrite(digsegment[4], HIGH);
            digitalWrite(digsegment[5], LOW);
            digitalWrite(digsegment[6], LOW);
            
  }
  else if (nilai==10)
  {
            digitalWrite(digsegment[0], LOW);
            digitalWrite(digsegment[1], LOW);
            digitalWrite(digsegment[2], LOW);
            digitalWrite(digsegment[3], LOW);
            digitalWrite(digsegment[4], LOW);
            digitalWrite(digsegment[5], LOW);
            digitalWrite(digsegment[6], HIGH);
         
  }else if(nilai=15){
    // all low
            digitalWrite(digsegment[0], HIGH);
            digitalWrite(digsegment[1], HIGH);
            digitalWrite(digsegment[2], HIGH);
            digitalWrite(digsegment[3], HIGH);
            digitalWrite(digsegment[4], HIGH);
            digitalWrite(digsegment[5], HIGH);
            digitalWrite(digsegment[6], HIGH);
  }
}
