#include <wiringPiSPI.h>
#include "max7219.h" 

// ham gui data/ hien thi du lieu. 
// nhan 2 tham so dau vao thu nhat la dia chi thanh ghi can can ghi du lieu, thu hai la du lieu can hien thi
void max7219_write(unsigned char reg, unsigned char data) {
    unsigned char buffer[2];
    buffer[0] = reg;
    buffer[1] = data;
    wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2);
}

// cau hinh hien thi cho du lieu dau vao tho^ khong can xu ly
void max7219_init() {
    wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED); // 2 thong so trong cau lenh da dc define trong file .h
    max7219_write(MAX7219_REG_SCANLIMIT, 0x07); // xác định số lượng thanh ghi số sẽ được sử dụng để hiển thị: 8
    max7219_write(MAX7219_REG_DECODEMODE, 0x00); // cấu hình chế độ giải mã: none
    max7219_write(MAX7219_REG_DISPLAYTEST, 0x00); // để tắt chế độ kiểm tra hiển thị
    max7219_write(MAX7219_REG_SHUTDOWN, 0x01); // bật MAX7219
    max7219_write(MAX7219_REG_INTENSITY, 0x0F); // độ sáng của LED. 0 (tối nhất) đến 15 (sáng nhất).
}
// cau hinh hien thi cho so float
void max7219_float_init() {
    wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED);
    max7219_write(MAX7219_REG_SCANLIMIT, 0x07); // xác định số lượng thanh ghi số sẽ được sử dụng để hiển thị: 8
    max7219_write(MAX7219_REG_DECODEMODE, 0xFF); // cấu hình chế độ giải mã: on
    max7219_write(MAX7219_REG_DISPLAYTEST, 0x00); // để tắt chế độ kiểm tra hiển thị
    max7219_write(MAX7219_REG_SHUTDOWN, 0x01); // bật MAX7219
    max7219_write(MAX7219_REG_INTENSITY, 0x0F); // độ sáng của LED. 0 (tối nhất) đến 15 (sáng nhất).
}
