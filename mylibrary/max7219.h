#define SPI_CHANNEL 0 //là kênh SPI được sử dụng để kết nối với MAX7219. Trong trường hợp này, kênh SPI được chọn là 0.
#define SPI_SPEED   10000000 //là tốc độ truyền dữ liệu trên kênh SPI. Trong trường hợp này, tốc độ được đặt là 10000000 (10 MHz).

#define MAX7219_REG_NOOP         0x00 //đại diện cho lệnh NO-OP (no operation), được sử dụng như một cách để giữ kết nối SPI được mở

// define dia chi thanh ghi
#define MAX7219_REG_DIGIT0       0x01
#define MAX7219_REG_DIGIT1       0x02
#define MAX7219_REG_DIGIT2       0x03
#define MAX7219_REG_DIGIT3       0x04
#define MAX7219_REG_DIGIT4       0x05
#define MAX7219_REG_DIGIT5       0x06
#define MAX7219_REG_DIGIT6       0x07
#define MAX7219_REG_DIGIT7       0x08

// define dia chi cac thanh ghi cau hinh che do hoat dong cho max7219
#define MAX7219_REG_DECODEMODE   0x09 
#define MAX7219_REG_INTENSITY    0x0A 
#define MAX7219_REG_SCANLIMIT    0x0B 
#define MAX7219_REG_SHUTDOWN     0x0C 
#define MAX7219_REG_DISPLAYTEST  0x0F

void max7219_write(unsigned char reg, unsigned char data); // ham gui data/ hien thi du lieu

void max7219_init(); // gui cau hinh hoat dong cho max7219

void max7219_float_init(); // cau hinh hien thi cho so float
