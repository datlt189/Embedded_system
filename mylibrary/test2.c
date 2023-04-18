#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "max7219.h"
#include "mpu6050.h"

// ham hien thi gia tri so thuc
void display_float(float num, uint8_t dec, uint8_t pos) // num: so can hien thi; dec: lay bao nhieu so sau dau phay; pos: vtri hien thi
    { 
    int32_t integerPart = (num); // lay phan nguyen
    int32_t fractionalPart = (num - integerPart) * pow(10,dec); // lay phan thap phan sau dau phay
    int32_t number = fabs(integerPart*pow(10,dec) + fractionalPart); // cong lai thanh chuoi de hien thi, fabs: lay tri tuyet doi so thuc
    /* vd: 12.34: dec = 2 
    -> integerPart = 12;
    -> fractionalPart = 12.34 - 12 = 0.34*10^dec = 34; 
    -> number = 12*10^2 + 34 = 1200 + 34 = 1234;
    **do vay khi so can hien thi nam trong [0,1) thi number cho ra se khong co so 0 o phia truoc do phep cong
    vd 0.12: dec = 2
    -> integerPart = 0;
    -> fractionalPart = 0.12 - 0 = 0.12*10^dec = 12; 
    -> number = 0*10^2 + 34 = 0 + 12 = 12;
    => khi hien thi se bi thieu so 0 va dau cham => truong hop nay dc tach rieng de hien thi nhu ben duoi   
    */
    // Đếm số chữ số của số hiển thị
    uint8_t count = 1;
    int32_t n = number;
    while (n / 10) { // trong khi n vẫn còn ít nhất một chữ số
        count++;
        n = n / 10; // Chia giá trị của n cho 10 để loại bỏ chữ số cuối cùng của n.
    }

    // Set scanlimit
    max7219_write(0x0B, 7); // giới hạn số lượng LED được quét va hien thi trên bảng LED.
    // neu set la count-1 thi se quet va hien thi tren bang led dung bang so luong chu so can hien thi
    // neu set la 7 thi all

    // Hiển thị số
    if (integerPart>=0 && integerPart<1) // xet rieng de hien thi cho truong hop so can hien thi nam trong [0,1) 
                                        // se hien thi so 0 va dau cham phia truoc
    {
        integerPart = 0;
        number = fabs(integerPart*pow(10,dec) + fractionalPart);
        for (int i = 0; i < count; i++) {
            max7219_write(i + pos + 1, number % 10); 
            number = number / 10;
        }
        max7219_write(dec+pos+1, 0|0x80); // them so 0 va dau cham tai vi tri hien thi cong voi so luong chu so sau dau phay + them 1
    }
    else{ // nguoc lai thi hien thi nhu binh thuong
        for (int i = 0; i < count; i++) {
            if (i == dec) {
                // Nếu đến vị trí hiển thị phần thập phân thì hiển thị dấu chấm
                max7219_write(i + pos + 1, (number % 10) | 0x80);
            } else {
                max7219_write(i + pos + 1, number % 10);
            }
            number = number / 10;
        }
    }
}

int main(void){
    // set operational mode for max7219
    max7219_float_init();
    // set operational mode for mpu6050
    initMpu();

    float roll, pitch; // de dung getRollPitch()
    int ledDisplayCount = 0; // bien dem luu gia tri de so sanh hien thi
    while(1){
        getRollPitch(&roll, &pitch);
        // tat cac led hien thi truoc do 
        for (int i = 1; i <= 8; i++){
            max7219_write(i, 0x0F);
        }
        // kiem tra xem gia tri roll co lon hon 45.0 hay khong neu co thi cho led hien thi chop tat lien tuc bang cach ben duoi
        if (fabs(roll) > 45.0) {  
            // Chỉ hiển thị LED nếu đếm chẵn
            if (ledDisplayCount % 2 == 0) {
                display_float(roll, 1, 4);
            }
            display_float(pitch, 1, 0); // dong thoi hien thi pitch
            
            ledDisplayCount++; // Tăng giá trị biến đếm sau mỗi lần lặp
            
        } 
        // tuong tu cho pitch
        else if (fabs(pitch) > 45.0){
            // Chỉ hiển thị LED nếu đếm chẵn. cach nay don gian de hieu nhung khong chinh dc tan so chop tat nhanh cham
            //rieng cho tung doan, ma neu muon chinh thi chinh delay o cuoi code no se chinh luon ca tan so quet hien thi cua led luon,
            //do bien ledDisplayCount phu thuoc vao so lan lap de hien thi.
            // Con 1 cach nua la tat led tuong ung voi roll pitch xong hien thi lai cach nay co the chinh thoai mai tan so hien thi
            //bang cach cho delay moi luc tat bat ma khong anh huong den tan so quet hien thi cua led.
            if (ledDisplayCount % 2 == 0) {
                display_float(pitch, 1, 0);
            }
            display_float(roll, 1, 4);
            
            ledDisplayCount++; // Tăng giá trị biến đếm sau mỗi lần lặp
            
        }
        else {
            // Hiển thị LED bình thường nếu không vượt quá ngưỡng 45 độ
            display_float(roll, 1, 4);
            display_float(pitch, 1, 0);
            ledDisplayCount = 0; // Đặt lại giá trị biến đếm nếu không vượt quá ngưỡng 45 độ
        }
        // hien thi roll pitch len terminal de check
        printf("roll: %.2f\n", roll);
        printf("pitch: %.2f\n", pitch);
        delay(150);
        
        
    }
    return 0;
}