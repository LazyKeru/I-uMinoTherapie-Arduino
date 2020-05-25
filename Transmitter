#include <Arduino.h> //Had to include the Arduino.h library, or else we couldn't define or detect our pointers in the NULL State.

//Connected modules:
const int LedRG = 13;                       //Pin commanding Red and Green Led
const int LDR = 14;                         //Pin receiving LDR value

/*# Information about the LDR #*/

#define VIN 5 // V power voltage
#define R 10000 //ohm resistance value

/* information returned after scanning for a tram */

int information_received_global;  //Used in the setup and loop functions

/* -== ID of Trame ==- */
/*Header info*/
const int headerID_Length PROGMEM = 4;
const int headerID[headerID_Length] = {'D', 'D', 'A', 'P'};

/* -== First Trame ==- */
/*Header info*/
const int headerLength_1st_Trame = 2;
const int header_1st_Trame[headerLength_1st_Trame] = {'%', '&'};

/*Body info*/
const int bodyLength_1st_Trame = 11;
unsigned int body_1st_Trame[bodyLength_1st_Trame] = {'*',0xFF,0xFF,'+',0xFF,'+',0xFF,0xFF,'$',0xFF,0xFF};

/*Footer info*/
const int footerLength_1st_Trame = 3;
const int footer_1st_Trame[footerLength_1st_Trame] = {'@', ':', '@'};

/* -== Second Trame ==- */
/*Header info*/
const int headerLength_2nd_Trame = 2;
const int header_2nd_Trame[headerLength_2nd_Trame] = {'&', '%'};

/*Footer info*/
const int footerLength_2nd_Trame = 3;
const int footer_2nd_Trame[footerLength_2nd_Trame] = {':', '@', ':'};

/*--------------------------------------------------------------------------------*/
/*================================================================================*/
/*--------------------------------------------------------------------------------*/

/* -==  My stack structure to stock the Trame it receives  ==- */

typedef struct ElementTrameStructure {
  int Byte;
  struct ElementTrameStructure * next = NULL;
} ElementTrame;

//Might be better to put it in a .h file. As arduino sometimes compiles functions before structures.
// ElementTrame doesn't work well on tinkercad. So when using tinkercad, replace ElementTrame by struct ElementTrameStructure

/* Function to create the structure which will stock the Trame */

ElementTrame* init_trame() {
  ElementTrame* element_trame = (ElementTrame*)malloc(sizeof(ElementTrame));
  if (element_trame == NULL) {
    exit(EXIT_FAILURE);
  }
  element_trame->Byte = 0x00;
  element_trame->next = NULL;
  return element_trame;
}

/* Function to remove the first element of the Trame list */

void pop(ElementTrame ** top_trame) {
  ElementTrame * next_element_trame = NULL;
  if (*top_trame == NULL) {
    exit(EXIT_FAILURE);
  }
  next_element_trame = (*top_trame)->next;
  free(*top_trame);
  *top_trame = next_element_trame;
  return;
}

/* Function to add elemenent to Trame list */

void pileup(ElementTrame ** top_trame, int incomingByte) {
  ElementTrame* new_element_trame;
  new_element_trame = (ElementTrame*)malloc(sizeof(ElementTrame));
  if (new_element_trame == NULL) {
    exit(EXIT_FAILURE);
  }
  new_element_trame->Byte = incomingByte;
  new_element_trame->next = *top_trame;
  *top_trame = new_element_trame;
  return;
}

/* Function to delete a Trame */

void delete_trame(ElementTrame * top_trame) {
  while (top_trame->next != NULL) { //Erases the trame_checked
    pop(&top_trame);
  }
  pop(&top_trame);
  return;
}

/*--------------------------------------------------------------------------------*/
/*================================================================================*/
/*--------------------------------------------------------------------------------*/

/* -==  Receiving a Trame / Getting the info  ==- */

/* function that prints one byte */

int print_received_byte(int incoming_byte) {
  //Serial.print(incoming_byte, HEX); //Remove comments when you want to see the received byte
  //Serial.print(" ");
}

/* function to extract informations from the trams */

