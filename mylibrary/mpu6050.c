#include <wiringPiI2C.h>
#include <stdlib.h> // exit()
#include <math.h> // atan.. (gcc...-lwringPi -lm)
#include <stdint.h> // int16_t..
#include <stdio.h> // print("Conect false! \n");
#include "mpu6050.h"

int mpu;

void initMpu(void) {
    //set up I2C interface
	mpu = wiringPiI2CSetup(0x68);
    // check I2C conect
	if(wiringPiI2CReadReg8(mpu, 0x75) != 0x68){
		printf("Conect false! \n");
		exit(1);
	}

    // sample rate (tan so lay mau): 100Hz
    // DVPF: On -> 0x09 (do khi DLPF On thi no se len 1KHz, nhung ta chi muon 100Hz nen ta dat no = 9)
    wiringPiI2CWriteReg8(mpu, SAMPLE_RATE, 0x09);
    // DLPF: fc = 94Hz -> 0x02
    wiringPiI2CWriteReg8(mpu, CONFIG, 0x02);
    // gyro: FS = +-500 -> 0x08
    wiringPiI2CWriteReg8(mpu, GYRO_CONFIG, 0x08);
    // accel: FS = +-8g -> 0x10
    wiringPiI2CWriteReg8(mpu, ACC_CONFIG, 0x10);
    // interrupt enable -> 0x01
    wiringPiI2CWriteReg8(mpu, INTERRUPT, 1);
    // clock source: gyroX -> 0x01
    wiringPiI2CWriteReg8(mpu, PWR_MANAGEMENT, 1);
}
// read sensor
int16_t readSensor(unsigned char sensor) {
    // Tạo biến chứa giá trị byte cao và byte thấp của thanh ghi 16 bit
    uint8_t highByte, lowByte;
    // Đọc byte cao và byte thấp của thanh ghi tương ứng với địa chỉ sensor
    highByte = wiringPiI2CReadReg8(mpu, sensor);
    lowByte = wiringPiI2CReadReg8(mpu, sensor + 1);
    // Ghép byte cao và byte thấp thành giá trị 16 bit và trả về giá trị đó
    return (int16_t)((highByte << 8) | lowByte);
}
//calculate pitch, roll
//Để sử dụng hàm này, bạn cần khai báo hai biến float cho roll và pitch trong hàm chính, 
//và sau đó gọi hàm getRollPitch và truyền biến của bạn làm tham số:
/*
float roll, pitch;
getRollPitch(&roll, &pitch);
// sử dụng giá trị roll và pitch ở đây để tính toán các giá trị khác trong chương trình của bạn
*/
void getRollPitch(float* roll, float* pitch) {
    // đọc giá trị từ các sensor
    float Ax = (float)readSensor(Acc_X)/4096.0;
    float Ay = (float)readSensor(Acc_Y)/4096.0;
    float Az = (float)readSensor(Acc_Z)/4096.0;
    
    // tính góc nghiêng
    *pitch = atan2(Ax, sqrt(pow(Ay,2) + pow(Az,2))) * 180 / M_PI;
    *roll = atan2(Ay, sqrt(pow(Ax,2) + pow(Az,2))) * 180 / M_PI;
}

