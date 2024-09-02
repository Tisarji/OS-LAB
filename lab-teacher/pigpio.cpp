#include <pigpio.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char modeName[9][8] = {"IN","OUT","ALT5","ALT4","ALT0","ALT1","ALT2","ALT3"," "};
char headerName[40][8] = {"3.3v","5v","2","5v","3","GND","4","14","GND","15",
						  "17","18","27","GND","22","23","3.3v","24","10","GND",
						  "9","25","11","8","GND","7","0","1","5","GND",
						  "6","12","13","GND","19","16","26","20","GND","21"};
int headerMap[40] =       {-1,-1,2,-1,3,-1,4,14,-1,15,
						  17,18,27,-1,22,23,-1,24,10,-1,
						  9,25,11,8,-1,7,0,1,5,-1,
						  6,12,13,-1,19,16,26,20,-1,21};
#define gpioPins 40

char altFunc[6][28][16]={{"SDA0","SCL0","SDA1","SCL1","GPCLK0","GPCLK1","GPCLK2","SPI0_CE1_N","SPI0_CE0_N","SPI0_MISO",
						  "SPI0_MOSI","SPI0_SCLK","PWM0","PWM1","TXD0","RXD0","FL0","FL1","PCM_CLK","PCM_FS",
						  "PCM_DIN","PCM_DOUT","SD0_CLK","SD0_XMD","SD0_DAT0","SD0_DAT1","SD0_DA2","SD0_DA3"},
                         {"SA5","SA4","SA3","SA2","SA1","SA0","SOE_N","SWE_N","SD0","SD1",
						  "SD2","SD3","SD4","SD5","SD6","SD7","SD8","SD9","SD10","SD11",
						  "SD12","SD13","SD14","SD15","SD16","SD17","TE0","TE1"},
				         {"PCLK","DE","LCD_VSYNC","LCD_HSYNC","DPI_D0","DPI_D1","DPI_D2","DPI_D3","DPI_D4","DPI_D5",
						  "DPI_D6","DPI_D7","DPI_D8","DPI_D9","DPI_D10","DPI_D11","DPI_D12","DPI_D13","DPI_D14","DPI_D15",
						  "DPI_D16","DPI_D17","DPI_D18","DPI_D19","DPI_D20","DPI_D21","DPI_D22","DPI_D23"},
				         {"SPI3_CEO_N","SPI3_MISO","SPI3_MOSI","SPI3_SCLK","SPI4_CEO_N","SPI4_MISO","SPI4_MOSI","SPI4_SCLK","_","_",
						  "_","_","SPI5_CE0_N","SPI5_MISO","SPI5_MOSI","SPI5_SCLK","CTS0","RTS0","SPI6_CE0_N","SPI6_MISO",
						  "SPI6_MOSI","SPI6_SCLK","SD1_CLK","SD1_CMD","SD1_DAT0","SD1_DAT1","SD1_DAT2","SD1_DAT3"},
                         {"TXD2","RXD2","CTS2","RTS2","TXD3","RXD3","CTS3","RTS3","TXD4","RXD4",
						  "CTS4","RTS4","TXD5","RXD5","CTS5","RTS5","SPI1_CE2_N","SPI1_CE1_N","SPI1_CE0_N","SPI1_MISO",
						  "SPI1_MOSI","SPI1_SCLK","ARM_TRST","ARM_RTCX","ARM_TDO","ARM_TCK","ARM_TDI","ARM_TMS"},
                         {"SDA6","SCL6","SDA3","SCL3","SDA3","SCL3","SDA4","SCL4","SDA4","SCL4",
						  "SDA5","SCL5","SDA5","SCL5","TXD1","RXD1","CTS1","RTS1","PWM0","PWM1",
						  "GPCLK0","GPCLK1","SDA6","SCL6","SPI3_CE1_N","SPI4_CE1_N","SPI5_CE1_N","SPI6_CE1_N"}};

char pullFunc[28][8]={"High","High","High","High","High","High","High","High","High","Low",
					  "Low","Low","Low","Low","Low","Low","Low","Low","Low","Low",
					  "Low","Low","Low","Low","Low","Low","Low","Low",};

char piModel[32][6]={"A","B","A+","B+","2B","-","CM1","-",
				     "3B","Zero","CM3","-","ZeroW","3B+","3A+","-",
					 "CM3+","4b"};

char *pinName(int pin);
char *pinMode(int pin);
int pinValue(int pin);

int main(){
    int hwRevision,h,i;

    if(gpioInitialise()<0){
        printf("usage:  sudo ./pigpio\n");
        exit(-1);
    }

    hwRevision = gpioHardwareRevision()>>4;
    hwRevision %=32;

    printf("+-----+------------+------+---+ PI %-5s +---+------+------------+-----+\n",piModel[hwRevision]);
    printf("| BCM |    Name    | Mode | V |  Board   | V | Mode | Name       | BCM |\n");
    printf("+-----+------------+------+---+----++----+---+------+------------+-----+\n");
    for(i=0;i<40;i+=2){
//     odd pin
        h = i;
        if(headerMap[h]<0){
            printf("|     | %10s |      |   |",pinName(h));
        }else{
            printf("| G%.2d | %10s | %4s | %.1d |",headerMap[h],pinName(h),pinMode(h),pinValue(h));
        }
        printf(" %2d || %2d ",i+1,i+2);
//     even pin
        h = i+1;
        if(headerMap[h]<0){
            printf("|   |      | %10s |     |\n",pinName(h));
        }else{
            printf("| %.1d | %4s | %10s | G%.2d |\n",pinValue(h),pinMode(h),pinName(h),headerMap[h]);
        }
    }
    printf("+-----+------------+------+---+----++----+---+------+------------+-----+\n");
    printf("| BCM |    Name    | Mode | V |  Board   | V | Mode | Name       | BCM |\n");
    printf("+-----+------------+------+---+ PI %-5s +---+------+------------+-----+\n",piModel[hwRevision]);
    gpioTerminate();
    return 0;
}

char *pinName(int pin){
    if(headerMap[pin]<0) return headerName[pin];
	switch(gpioGetMode(headerMap[pin])){
	case PI_ALT0: return altFunc[0][headerMap[pin]];
	case PI_ALT1: return altFunc[1][headerMap[pin]];
	case PI_ALT2: return altFunc[2][headerMap[pin]];
	case PI_ALT3: return altFunc[3][headerMap[pin]];
	case PI_ALT4: return altFunc[4][headerMap[pin]];
	case PI_ALT5: return altFunc[5][headerMap[pin]];
	}
	return headerName[pin];
}
char *pinMode(int pin){
    if(headerMap[pin]<0)
		return modeName[8];
    return modeName[gpioGetMode(headerMap[pin])];
}

int pinValue(int pin){
	if(headerMap[pin]<0)
		return -1;
	return gpioRead(headerMap[pin]);
}