int receive_trame_info() {
  int res;   // 0: Time out / 1: Header error / 2: Footer error / 3: Error in the received Trame / 4: The info isn't recognised by our software
  int i = 0;      // 5: info - Manual / 6: info - Automatic / 7: info - Nack / 8: info - Ack
  int incoming_byte = -1;
  int trame_type = -1; // 1: 1st trame type / 2: 2nd trame type
  //It waits to receive the start of what could be one of two trame. If it doesn't, well it times out.
  while ((incoming_byte != header_1st_Trame[0] && incoming_byte != header_2nd_Trame[0]) && i < 1000) {
    incoming_byte = Serial.read();
    i++;
  }
  Serial.print("Receiving Bytes : ");
  print_received_byte(incoming_byte);
  if (incoming_byte == header_1st_Trame[0]) {
    trame_type = 1;
    //Serial.println("The byte is similar to the header of our 1st header type.");
  } else if (incoming_byte == header_2nd_Trame[0]) {
    trame_type = 2;
    //Serial.println("The byte is similar to the header of our 2nd header type.");
  }
  if(trame_type != -1){ //Checks if it didn't timeout + if it didn't define a trame type
    if(trame_type == 1){ //Checks the header of the trame
      //Serial.println("Checks the header of 1st Trame.");
      res = receive_X_trame(header_1st_Trame, headerLength_1st_Trame);
    }else{
      //Serial.println("Checks the header of 2nd Trame.");
      res = receive_X_trame(header_2nd_Trame, headerLength_2nd_Trame);
    }
    if(res == -1){ //Continues to analyse the trame if there is no header error
      //Serial.println("No header Error.");
      ElementTrame * trame = init_trame(); //Stocks the received element in a linked structure
      if (trame_type == 1){ //Checks the footer of the trame and piles up the rest of the trame
        res = pileup_X_trame_and_analyse_footer(footer_1st_Trame, footerLength_1st_Trame, &trame);
      }else{
        res = pileup_X_trame_and_analyse_footer(footer_2nd_Trame, footerLength_2nd_Trame, &trame);
      }
      if(res == -1){
        //Serial.println("The footer is correct.");
        if (trame_type == 1){
          //Serial.println("Analyse 1st trame");
          res = analyse_1st_trame(&trame);
        }else{
          //Serial.println("Analyse 2nd trame");
          res = analyse_2nd_trame(&trame);
        }
      }else{
        delete_trame(trame);
      }
      //Use to call the analyse_1st_trame() and analyse_2nd_trame() functions. However there was a problem, that the change brought to the Tram declared in this variable wouldn't be changed by the other called functions.
    }
  }else{
    res = 0; // Time out
  }
  Serial.println("");
  received_information_from_Trame(res);
  return res;
}

/* function that receives the trams */

int receive_X_trame(const int * header, const int headerLength) {
  int res = -1;
  int incoming_byte = 0;
  int i = 0;
  //Serial.println("Checks the rest of the start of the Header type.");
  for (i = 1; (i < headerLength && res != 1); i++) {
    incoming_byte = Serial.read();
    print_received_byte(incoming_byte);
    if (header[i] != incoming_byte) {
      res = 1; //means there is a header error
      //Serial.println("Error in header.");
    }
  }
  if (res != 1) {
    //Serial.println("Checks the ID of header");
    for (i = 0; (i < headerID_Length && res != 1); i++) {
      incoming_byte = Serial.read();
      print_received_byte(incoming_byte);
      if (headerID[i] != incoming_byte) {
        res = 1; //means there is a header error
        //Serial.println("Error in header's ID.");
      }
    }
  }
  return res;
}

/* function that stocks the body of the tram and checks the footer for any errors */

