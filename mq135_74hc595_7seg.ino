//
//  mq135_74hc595_7seg.ino
//
// github:
//     https://github.com/yoggy/mq135_74hc595_7seg
//
// license:
//     Copyright (c) 2016 yoggy <yoggy0@gmail.com>
//     Released under the MIT license
//     http://opensource.org/licenses/mit-license.php;
//
#define MQ135_AO 0

long count = 0;
long total = 0;
long total_count = 100;
long avg = 0;

void setup() {
  Serial.begin(9600);
  setup_74hc595();
}

void loop() {
  // read analog value from MQ-135 AO pin.
  // http://www.aitendo.com/product/10193
  
  total += analogRead(MQ135_AO);
  count ++;

  if (count == total_count) {
    avg = total / total_count;
    total = 0;
    count = 0;
    Serial.println(avg);
  }

  disp_74hc595(avg);
}

//
//  for TM4D595 (7segment display using 74HC595)
//
//  see also...
//  http://www.aitendo.com/product/13451
//  http://aitendo3.sakura.ne.jp/aitendo_data/product_img/LED/TM4D595/TM4D595_SKETCH.txt
//
unsigned char LED_BCD[16] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e};

int disp_74hc595_dio = 2;
int disp_74hc595_rclk = 3;
int disp_74hc595_sclk = 5;

void setup_74hc595() {
  pinMode(disp_74hc595_dio, OUTPUT);
  pinMode(disp_74hc595_rclk, OUTPUT);
  pinMode(disp_74hc595_sclk, OUTPUT);
}

void disp_74hc595(int val) {
  static int idx = 0;

  if (idx == 4) {
    write_74HC595(0); // blank
    delay(7);
  }
  else {
    char buf[5];
    snprintf(buf, 5, "%04d", val);
    HC_dio_analyze(idx, buf[3-idx] - '0', 0);
  }

  idx ++;
  idx = idx % 5;
}

void HC_dio_analyze(char LED_number, unsigned char LED_display, unsigned char LED_dp)
{
  unsigned int HC_DISP = 0, HC_LEDCODE, HC_LEDCODE_temp = 0;
  if (LED_display > 15) LED_display = 0;

  HC_LEDCODE = LED_BCD[LED_display] ;

  for (unsigned char i = 0; i < 8; ++i) {
    HC_LEDCODE_temp <<= 1;

    if (HC_LEDCODE & 0x01) HC_LEDCODE_temp |= 0x01;

    HC_LEDCODE >>= 1;
  }

  if (LED_dp)  HC_LEDCODE_temp &= 0xfe;

  HC_DISP = HC_LEDCODE_temp;

  switch (LED_number)
  {
    case 0: HC_DISP |= 0x8000; break;
    case 1: HC_DISP |= 0x4000; break;
    case 2: HC_DISP |= 0x2000; break;
    case 3: HC_DISP |= 0x1000; break;
  }

  write_74HC595(HC_DISP);
}

void write_74HC595(unsigned int data)
{
  digitalWrite(disp_74hc595_rclk, LOW);
  shiftOut(disp_74hc595_dio, disp_74hc595_sclk, LSBFIRST, data);
  shiftOut(disp_74hc595_dio, disp_74hc595_sclk, LSBFIRST, data >> 8);
  digitalWrite(disp_74hc595_rclk, HIGH);
}
