#define LEFT_SWITCH 30                                                  //End line switch on the Left
#define RIGHT_SWITCH 9                                                  //End line switch on the right
#define CHA_CART 4                                                      //Cart's Encoder Channel A
#define CHB_CART 3                                                      //Cart's Encoder Channel B
#define CHA_PEN 37                                                      //Pendulum's Encoder Channel A
#define CHB_PEN 36                                                      //Pendulum's Encoder Channel B
#define LEFT 6                                                          //motor rotates anticlockwise - CART to the Left
#define RIGHT 7                                                         //motor rotates clockwise - CART to the Right

//.............................................................................................................................//
int LEFT_SWITCH_state;                                                  //Variable used for indicating if LEFT_SWITCH is ON or OFF (1 ou 0)
int RIGHT_SWITCH_state;                                                 //Variable used for indicating if RIGHT_SWITCH is ON or OFF (1 ou 0)
int valPWM;                                                             //Variable witch alternates between 0 and 255
//.............................................................................................................................//
volatile int masterCountCart = 0;                                       //motor encoder counter (reseting)
volatile int cartSignal, oldCartSignal;                                 //Variables used for checking Cart's position. Necessary for 'masterCountCart'
int cartCurrentPosition;                                                //Cart's current position
float cartCurrentVelocity;                                              //Cart's current velocity
float cartVelocity(void);                                               //'getCartVelocity()' returns the current linear velocity of the Cart [pulse/miliseconds]
//.............................................................................................................................//
volatile int masterCountPen = 0;                                        //contador encoder cart
volatile int penSignal, oldPenSignal;                                   //Variables used for checking Pendulum's position. Necessary for 'masterCountPen'
float getPendulumVelocity(void);                                        //'getPendulumVelocity()' returns the current angular velocity of the Pendulum [cicles/miliseconds]
//.............................................................................................................................//
long int previousTime = 0;                                              //Variable used calculating time
long int currentTime;                                                   //Variable used calculating 
long int deltaTime;                                                     //Variable used calculating
//.............................................................................................................................// 
float errorValue;                                                       //Variable witch contains the current error of control
float previousErrorValue = 0;                                           //Variable witch contains the last error of control
float deltaError = 0;                                                   //Variable used for comparing the last with the current error
float roundedValue;                                                     //Variable used for containing a calculated value rounded 
//.............................................................................................................................// 
float Kp = 1.2;                                                         //Proporcionality constant
float Ki = 1;                                                           //integration constant
float Kd = 0;                                                           //Derivative constant
//float proporcional(void);                                             //Function used for propotional control
//float integration(void);                                              //Function used for integration control
//float derivation(void);                                               //Function used for derivation control
int Prop = 0; 
float Int = 0;
float Der = 0; 
long int timeForIntagrating;                                            //Used in Integration control
long int TMP_INT = 50;                                                  //Used in Integration control
long int timeForDerivating;                                             //Used in Derivation control
long int TMP_DER = 50;                                                  //Used in Derivation control
//.............................................................................................................................//

void setup(){
  pinMode(CHA_PEN, INPUT);                                              //Pendulum's Encoder Channel A used for 3.3V input
  pinMode(CHB_PEN, INPUT);                                              //Pendulum's Encoder Channel B used for 3.3V input
  pinMode(LEFT_SWITCH, INPUT);                                          //End line switch used for 3.3V input
  pinMode(RIGHT_SWITCH, INPUT);                                         //End line switch 
  pinMode(CHA_CART, INPUT);                                             //Cart's Encoder Channel A used for 3.3V input
  pinMode(CHB_CART, INPUT);                                             //Cart's Encoder Channel B used for 3.3V input
  
  pinMode(RIGHT, OUTPUT);                                               //Output pin used for rotates the motor anticlockwise - CART to the Left
  pinMode(LEFT, OUTPUT);                                                //Output pin used for rotates the motor anticlockwise - CART to the Right
  
  Serial.begin(115200);                                                 //Serial Begining
  Serial.println("Sistema Inicializado \n");                            //Serial message display
  Serial.println("Entre com a velocidade desejada: ");                  //Serial message display
  
  RIGHT_SWITCH_state = digitalRead(RIGHT_SWITCH);                       //Checking current state of RIGHT_SWITCH
  LEFT_SWITCH_state = digitalRead(LEFT_SWITCH);                         //Checking current state of LEFT_SWITCH

  attachInterrupt(CHA_CART,calculatingCartPosition, CHANGE);            //Calculating CH-A cart's position
  attachInterrupt(CHB_CART,calculatingCartPosition, CHANGE);            //Calculating CH-B cart's position
  
  masterCountCart = 0;                                                  //Counter reset
  left_calibration();                                                   //It makes the operation to iniciate with Cart on the initial position (to the left, near by LEFT_SWITCH)
  currentTime = millis();                                               //It initiates the timer
  timeForIntagrating = currentTime + TMP_INT;
  timeForDerivating = currentTime + TMP_DER;
}
//.............................................................................................................................//