int pileup_X_trame_and_analyse_footer(const int * footer, const int footerLength, ElementTrame ** trame){
  //Serial.println("Receives the rest of the Trame and checks for the footer.");
  int res = 2; //If we don't find the good footer, than there is a footer error
  int incoming_byte;
  int i;
  while ((Serial.available() > 0) && (res == 2)) {
    incoming_byte = Serial.read();
    pileup(trame, incoming_byte);
    print_received_byte((*trame)->Byte);
    if (incoming_byte == footer[0]) { //Checks for bytes similar to the start of our footer's type
      //Serial.println("This byte is similar to the start of our footer's starting byte.");
      for (i = 1; (i < footerLength); i++) { //Checks the next 'footerLength' bytes, and compares them to the footer
        incoming_byte = Serial.read();
        pileup(trame, incoming_byte);
        print_received_byte((*trame)->Byte);
        if ((footer[i] == (*trame)->Byte) && (i == (footerLength - 1))) {
          res = -1; //If the footer has 0 errors it changes back to it's '-1' State
          //Serial.print("Received footer with no Error");
        }
      }
    }
  }
  return res;
}

/* function that analyse the 1st trame body */

int analyse_1st_trame(ElementTrame ** trame_reception) {
  ElementTrame * trame_checked = init_trame();
  int i;
  for (i = 0; i < footerLength_1st_Trame; i++) {
    pop(trame_reception);
  } //Removes footer
  int res;
  unsigned int x_MSB, x_LSB;
  int number_of_1_bit_expected = -1; 
  int bit_value = 0;
  int counted_1_bit = 0;
  if((*trame_reception)->next != NULL){
    x_LSB = (*trame_reception)->Byte;
    pop(trame_reception);
    if((*trame_reception)->next != NULL){
      x_MSB = (*trame_reception)->Byte;
      pop(trame_reception);
      number_of_1_bit_expected = bit_shift_combine(x_MSB, x_LSB);
      if((*trame_reception)->next != NULL){
        pop(trame_reception);
        do{
          for (i = 0; i < 8; i++) { //Goes through the 8 bit of 1 byte and adds up the number of counted '1' bit
            bit_value = bitRead((*trame_reception)->Byte, (i));
            if (bit_value == 1) {
              counted_1_bit = counted_1_bit + 1;
            }
          }
          pileup(&trame_checked, (*trame_reception)->Byte);
          pop(trame_reception);
        }while((*trame_reception)->next != NULL && (*trame_reception)->Byte != '*'); //Ends loop when it arrives at the end of the tram (then there is most likely an error) or that the next byte is '*'
        //Serial.print("expected bit: ");
        //Serial.println(counted_1_bit, DEC);
        //Serial.print("Received bit: ");
        //Serial.println(number_of_1_bit_expected, DEC);
      }
    }
  }
  delete_trame(*trame_reception); //Delete the trame of reception which we finished using
  //show_trame_but_delete_it(trame_checked); Checed the we extracted the part we wanted. So in the best case the bytes between '*' and '$'
  if(number_of_1_bit_expected == counted_1_bit){ //If the number of 1 bit concords, then the message is most likely undammaged
    for(i=0;i<3;i++){ //Remove the 2 bytes for 'affichage Text' and 1 byte for the '+'
      if(trame_checked->next != NULL){
        pop(&trame_checked);
      }
    }
    if (trame_checked->Byte == 0xAA) {
      res = 5; // info - Manual
    } else if (trame_checked->Byte == 0xDD) {
      res = 6; // info - Automatic
    } else {
      res = 4; //The information is unrecognised
    }
  } else {
    res = 3; //The Tram's body is damaged
  }
  delete_trame(trame_checked);
  return res;
}

/* function combines two bytes into a decimal number */

int bit_shift_combine( unsigned int x_MSB, unsigned int x_LSB)
{
  int combined;
  combined = x_MSB;              //send x_high to rightmost 8 bits
  combined = combined << 8;      //shift x_high over to leftmost 8 bits
  combined |= x_LSB;             //logical OR keeps x_high intact in combined and fills in                                                             //rightmost 8 bits
  return combined;               //Credits: Matthew - http://projectsfromtech.blogspot.com/2013/09/combine-2-bytes-into-int-on-arduino.html
}

/* function that analyse the 2nd trame body */

