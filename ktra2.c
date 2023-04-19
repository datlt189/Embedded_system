#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

int mpu;

void mpu6050_init(void) {
    //set up I2C interface
	mpu = wiringPiI2CSetup(0x68);
    // check I2C conect
	if(wiringPiI2CReadReg8(mpu, 0x75) != 0x68){
		printf("Conect false! \n");
		exit(1);
	}

    // sample rate (tan so lay mau): 100Hz
    // DVPF: On -> 0x09 (do khi DLPF On thi no se len 1KHz, nhung ta chi muon 500Hz nen ta dat no = 4)
    wiringPiI2CWriteReg8(mpu, 0x19, 0x04);
    // DLPF: fc = 44Hz -> 0x03
    wiringPiI2CWriteReg8(mpu, 0x1A, 0x03);
    // gyro: FS = +-250 -> 0x08
    wiringPiI2CWriteReg8(mpu, 0x1B, 0x00);
    // accel: FS = +-4g -> 0x08
    wiringPiI2CWriteReg8(mpu, 0x1C, 0x08);
    // interrupt enable -> 0x01
    wiringPiI2CWriteReg8(mpu, 0x38, 1);
    // clock source: gyroX -> 0x01
    wiringPiI2CWriteReg8(mpu, 0x6B, 1);
}

//Ham send data
void max7219_write(unsigned char reg, unsigned char data) {
    unsigned char buffer[2];
    buffer[0] = reg;
    buffer[1] = data;
    wiringPiSPIDataRW(0, buffer, 2);
}

// cau hinh che do hoat dong max7219
void max7219_init(void) {
    wiringPiSPISetup(0, 10000000); // 2 thong so trong cau lenh da dc define trong file .h
    max7219_write(0x0B, 0x07); // xác định số lượng thanh ghi số sẽ được sử dụng để hiển thị: 8
    max7219_write(0x09, 0x00); // cấu hình chế độ giải mã: none
    max7219_write(0x0F, 0x00); // để tắt chế độ kiểm tra hiển thị
    max7219_write(0x0C, 0x01); // bật MAX7219
    max7219_write(0x0A, 0x09); // độ sáng của LED. 0 (tối nhất) đến 15 (sáng nhất).
}

//read sensor
int16_t readSensor(unsigned char sensor) {
    // Tạo biến chứa giá trị byte cao và byte thấp của thanh ghi 16 bit
    uint8_t highByte, lowByte;
    // Đọc byte cao và byte thấp của thanh ghi tương ứng với địa chỉ sensor
    highByte = wiringPiI2CReadReg8(mpu, sensor);
    lowByte = wiringPiI2CReadReg8(mpu, sensor + 1);
    // Ghép byte cao và byte thấp thành giá trị 16 bit và trả về giá trị đó
    return (int16_t)((highByte << 8) | lowByte);
}

void getRoll(float* roll) {
    // đọc giá trị từ các sensor
    float Ax = (float)readSensor(59)/4096.0;
    float Ay = (float)readSensor(61)/4096.0;
    float Az = (float)readSensor(63)/4096.0;
    
    // tính góc nghiêng
    // *pitch = atan2(Ax, sqrt(pow(Ay,2) + pow(Az,2))) * 180 / M_PI;
    *roll = atan2(Ay, sqrt(pow(Ax,2) + pow(Az,2))) * 180 / M_PI;
}
// Khai báo mảng chứa mã hex của số từ 0 đến 9
unsigned char digits[10] = {
  0x7E, // Mã hex của số 0
  0x30, // Mã hex của số 1
  0x6D, // Mã hex của số 2
  0x79, // Mã hex của số 3
  0x33, // Mã hex của số 4
  0x5B, // Mã hex của số 5
  0x5F, // Mã hex của số 6
  0x70, // Mã hex của số 7
  0x7F, // Mã hex của số 8
  0x7B, // Mã hex của số 9
};
// Hàm chuyển đổi số nguyên thành mã hex để hiển thị trên led
void chuyendoi(uint8_t num) {
    uint8_t donvi = num % 10;
    uint8_t chuc = num / 10;
    unsigned char machuc = digits[chuc];
    unsigned char madonvi = digits[donvi];
    // Sử dụng hàm max7219_write để hiển thị trên thanh ghi 5 và 6
    max7219_write(5, madonvi);
    max7219_write(6, machuc);
}

// khai bao mang hien thi OPN
unsigned char open[3] = {
    0x76, // chu N
    0x67, // chu P
    0x7E, // chu O  
};
// khai bao mang hien thi CLS
unsigned char close[3] = {
    0x5B, // chu S
    0x0E, // chu L
    0x4E, // chu C  
};

// tinh toan van toc
float DT = 100; //delay cung nhu thoi gian giua 2 lan lay mau accel de tinh van toc theo cong thuc v = a*delta(t)
float vel(float accel){
    float v = accel*DT*0.001;
    return v;
}

int main(void){
    // set operational mode for max7219
    max7219_init();
    // set operational mode for mpu6050
    mpu6050_init();

    float roll; // de dung getRoll()

    int ledDisplayCount = 0; //bien dem de hien thi chop tat. chi hien thi khi bien nay mang so chan

    unsigned long previousMillis = 0;   // thời điểm trước đó
    const long interval = 3000;          // khoảng thời gian nhấp nháy của LED (miligiây)
    int ledState = 0;                // trạng thái của LED

    while(1){
        getRoll(&roll); // lay gia tri roll
        roll = fabs(roll); // lay gia tri duong cua roll
        uint8_t roll1 = (uint8_t)roll; // dung bien roll1 luu gia tri ep kieu cua roll de hien thi
        float AccZ = (float)readSensor(63)/8192.0*9.81; // tinh toan gia toc truc z don vi m/s^2 khi dung yen AccZ xap xi 9.81m/s^2

        // tat cac led hien thi truoc do
        for (int i = 1; i <= 8; i++){
            max7219_write(i, 0x00);
        }

        if (AccZ < 9.5 || AccZ > 10.5) max7219_write(8, 0x3D); // neu ngoai khoang gia tri nay tuc xe dang chuyen dong send d
        else max7219_write(8, 0x67); // nguoc lai send P

        max7219_write(7, 0x01); // viet dau gach ngang tai led so 7

        chuyendoi(roll1); //goi ham chuyen doi cho gia tri roll1 da xu ly truoc do de hien thi len led 7 doan so 5 va 6

        max7219_write(4, 0x01); // viet dau gach ngang tai led so 4

        float v = fabs(vel(AccZ)*3.6); // tinh toan van toc dua theo gia toc truc z

        // ktra neu roll1 nho hon 10 tuc cop xe dong send mang close da khai bao ben tren
        if (roll1 <= 10) for (int i = 0; i<3; i++) max7219_write(i+1, close[i]); 
        // // ktra neu v > 10 va roll1 lon hon 10 thi cho led hien thi mang open chop tat lien tuc de canh bao 
        else if (v>10.0 && roll1 > 10){
            if (ledDisplayCount % 2 == 0) {
                for (int i = 0; i<3; i++) max7219_write(i+1, open[i]);
            }
            ledDisplayCount++; // Tăng giá trị biến đếm sau mỗi lần lặp
        } 
        // truong hop con lai cop xe mo send mang open da khai bao ben tren
        else for (int i = 0; i<3; i++) max7219_write(i+1, open[i]);

        
        //hien thi len terminal de check
        printf("AccZ: %.2f %.2f\n", AccZ, v);
        delay(DT);    
    }
    return 0;
}