void loop() 
{ 
    movingCart_PIDcontrol(2, 5350);                                     // movingCart_PIDcontrol(cartVelocity_reference [pulses/ms], desiredPosition [pulses]). This function uses PID control to moving cart along rail
}
//.............................................................................................................................//

void calculatingCartPosition() 
{
  oldCartSignal = cartSignal;                                           //Variable atualization
  cartSignal = (digitalRead(CHA_CART)*2) + digitalRead(CHB_CART);       //Variable atualization
  
  if((oldCartSignal == 0) && (cartSignal == 1))
    masterCountCart-- ;                                                 // decreases -1 on anticlockwise counting 
  if((oldCartSignal == 0) && (cartSignal == 2))
    masterCountCart++ ;                                                 // increases 1 on clockwise counting  

  if((oldCartSignal == 1) && (cartSignal == 3))
    masterCountCart-- ;                                                 // decreases -1 on anticlockwise counting 
  if((oldCartSignal == 1) && (cartSignal == 0))
    masterCountCart++ ;                                                 // increases 1 on clockwise counting  

  if((oldCartSignal == 2) && (cartSignal == 0))
    masterCountCart-- ;                                                 // decreases -1 on anticlockwise counting 
  if((oldCartSignal == 2) && (cartSignal == 3))
    masterCountCart++ ;                                                 // increases 1 on clockwise counting  

  if((oldCartSignal == 3) && (cartSignal == 2))
    masterCountCart-- ;                                                 // decreases -1 on anticlockwise counting 
  if((oldCartSignal == 3) && (cartSignal == 1))
    masterCountCart++ ;                                                 // increases 1 on clockwise counting  
}
//.............................................................................................................................//
void calculatingPendulumPosition() 
{
  oldPenSignal = penSignal;                                             //Variable atualization
  penSignal = (digitalRead(CHA_PEN)*2) + digitalRead(CHB_PEN);          //Variable atualization
  
  if(masterCountPen > 2047) masterCountPen -= 2048;                     //It resests Pendulum position after clockwise cycle
  if(masterCountPen < -2047) masterCountPen += 2048;                    //It resests Pendulum position after anticlockwise cycle
        
  if((oldPenSignal == 0) && (penSignal == 1))
    masterCountPen-- ;                                                  // decreases -1 on anticlockwise counting 
  if((oldPenSignal == 0) && (penSignal == 2))
    masterCountPen++ ;                                                  // increases 1 on clockwise counting 

  if((oldPenSignal == 1) && (penSignal == 3))
    masterCountPen-- ;                                                  // decreases -1 on anticlockwise counting 
  if((oldPenSignal == 1) && (penSignal == 0))
    masterCountPen++ ;                                                  // increases 1 on clockwise counting     

  if((oldPenSignal == 2) && (penSignal == 0))
    masterCountPen-- ;                                                  // decreases -1 on anticlockwise counting 
  if((oldPenSignal == 2) && (penSignal == 3))
    masterCountPen++ ;                                                  // increases 1 on clockwise counting  

  if((oldPenSignal == 3) && (penSignal == 2))
    masterCountPen-- ;                                                  // decreases -1 on anticlockwise counting 
  if((oldPenSignal == 3) && (penSignal == 1))
    masterCountPen++ ;                                                  // increases 1 on clockwise counting 
}
//.............................................................................................................................//
void left_calibration()
{
  LEFT_SWITCH_state = digitalRead(LEFT_SWITCH);                         //Atualizating current state of LEFT_SWITCH
  while (LEFT_SWITCH_state != 1)                                        //checking condition
  {
    delay(1);                                                           //Minimum delay necessary
    analogWrite(RIGHT,0);
    analogWrite(LEFT,41);
    //valPWM = -41;
    //motor(valPWM);                                                    //Moving Cart to the left 
    LEFT_SWITCH_state = digitalRead(LEFT_SWITCH);                       //Atualizating current state of LEFT_SWITCH
  }                                           
  cartCurrentPosition = masterCountCart;                                //Atualizates Cart current position
  valPWM = 10;                                
  
  do{                                                                   //"do-while" - The sequency of commands wiil happen at least once and it will repeat while LEFT_SWITCH_state == 1
    motor(valPWM);                                                      //Moving Cart to the right
    if(cartCurrentPosition == masterCountCart)                          //If there was no change in cart position, the PWM value will be increased
    valPWM++;                                                           //PWM value is increased 
    else                                                                //If there was change in cart position
    cartCurrentPosition = masterCountCart;                              //variable atualization
    delay(10);                                                          //delay time
    LEFT_SWITCH_state = digitalRead(LEFT_SWITCH);                       //Atualizating current state of LEFT_SWITCH
    }
    while(LEFT_SWITCH_state);                                           //End of 'do-while'
    
    analogWrite(RIGHT,200);                                             //Braking motor
    analogWrite(LEFT,200);                                              //Breaking motor
    delay(1000);                                                        //One second of delay time
    motor(0);                                                           //motor traction freedom
    masterCountCart = 0;                                                //motor encoder counter (reseting)
}
//.............................................................................................................................//
float getPendulumVelocity()                                             //Function used for calculating pendulum's angular velocity
{
  int initialPosition, finalPosition;                                   //Variables used for calculating velocity
  float velocity;                                                       //Variable used for calculating velocity
  initialPosition = masterCountPen;
  delay(10);
  finalPosition = masterCountPen;
  velocity = (float)((finalPosition - initialPosition)/ 10.0);          //Calculating velocity
  return(velocity);                                                     //The function returns the calculated velocity
}
//.............................................................................................................................//
float getCartVelocity()                                                 //Function used for calculating cart's linear velocity
{
  int initialPosition, finalPosition;                                   //Variables used for calculating velocity
  float velocity;                                                       //Variable used for calculating velocity
  initialPosition = masterCountCart;                                    //Variable atualization                          
  delay(10);                                                            //Delay time 
  finalPosition = masterCountCart;                                      //Variable atualization 
  velocity = (float)((finalPosition - initialPosition)/ 10.0);          //Calculating velocity
  return(velocity);                                                     //The function returns the calculated velocity
}
//.............................................................................................................................//
void motor(int vel)                                                      //Function used for actuating motor. It makes cart to moviment
{
  if(vel > 255) vel = 255;                                               //Seting limits
  if(vel < -255) vel = -255;                                             //Seting limits
  if(vel > 0)
  {
    analogWrite(RIGHT,abs(vel));                                         //Cart moves to the right
    analogWrite(LEFT,0);                                                 //Cart do not move to the left
  }
  else
  {
    analogWrite(LEFT,abs(vel));;                                         //Cart moves to the left
    analogWrite(RIGHT,0);                                                //Cart do not move to the right
  }
}
//.............................................................................................................................//
void proporcional()                                                      //Function used for propotional control
{
  roundedValue = round(Kp * errorValue);                                 //Rounding proportional value calculated according to error
  Prop = (int) roundedValue;                                             //Storing calculated value
}
//.............................................................................................................................//
void integration( )                                                      //Function used for integration control
{   
  currentTime = millis();                                                //Storing current time
 
  if (currentTime >= timeForIntagrating)                                 //Checking condition
  {
    Int = (int) errorValue*Ki + Int;                                     //Calculating integration controal value
    if(Int > 255) Int = 255;                                             //Setting limits
    if(Int < 0) Int = 0;                                                 //Setting limits
    currentTime = millis();                                              //Storing current time
    timeForIntagrating = currentTime + TMP_INT;                          //Variable atualization
  }  
}
//.............................................................................................................................//
void derivation( )                                                       //Function used for derivation control
{ 
  currentTime = millis();                                                //Storing current time
 
  if (currentTime >= timeForDerivating)                                  //Checking condition
  {
    deltaError = errorValue - previousErrorValue;                        //Calculating deltaError used in derivation calculus
    deltaTime = currentTime - previousTime;                              //Calculating deltaTime used in derivation calculus
    Der = (int) ((deltaError*Kd)/(deltaTime));                           //Calculating derivation control value
    previousErrorValue = errorValue;                                     //Variable atualization
    previousTime = currentTime;                                          //Variable atualization
    if(Der > 255) Der  = 255;                                            //Setting limits
    if(Der < 0) Der  = 0;                                                //Setting limits
    currentTime = millis();                                              //Storing current time
    timeForDerivating = currentTime + TMP_DER;                           //Variable atualization
  }  
}
//.............................................................................................................................//
void movingCart_PIDcontrol (int cartVelocity_reference, int desiredPosition)  //Function used for moving cart along rail. It used PID control system.
                                                                              //For using the function, the operator needs to provide cart's desired linear velocity and desired final rail position 
{
  float finalCartVelocity;
  while(masterCountCart < (desiredPosition - ((desiredPosition/100)*cartVelocity_reference)))
  {
    cartCurrentVelocity = getCartVelocity();                                  //Variable atualization      
    errorValue = cartVelocity_reference - cartCurrentVelocity;                //Error variable atualization   
    proporcional();                                                           //Calculating proportional 
    integration();                                                            //Calculation integration
    derivation();                                                             //Calculating derivation 
    valPWM = Prop + Int + Der;
    motor(valPWM);
    
    Serial.print(" VelRef: ");Serial.print(cartVelocity_reference);           //It displays the choosen reference velocity 
    Serial.print(" VelAtual: ");Serial.print(cartCurrentVelocity);            //It displays the current velocity 
    Serial.print(" Erro: ");Serial.println(errorValue);                       //It displays the current error value
    Serial.print(" Posição Cart: ");Serial.println(masterCountCart); 
  }
    while(masterCountCart > (desiredPosition + ((desiredPosition/100)*cartVelocity_reference)))
  {
    cartCurrentVelocity = getCartVelocity();                                  //Variable atualization      
    errorValue = (cartVelocity_reference/2) - cartCurrentVelocity;            //Error variable atualization   
    proporcional();                                                           //Calculating proportional 
    integration();                                                            //Calculation integration
    derivation();                                                             //Calculating derivation 
    valPWM = Prop + Int + Der;
    valPWM = valPWM*(-1);
    motor(valPWM);
    
    Serial.print(" VelRef: ");Serial.print(cartVelocity_reference);           //It displays the choosen reference velocity 
    Serial.print(" VelAtual: ");Serial.print(cartCurrentVelocity);            //It displays the current velocity 
    Serial.print(" Erro: ");Serial.print(errorValue);                         //It displays the current error value
    Serial.print(" Posicao Cart: ");Serial.println(masterCountCart); 
  }
  
  finalCartVelocity = getCartVelocity();                                      //Variable atualization
  analogWrite(RIGHT,200);                                                     //Braking motor
  analogWrite(LEFT,200);                                                      //Breaking motor
  delay(1000);                                                                //One second of delay time
  motor(0);                                                                   //motor traction freedom
  Serial.print("Posição de chegada: ");Serial.println(masterCountCart);       //It displays the position
  Serial.print("Velocidade atingida: ");Serial.println(finalCartVelocity);    //It displays the linear velocity reached
  delay(5);                                                                   //Delay time
}
//.............................................................................................................................//