int analyse_2nd_trame(ElementTrame ** trame_reception) {
  int i = 0;
  for (i = 0; i < footerLength_1st_Trame; i++) {
    //print_received_byte(trame->Byte); //Used to see what we had in our linked list
    pop(trame_reception);
  } //Removes footer
  int res = -1;
  int info;
  int expected_length = 1;
  int real_length = 0;
  ElementTrame * trame_checked = init_trame();
  //Serial.print("Expected length of the trame_reception's body : ");
  //Serial.println(expected_length);
  //It checks that the trame isn't longer than expected
  while ((*trame_reception)->next != NULL) {
    real_length++;
    print_received_byte((*trame_reception)->Byte);
    pileup(&trame_checked, (*trame_reception)->Byte);
    pop(trame_reception);
  }
  delete_trame(*trame_reception);
  //Serial.print("Actual length : ");
  //Serial.println(real_length);
  if(real_length == expected_length){
    //Serial.println("The body isn't damaged.");
    info = trame_checked->Byte;
    if ((info == '!') || (info == 0x00)) {
      res = 8; //info: Ack
    }else{
      if((info == '?') || (info == 0x3F)){
        res = 7; //info: Nack
      }else{
        res = 4; //The info isn't recognised by our software
      }
    }
  }else{
    //Serial.println("The body is damaged.");
    res = 3; //The Tram's body is damaged
  }
  delete_trame(trame_checked);
  return res;
}

/*--------------------------------------------------------------------------------*/
/*================================================================================*/
/*--------------------------------------------------------------------------------*/

/* -==  Sending a Trame  ==- */

/* Send a trame */

void send_trame(ElementTrame ** trame){
  Serial.print("Sending trame : ");
  while((*trame)->next != NULL){
    Serial.print((*trame)->Byte, HEX);//This is the only usefull serial print !!
    //Serial.print(" "); //Will need to remove. It's to see the trame more clearly
    pop(trame);
  }
  Serial.print(" - ");
  delete_trame(*trame);
  return;
}

/* function add's X tab to the trame */

void add_X_tab_to_trame(const int * x, const int xLength, ElementTrame ** trame){
  int i;
  for(i=0;i<xLength;i++){
    pileup(trame,x[xLength-i-1]);
  }
  return;
}

void add_X_tab_unsigned_to_trame(unsigned int * x, const int xLength, ElementTrame ** trame){
  int i;
  for(i=0;i<xLength;i++){
    pileup(trame,x[xLength-i-1]);
  }
  return;
}

/* function sends 2nd trame type, with one choosen byte (int message - ! or ?) */

void send_2nd_trame(int message){
  ElementTrame * trame = init_trame();
  add_X_tab_to_trame(footer_2nd_Trame, footerLength_2nd_Trame, &trame);
  pileup(&trame,message);
  add_X_tab_to_trame(headerID,headerID_Length,&trame);
  add_X_tab_to_trame(header_2nd_Trame, headerLength_2nd_Trame, &trame);
  send_trame(&trame);
  if(message == '!'){
    Serial.println("Ack");
  }else if(message == '!'){
    Serial.println("Nack");
  }else{
    Serial.println("UNKNOWN");
  }
}

/* function sends 1st trame type, start message */

void send_start(){
  ElementTrame * trame = init_trame();
  add_X_tab_to_trame(footer_1st_Trame, footerLength_1st_Trame, &trame);
  body_1st_Trame[1] = 0x7F;
  body_1st_Trame[2] = 0x7F;
  dec_shift_to_2_bytes(&body_1st_Trame[9], &body_1st_Trame[10], number_of_1_bit());
  add_X_tab_unsigned_to_trame(body_1st_Trame, bodyLength_1st_Trame, &trame);
  add_X_tab_to_trame(headerID,headerID_Length,&trame);
  add_X_tab_to_trame(header_1st_Trame, headerLength_1st_Trame, &trame);
  send_trame(&trame);
  Serial.println("Start");
}

/* function sends 1st trame type, lux value */

