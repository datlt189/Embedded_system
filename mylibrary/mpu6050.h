#define MPU_ADDRESS     0x68 // dia chi mpu 
#define SAMPLE_RATE     0x19
#define CONFIG          0x1A
#define GYRO_CONFIG     0x1B
#define ACC_CONFIG      0x1C
#define INTERRUPT       0x38
#define PWR_MANAGEMENT  0x6B

//define 3 trục x, y, z
#define Acc_X           59 // 59-high, 60-low
#define Acc_Y           61 // 61-high, 62-low
#define Acc_Z           63 // 63-high, 64-low

void initMpu(void); // cau hinh che do hoat dong cho mpu6050
void getRollPitch(float* roll, float* pitch); // ham lay gia tri gia toc tu cac sensor -> tinh toan ra 2 tham so roll va pitch

