#include <Servo.h>

// Title:   Đồ Án 2 - Hệ Thống Camera Xoay Theo Chuyển Động
// Subject: Điều khiển hai động cơ servo để đưa hình ảnh vào trung tâm của màn hình.
// Date:    2016


#define  servomaxx   180   // Thiết lập Servo ngang (x) có thể quay được tối đa là 180 độ.
#define  servomaxy   180   // Thiết lập Servo dọc (y) có thể quay được tối đa là 180 độ.
#define  screenmaxx   320   // Độ phân giải chiều rộng của màn hình là 320
#define  screenmaxy   240    // Độ phân giải chiều dọc của màn hình là 240
#define  servocenterx   90  // Thiết lập vị trị ban đầu của Servo X là 90
#define  servocentery   90  // Thiết lập vị trị ban đầu của Servo Y là 90
#define  servopinx   9   // Thiết lập chân điều khiển cho Servo X trong arduino là 9
#define  servopiny   10  // Thiết lập chân điều khiển cho Servo Y trong arduino là 10
#define  baudrate 57600  // Tốc độ của cổng com. Phải thiết lập trùng với setting bên C++.
#define distancex 1  // Bước xoay của Servo X
#define distancey 1  // Bước xoay của Servo Y

int valx = 0;       // store x data from serial port
int valy = 0;       // store y data from serial port
int posx = 0;
int posy = 0;
int incx = 10;  // significant increments of horizontal (x) camera movement
int incy = 10;  // significant increments of vertical (y) camera movement

Servo servox;
Servo servoy;

short MSB = 0;  // to build  2 byte integer from serial in byte
short LSB = 0;  // to build  2 byte integer from serial in byte
int   MSBLSB = 0;  //to build  2 byte integer from serial in byte

void setup() {

  Serial.begin(baudrate);        // Mở cổng giao tiếp Serial tại baudrate = 57600
  Serial.println("Starting Cam-servo Face tracker");

  pinMode(servopinx,OUTPUT);    // Thông báo đèn cho servo x
  pinMode(servopiny,OUTPUT);    // Thông báo đèn cho servo y

  servoy.attach(servopiny); //Cài đặt chức năng điều khiển cho chân servopiny(10)
  servox.attach(servopinx); //Cài đặt chức năng điều khiển cho chân servopinx(9)

  // center servos

  servox.write(servocenterx); // Cho servo quay một góc là Servocenterx (90 độ)
  delay(200);
  servoy.write(servocentery); // Cho servo quay một góc là Servocentery (90 độ)
  delay(200);
}

void loop () {
  while(Serial.available() <=0); // wait for incoming serial data
  if (Serial.available() >= 4)  // wait for 4 bytes. 
  {
    // get X axis 2-byte integer from serial
    // Lấy 2 byte số nguyên của trục x từ serial
    MSB = Serial.read();
    delay(5);
    LSB = Serial.read();
    MSBLSB=word(MSB, LSB);
    valx = MSBLSB; 
    delay(5);

    // get Y axis 2-byte integer from serial
    MSB = Serial.read();
    delay(5);
    LSB = Serial.read();
    MSBLSB=word(MSB, LSB);
    valy = MSBLSB; 
    delay(5);

    // read last servos positions
    posx = servox.read(); 
    posy = servoy.read();

    //Find out if the X component of the face is to the left of the middle of the screen.
    if(valx < (screenmaxx/2 - incx)){
      if( posx >= incx ) posx += distancex; //Update the pan position variable to move the servo to the left.
    }
    //Find out if the X component of the face is to the right of the middle of the screen.
    else if(valx > screenmaxx/2 + incx){
      if(posx <= servomaxx-incx) posx -=distancex; //Update the pan position variable to move the servo to the right.
    }

    //Find out if the Y component of the face is below the middle of the screen.
    if(valy < (screenmaxy/2 - incy)){
      if(posy >= 5)posy += distancey; //If it is below the middle of the screen, update the tilt position variable to lower the tilt servo.
    }
    //Find out if the Y component of the face is above the middle of the screen.
    else if(valy > (screenmaxy/2 + incy)){
      if(posy <= 175)posy -= distancey; //Update the tilt position variable to raise the tilt servo.
    }

    // Servos sẽ xoay cho phù hợp
    servox.write(posx);
    servoy.write(posy);

  }   
}
