void send_lux(int lux){
  ElementTrame * trame = init_trame();
  add_X_tab_to_trame(footer_1st_Trame, footerLength_1st_Trame, &trame);
  body_1st_Trame[1] = 0xFA;
  body_1st_Trame[2] = 0xFA;
  dec_shift_to_2_bytes(&body_1st_Trame[6], &body_1st_Trame[7], lux);
  dec_shift_to_2_bytes(&body_1st_Trame[9], &body_1st_Trame[10], number_of_1_bit());
  add_X_tab_unsigned_to_trame(body_1st_Trame, bodyLength_1st_Trame, &trame);
  add_X_tab_to_trame(headerID,headerID_Length,&trame);
  add_X_tab_to_trame(header_1st_Trame, headerLength_1st_Trame, &trame);
  send_trame(&trame);
  Serial.println("Lux");
}

/* function that will tell the number of 1 bit in the bytes between * and $ */

int number_of_1_bit(){
  int counted_1_bit = 0;
  int i,j;
  for(i=1;i<bodyLength_1st_Trame-3;i++){ //Goes through the byte between '*' and '$' of the 1st trame type
    for (j = 0; j < 8; j++) { //Goes through the 8 bit of 1 byte and adds up the number of counted '1' bit
      if ((bitRead(body_1st_Trame[i], j)) == 1) {
        counted_1_bit = counted_1_bit + 1;
      }
    }
  }
  //Serial.print("counted_1_bit: "); //Used for debugging
  //Serial.println(counted_1_bit, DEC);
  return counted_1_bit;
}

/* function that returns 2 bytes  */

void dec_shift_to_2_bytes(unsigned int * x_MSB, unsigned int * x_LSB, int dec)
{
  *x_LSB = dec & 0xff;
  *x_MSB = dec >> 8;
}

/*--------------------------------------------------------------------------------*/
/*================================================================================*/
/*--------------------------------------------------------------------------------*/

/* -==  Getting Lux Value  ==- */

/* function gets lux value */

int get_lux(){
  int sensorVal; // Analog value from the sensor
  int lux; //Lux value
  sensorVal = analogRead(LDR);
  lux = sensorRawToPhys(sensorVal);
  return lux;
}

int sensorRawToPhys(int raw){ /* Credit - https://www.aranacorp.com/fr/mesure-de-luminosite-avec-une-photoresistance/ */
  // Conversion rule
  float Vout = float(raw) * (VIN / float(1024));// Conversion analog to voltage
  float RLDR = (R * (VIN - Vout))/Vout; // Conversion voltage to resistance
  int phys=500/(RLDR/1000); // Conversion resitance to lumen
  return phys;
}

/*--------------------------------------------------------------------------------*/
/*================================================================================*/
/*--------------------------------------------------------------------------------*/

/* -==  functions for debugging  ==- */

/*--------------------------------------------------------------------------------*/

/* function to blink */

void bink_Yas() {
  digitalWrite(LedRG, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LedRG, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);
  return;
}

/* function to check memory usage - credit : Mikael Patel */

void freeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  Serial.print("Free memory is: ");
  Serial.println((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
  return;
}

/* function which shows the content of a trame but delete's it at the same time */

void show_trame_but_delete_it(ElementTrame * top_trame) { //For debugging. Managed to see that Arduino couldn't detect the NULL pointer without it's library. Or that a tram changed states from one function to another (never figured why, I just went around the problem)
  Serial.print("The Tram:");
  while(top_trame->next != NULL){ //Erases the trame_checked
    print_received_byte(top_trame->Byte);
    pop(&top_trame);
  }
  pop(&top_trame);
  Serial.print(" done");
}

/* functions that communicates the type of info it received | It's only informative */

void received_information_from_Trame(int Information) {
  // 0: Time out / 1: Header error / 2: Footer error / 3: Error in the received Trame / 4: The info isn't recognised by our software
  // 5: info - Manual / 6: info - Automatic / 7: info - Nack / 8: info - Ack
  Serial.print("Received Trame information: ");
  if (Information == 0) {
    Serial.print("Time out");
  } else {
    if (Information == 1) {
      Serial.print("Header error");
    } else {
      if (Information == 2) {
        Serial.print("Footer error");
      } else {
        if (Information == 3) {
          Serial.print("Error in the received Trame");
        } else {
          if (Information == 4) {
            Serial.print("The info isn't recognised by our software");
          } else {
            if (Information == 5) {
              Serial.print("Manual");
            } else {
              if (Information == 6) {
                Serial.print("Automatic");
              } else {
                if (Information == 7) {
                  Serial.print("Nack");
                } else {
                  if (Information == 8) {
                    Serial.print("Ack");
                  } else {
                    Serial.print("UNKNOWN");
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  Serial.println(" ");
  return;
}

/*--------------------------------------------------------------------------------*/
/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*
    2 type of trams:
      1st type:

          Start: %&DDAP*[0x7F7F]+0+00$[Somme des bits a 1 des champs '[0x7F7F]+0+00' donc 28 ou 0x007F]@:@
          Manuel: %&DDAP*[0xFAFA]+[0xAA]+00$[Somme des bits a 1 des champs du corps]@:@
          %&DDAP*FF+A+FF$FF@:@
          Automatique: %&DDAP*[0xFAFA]+[0xDD]+00$[Somme des bits a 1 des champs du corps]@:@
          %&DDAP*FF+D+FF$FF@:@
          Lux: %&DDAP*[0xFAFA]+0+[Lux Value]$[Somme des bits a 1 des champs du corps]@:@

      2nd type:
          Ack:  &%DDAP!:@:
          Nack: &%DDAP?:@:

          When getting information you might get:
      
      // 0: Time out / 1: Header error / 2: Footer error / 3: Error in the received Trame / 4: The info isn't recognised by our software
      // 5: info - Manual / 6: info - Automatic / 7: info - Nack / 8: info - Ack
      
*/
/*--------------------------------------------------------------------------------*/
/*================================================================================*/
/*--------------------------------------------------------------------------------*/

/* -==  Setup  ==- */

void setup() {
  //setup code, runs once :
  Serial.begin(115200);
  pinMode(LedRG, OUTPUT);
  freeRAM();
  Serial.println("Starting Software :");
  //Wait 10s, so that the value on the LDR stabilizes and has the led blink
  int i;
  digitalWrite(LedRG,LOW); //The Red led is on and the Green led is off
  delay(2000);
  for(i=0;i<2;i++){
    digitalWrite(LedRG,HIGH); //The Green led is on and the Red led is off
    delay(2000);
    digitalWrite(LedRG,LOW); //The Red led is on and the Green led is off
    delay(2000);
  }
  information_received_global = 0;
}

void loop() {
  while((information_received_global == 0) || (information_received_global == 1)){ //Wait's to receive a Tram (if it times out or it's a header error we are most likely receiving nothing.)
    information_received_global = receive_trame_info();
  }
  if(information_received_global == 6){ //We're in Automatic mode
    digitalWrite(LedRG,HIGH); //The Green led is on and the Red led is off
    do{
      send_start();
      information_received_global = receive_trame_info();
    }while((information_received_global != 8) && (information_received_global != 5) && (information_received_global != 6)); //repeats until it receives the Ack information or is getting the information about a mode
    if(information_received_global == 8){
      do{
        send_lux(get_lux());
      }while((information_received_global != 8) && (information_received_global != 5) && (information_received_global != 6)); //repeats until it receives the Ack information or is getting the information about a mode
      if(information_received_global == 8){
        int k = 0;
        do{
          k = k + 1;
          information_received_global = receive_trame_info();
          if((information_received_global < 5) && (information_received_global > 1)){//Received damaged tram
            send_2nd_trame('?'); // 2nd tram: Nack
          }
          delay(500);
        }while((k < 30) && (information_received_global != 5)); //Waits around 15 seconds to exit or that we switch to manuel
        if(information_received_global != 5){
          information_received_global = 6; //Stay in Manual
        }
      }
    }
    
  }else if(information_received_global == 5){ //We're in Manual mode
    digitalWrite(LedRG,LOW);//The Red led is on and the Green led is off
    send_2nd_trame('!'); // 2nd tram: Ack
    information_received_global = 0;
  }else{ //We received a damaged Tram or not what we expected
    send_2nd_trame('?'); // 2nd tram: Nack
    information_received_global = 0;
  }
}
