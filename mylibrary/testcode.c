#include <wiringPi.h>
#include "mpu6050.h"
#include "max7219.h"

// define arrow
unsigned char arrowRight[8] = {0x08, 0x0C, 0x0E, 0xFF, 0xFF, 0x0E, 0x0C, 0x08};
unsigned char arrowLeft[8] = {0x10, 0x30, 0x70, 0xFF, 0xFF, 0x70, 0x30, 0x10};
unsigned char arrowDown[8] = {0x18, 0x18, 0x18, 0x18, 0xFF, 0x7E, 0x3C, 0x18};
unsigned char arrowUp[8] = {0x18, 0x3C, 0x7E, 0xFF, 0x18, 0x18, 0x18, 0x18};
unsigned char Still[8] = {0x00, 0x18, 0x18, 0x7E, 0x7E, 0x18, 0x18, 0x00};

int main(void){
    wiringPiSetup();
    // set up interface I2C, init MPU6050, check I2C conect
    initMpu();
    // set up interface SPI, init Max7219
    max7219_init();
    delay(100); //100ms
    // Đọc giá trị đo
    while(1){
        float roll, pitch;
        // đọc giá trị từ các sensor
        getRollPitch(&roll, &pitch);
        delay(100);
        for(int i = 0;  i < 8; i++){
            max7219_write(i+1, 
            (roll > 20.0) ? arrowLeft[i] :
            (roll < -20.0) ? arrowRight[i] :
            (pitch > 20.0) ? arrowUp[i] :
            (pitch < -20.0) ? arrowDown[i] :
            Still[i]
            );
            delay(30);
        }
    }
    return 0;
}