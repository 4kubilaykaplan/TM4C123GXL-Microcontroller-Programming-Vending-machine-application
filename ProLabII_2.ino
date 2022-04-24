// KUBİLAY KAPLAN - 180201123 / SUAT YÜCELEN - 180201131
// KOCAELİ ÜNİVERSİTESİ - PRO. LAB. II - 2. PROJE
// PROJEDE TIVA C SERİSİ TM4C123 LAUNCHPAD GELİŞTİRME KARTI KULLANILDI.
// PROJE, ENERGIA IDE ÜZERİNDE GELİŞTİRİLDİ.
// BREADBOARD ÜZERİNDEKİ HARİCİ DEVREDE NOKIA 5110 BLUE LCD PANEL, 10K POTANSIYOMETRE, 6 ŞAR ADET BUTON VE 10K OHM DİRENÇ KULLANILDI.
//
// PINLER AŞAĞIDA BELİRTİLMİŞTİR:
//
// LCD 5110   -   TM4C123
// ==================================
// (1) RST    -   PB_5
// (2) CE     -   PA_7
// (3) DC     -   PA_2
// (4) Din    -   PB_7
// (5) Clk    -   PB_4
// (6) Vcc    -   +3.3v
// (7) BL     -   Potansiyometre Orta Pini -> +3.3v
// (8) GND    -   GND
// ==================================
// ==================================
// BUTONLAR   -   TM4C123
// ==================================
// BUTON 1    -   PE2
// BUTON 2    -   PE1
// BUTON 3    -   PD3
// BUTON 4    -   PD2
// BUTON 5    -   PD1
// BUTON 6    -   PD0
// ==================================

#include <LCD_5110_SPI.h>
#include "SPI.h"
#include "Energia.h"
#include <EEPROM.h>
#include <stdio.h>
#define kirmizi RED_LED
#define yesil GREEN_LED
#define mavi BLUE_LED

LCD_5110_SPI ekran(PA_7,    // Chip Select
                   PA_2,    // Data/Command
                   PB_5,    // Reset
                   PE_3,    // Backlight - bu programda kullanılmamıştır
                   PF_1);   // Push Button - bu programda kullanılmamıştır

// 1.AŞAMA -  2.AŞAMA
//==============================
const int buton1 = PE_2;    //  5 TL   -  Köpükleme   - İLERLEME BUTONU
const int buton2 = PE_1;    // 10 TL   -  Yıkama
const int buton3 = PD_3;    // 20 TL   -  Kurulama
const int buton4 = PD_2;    // 50 TL   -  Cilalama
const int buton5 = PD_1;    //100 TL   -  BİTİŞ
const int buton6 = PD_0;    // BİTİŞ   -  RESET

int kasa[5] = {20, 20, 10, 30, 5}; // 5 TL, 10 TL, 20 TL, 50 TL, 100 TL ADETLERİ
int kopukleme[2] = {30, 15}; //30 KULLANIM, 15 TL
int yikama[2] = {50, 10};    //50 KULLANIM, 10 TL
int kurulama[2] = {100, 5};  //100 KULLANIM, 5 TL
int cilalama[2] = {20, 50};  //20 KULLANIM, 50 TL


int butonDurum1 = 0;
int butonDurum2 = 0;
int butonDurum3 = 0;
int butonDurum4 = 0;
int butonDurum5 = 0;
int butonDurum6 = 0;

int randomSayi = 0;
int yuklenen = 0;
int tempYuklenen = 0;

int sKopukleme = 0;
int sYikama = 0;
int sKurulama = 0;
int sCilalama = 0;

int s5TL = 0;
int s10TL = 0;
int s20TL = 0;
int s50TL = 0;
int s100TL = 0;

boolean bittiMi = false;
boolean basarili = false;

void setup()
{
  SPI.setModule(2);      // üçüncü SPI portu kullanılıyor.
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV128); // Clock ayarlama => LM4F120H5QR DIV2 = 4 MHz !  
  
  Serial.begin(9600);
  
  //EEPROM.write(0, 0);
  if(EEPROM.read(0)==0)
  {
  bellegeYaz();
  }
  else if(EEPROM.read(0)==1)
  {
  bellektenOku();
  }
  
  randomSeed(analogRead(0));
  initPinModes();
  ekran.begin();
  ekran.setContrast(0x48);
  ekran.clear();

  while (digitalRead(buton1) == HIGH)
  {
    ekran.setFont(0);
    ekran.text(0, 0, "KOU-BIL. MUH.");
    ekran.text(5, 9, "PRO. LAB. II");
    ekran.text(0, 18, "KUBILAY KAPLAN");
    ekran.text(5, 27, "SUAT YUCELEN");
    ekran.setFont(2);
    ekran.text(36, 42, "BUTON:1 -->>");
    ekran.copyToScreen();
  }
  delay(500);
  ekran.clear();
  ekran.setFont(2);
  ekran.text(0, 0, "PARA YUKLEME EKRANI");
  ekran.text(0, 6, "1: 5 TL");
  ekran.text(0, 12, "2: 10 TL");
  ekran.text(0, 18, "3: 20 TL");
  ekran.text(0, 24, "4: 50 TL");
  ekran.text(0, 30, "5: 100 TL");
  ekran.text(0, 36, "6: BITIRME");
  ekran.text(0, 42, "BIR BUTONA BASIN(1-6)");
  ekran.copyToScreen();

}

void loop()
{
  butonDurum1 = digitalRead(buton1);
  butonDurum2 = digitalRead(buton2);
  butonDurum3 = digitalRead(buton3);
  butonDurum4 = digitalRead(buton4);
  butonDurum5 = digitalRead(buton5);
  butonDurum6 = digitalRead(buton6);

  if (bittiMi == false)
  {
    digitalWrite(kirmizi, LOW);
    digitalWrite(mavi, HIGH);
    buton5TL();
    buton10TL();
    buton20TL();
    buton50TL();
    buton100TL();
    yuklemeBitis();
  }
  else
  {
    secimBitis();

    if (basarili == false)
    {
      digitalWrite(kirmizi, LOW);
      digitalWrite(mavi, HIGH);
      kopuklemeButon();
      yikamaButon();
      kurulamaButon();
      cilalamaButon();
      resetButon();
    }
  }
}

void initPinModes()
{
  pinMode(kirmizi, OUTPUT);
  pinMode(yesil, OUTPUT);
  pinMode(mavi, OUTPUT);
  pinMode(buton1, INPUT_PULLUP);
  pinMode(buton2, INPUT_PULLUP);
  pinMode(buton3, INPUT_PULLUP);
  pinMode(buton4, INPUT_PULLUP);
  pinMode(buton5, INPUT_PULLUP);
  pinMode(buton6, INPUT_PULLUP);
}

void bellektenOku()
{
  kasa[0] = EEPROM.read(1);
  kasa[1] = EEPROM.read(2);
  kasa[2] = EEPROM.read(3);
  kasa[3] = EEPROM.read(4);
  kasa[4] = EEPROM.read(5);
  kopukleme[0] = EEPROM.read(6);
  yikama[0] = EEPROM.read(7);
  kurulama[0] = EEPROM.read(8); 
  cilalama[0] = EEPROM.read(9); 
}

void bellegeYaz()
{
  EEPROM.write(1, kasa[0]);
  EEPROM.write(2, kasa[1]);
  EEPROM.write(3, kasa[2]);
  EEPROM.write(4, kasa[3]);
  EEPROM.write(5, kasa[4]);
  EEPROM.write(6, kopukleme[0]);
  EEPROM.write(7, yikama[0]);
  EEPROM.write(8, kurulama[0]); 
  EEPROM.write(9, cilalama[0]);
}

// 1.AŞAMA FONKSİYONLARI

void paraYuklemeEkrani(int miktar)
{
  ekran.clear();
  String para = String((String(yuklenen)) + " TL");
  String yukleme = String((String(miktar)) + " TL YUKLENDI...");
  ekran.setFont(2);
  ekran.text(0, 0, yukleme);
  ekran.setFont(0);
  ekran.text(0, 15, "TOPLAM BAKIYE:");
  ekran.text(0, 24, para);
  ekran.copyToScreen();
}

void buton5TL()
{
  if (butonDurum1 == 0) //5TL
  {
    yuklenen += 5;
    s5TL++;
    digitalWrite(mavi, LOW);
    delay(500);
    paraYuklemeEkrani(5);
  }
  else {}
}

void buton10TL()
{
  if (butonDurum2 == 0) //10TL
  {
    yuklenen += 10;
    s10TL++;
    digitalWrite(mavi, LOW);
    delay(500);
    paraYuklemeEkrani(10);
  }
  else { }
}

void buton20TL()
{
  if (butonDurum3 == 0) //20TL
  {
    yuklenen += 20;
    s20TL++;
    digitalWrite(mavi, LOW);
    delay(500);
    paraYuklemeEkrani(20);
  }
  else {}
}

void buton50TL()
{
  if (butonDurum4 == 0) //50TL
  {
    yuklenen += 50;
    s50TL++;
    digitalWrite(mavi, LOW);
    delay(500);
    paraYuklemeEkrani(50);
  }
  else {}
}

void buton100TL()
{
  if (butonDurum5 == 0) //100TL
  {
    yuklenen += 100;
    s100TL++;
    digitalWrite(mavi, LOW);
    delay(500);
    paraYuklemeEkrani(100);
  }
  else {}
}

void yuklemeBitis()
{
  if (butonDurum6 == 0) // YÜKLEME BİTİŞ
  {
    bittiMi = true;
    tempYuklenen = yuklenen;
    digitalWrite(mavi, LOW);
    delay(500);
    ekran.clear();
    ekran.setFont(2);
    ekran.text(0, 0, "HIZMET SECIM EKRANI");
    ekran.setFont(0);
    ekran.text(0, 6, "1-KOPUKLEME");
    ekran.text(0, 15, "2-YIKAMA");
    ekran.text(0, 24, "3-KURULAMA");
    ekran.text(0, 33, "4-CILALAMA");
    ekran.setFont(2);
    ekran.text(0, 42, "5-BITIS | 6-RESET");
    ekran.copyToScreen();
  }
  else { }
}

// 2.AŞAMA FONKSİYONLARI

void yetersizBakiyeEkrani()
{
  ekran.clear();
  String para = String("TOPLAM BAKIYE: " + (String(yuklenen)) + " TL");
  ekran.setFont(2);
  ekran.text(0, 0, "YETERSIZ BAKIYE...");
  ekran.text(0, 6, "ISLEM YAPILAMADI...");
  ekran.text(0, 12, para);
  ekran.copyToScreen();
}

void hizmetEkrani()
{
  ekran.clear();
  String para = String("TOPLAM BAKIYE:" + (String(yuklenen)) + " TL");
  String hizmet1 = String("KOPUK.: " + (String(sKopukleme)) + " - STOK:" + (String(kopukleme[0] - sKopukleme)));
  String hizmet2 = String("YIKAMA: " + (String(sYikama)) + " - STOK:" + (String(yikama[0] - sYikama)));
  String hizmet3 = String("KURUL.: " + (String(sKurulama)) + " - STOK:" + (String(kurulama[0] - sKurulama)));
  String hizmet4 = String("CILAL.: " + (String(sCilalama)) + " - STOK:" + (String(cilalama[0] - sCilalama)));
  ekran.setFont(2);
  ekran.text(0, 0, para);
  ekran.text(0, 8, "ALINAN HIZMETLER:");
  ekran.text(0, 16, hizmet1);
  ekran.text(0, 24, hizmet2);
  ekran.text(0, 32, hizmet3);
  ekran.text(0, 40, hizmet4);
  ekran.copyToScreen();
}

void yetersizStokEkrani()
{
  ekran.clear();
  ekran.setFont(0);
  ekran.text(0, 9, "BU HIZMET ICIN");
  ekran.text(0, 18, "YETERLI STOK");
  ekran.text(0, 27, "YOK");
  ekran.copyToScreen();
}

void kalanHizmetlerEkrani()
{
  ekran.clear();
  String hizmet1 = String("KOPUKLEME: " + (String(kopukleme[0])));
  String hizmet2 = String("YIKAMA   : " + (String(yikama[0])));
  String hizmet3 = String("KURULAMA : " + (String(kurulama[0])));
  String hizmet4 = String("CILALAMA : " + (String(cilalama[0])));
  ekran.setFont(2);
  ekran.text(0, 0, "STOK BILGISI:");
  ekran.setFont(0);
  ekran.text(0, 6, hizmet1);
  ekran.text(0, 15, hizmet2);
  ekran.text(0, 24, hizmet3);
  ekran.text(0, 33, hizmet4);
  ekran.setFont(2);
  ekran.text(36, 42, "BUTON:1 -->>");
  ekran.copyToScreen();
}

void kalanKasaEkrani()
{
  ekran.clear();
  int guncelKasaToplam = 5 * kasa[0] + 10 * kasa[1] + 20 * kasa[2] + 50 * kasa[3] + 100 * kasa[4];
  String kasaBilgisi = String("KASA : " + (String(guncelKasaToplam)) + " TL");
  String para1 = String("5 TL ADET   : " + (String(kasa[0])));
  String para2 = String("10 TL ADET  : " + (String(kasa[1])));
  String para3 = String("20 TL ADET  : " + (String(kasa[2])));
  String para4 = String("50 TL ADET  : " + (String(kasa[3])));
  String para5 = String("100 TL ADET : " + (String(kasa[4])));
  ekran.setFont(2);
  ekran.text(0, 0, kasaBilgisi);
  ekran.text(0, 7, para1);
  ekran.text(0, 14, para2);
  ekran.text(0, 21, para3);
  ekran.text(0, 28, para4);
  ekran.text(0, 35, para5);
  //ekran.text(36, 42, "BUTON:1 -->>");
  ekran.copyToScreen();
}

void kasaStokAyarla()
{
  kasa[0] += s5TL;
  kasa[1] += s10TL;
  kasa[2] += s20TL;
  kasa[3] += s50TL;
  kasa[4] += s100TL;
  kopukleme[0] -= sKopukleme;
  yikama[0] -= sYikama;
  kurulama[0] -= sKurulama;
  cilalama[0] -= sCilalama;
}

void secimleriSifirla()
{
  sKopukleme = 0;
  sYikama = 0;
  sKurulama = 0;
  sCilalama = 0;
}

void paralariSifirla()
{
  s5TL=0;
  s10TL=0;
  s20TL=0;
  s50TL=0;
  s100TL=0;
}

void paraBoz (int para[5], int farkGiren)
{
  int fark = farkGiren;
  int banknot[5] = {5, 10, 20, 50, 100};
  int x, i;

  for (i = 5; i--; i > -1)
  {
    x = fark / banknot[i];

    if (x < kasa[i])
    {
      para[i] = x;
      kasa[i] -= x;
      fark -= x * banknot[i];
    }
    else if (kasa[i] == 0)
    {
      para[i] = 0;
    }
    else if ((x > kasa[i]) || (x == kasa[i]))
    {
      para[i] = kasa[i];
      x -= kasa[i];
      fark -= kasa[i] * banknot[i];
      kasa[i] = 0;
    }
  }
}

void kopuklemeButon()
{
  if (butonDurum1 == 0) //KÖPÜKLEME
  {
    if ( (kopukleme[0] - sKopukleme) > 0 )
    {
      if (yuklenen < kopukleme[1])
      {
        digitalWrite(mavi, LOW);
        delay(500);
        yetersizBakiyeEkrani();
      }
      else
      {
        sKopukleme++;
        yuklenen -= kopukleme[1];
        digitalWrite(mavi, LOW);
        delay(500);
        hizmetEkrani();
      }
    }
    else
    {
      digitalWrite(mavi, LOW);
      delay(500);
      yetersizStokEkrani();
    }
  }
  else {}
}

void yikamaButon()
{
  if (butonDurum2 == 0) //YIKAMA
  {
    if ( (yikama[0] - sYikama) > 0 )
    {
      if (yuklenen < yikama[1])
      {
        digitalWrite(mavi, LOW);
        delay(500);
        yetersizBakiyeEkrani();
      }
      else
      {
        sYikama++;
        yuklenen -= yikama[1];
        digitalWrite(mavi, LOW);
        delay(500);
        hizmetEkrani();
      }
    }
    else
    {
      digitalWrite(mavi, LOW);
      delay(500);
      yetersizStokEkrani();
    }
  }
  else {}
}

void kurulamaButon()
{
  if (butonDurum3 == 0) //KURULAMA
  {
    if ( (kurulama[0] - sKurulama) > 0 )
    {
      if (yuklenen < kurulama[1])
      {
        digitalWrite(mavi, LOW);
        delay(500);
        yetersizBakiyeEkrani();
      }
      else
      {
        sKurulama++;
        yuklenen -= kurulama[1];
        digitalWrite(mavi, LOW);
        delay(500);
        hizmetEkrani();
      }
    }
    else
    {
      digitalWrite(mavi, LOW);
      delay(500);
      yetersizStokEkrani();
    }
  }
  else {}
}

void cilalamaButon()
{
  if (butonDurum4 == 0) //CİLALAMA
  {
    if ( (cilalama[0] - sCilalama) > 0 )
    {
      if (yuklenen < cilalama[1])
      {
        digitalWrite(mavi, LOW);
        delay(500);
        yetersizBakiyeEkrani();
      }
      else
      {
        sCilalama++;
        yuklenen -= cilalama[1];
        digitalWrite(mavi, LOW);
        delay(500);
        hizmetEkrani();
      }
    }
    else
    {
      digitalWrite(mavi, LOW);
      delay(500);
      yetersizStokEkrani();
    }
  }
  else {}
}

void resetButon()
{
  if (butonDurum6 == 0) //RESET
  {
    yuklenen = tempYuklenen;
    secimleriSifirla();
    digitalWrite(mavi, LOW);
    delay(500);
    hizmetEkrani();
  }
  else { }
}

void secimBitis()
{
  if (butonDurum5 == 0) //SEÇİM BİTİŞİ
  {
    basarili = true;
    randomSayi = random(0, 4);
    String sayi = String("RANDOM SAYI: " + (String(randomSayi)));

    if (randomSayi == 2)
    {
      String STRreset = String("IADE: " + (String(tempYuklenen)) + " TL");
      digitalWrite(mavi, LOW);
      delay(1000);
      digitalWrite(yesil, LOW);
      delay(1000);
      digitalWrite(kirmizi, HIGH);
      delay(1000);
      secimleriSifirla();
      ekran.clear();
      ekran.setFont(0);
      ekran.text(0, 0, sayi);
      ekran.text(0, 9, "PARA SIKISMASI");
      ekran.text(0, 18, "LUTFEN SISTEMI");
      ekran.text(0, 27, "RESETLEYIN");
      ekran.text(0, 36, STRreset);
      ekran.copyToScreen();
    }
    else
    {
      digitalWrite(mavi, LOW);
      digitalWrite(kirmizi, LOW);
      delay(1000);
      digitalWrite(yesil, HIGH);
      delay(1000);
      kasaStokAyarla();
      int diziParaUstu[5] = {0, 0, 0, 0, 0};
      int tempKasaToplam = 5 * kasa[0] + 10 * kasa[1] + 20 * kasa[2] + 50 * kasa[3] + 100 * kasa[4];
      paraBoz(diziParaUstu, yuklenen);
      int guncelKasaToplam = 5 * kasa[0] + 10 * kasa[1] + 20 * kasa[2] + 50 * kasa[3] + 100 * kasa[4];
      int paraUstu = 5 * diziParaUstu[0] + 10 * diziParaUstu[1] + 20 * diziParaUstu[2] + 50 * diziParaUstu[3] + 100 * diziParaUstu[4];
      String strParaUstu = String("PARAUSTU:" + (String(paraUstu)) + "TL");
      String borc = String("BORCUMUZ:" + (String(yuklenen - paraUstu)) + "TL");
      String strParaUstu1 = String("100'LUK: " + (String(diziParaUstu[4])) + "-50'LIK: " + (String(diziParaUstu[3])));
      String strParaUstu2 = String("20'LIK: " + (String(diziParaUstu[2])) + "-10'LUK: " + (String(diziParaUstu[1])));
      String strParaUstu3 = String("5'LIK: " + (String(diziParaUstu[0])));
      EEPROM.write(0, 1);
      bellegeYaz();

      while (digitalRead(buton1) == HIGH)
      {
        if ((paraUstu < yuklenen) && (tempKasaToplam > yuklenen)) // KASADA PARA VAR ANCAK PARA ÜSTÜ İÇİN GEREKLİ BANKNOTLAR YOK
        {
          ekran.clear();
          ekran.setFont(2);
          ekran.text(0, 0, sayi);
          ekran.text(0, 6, "KASA YETERSIZ!");
          ekran.text(0, 12, strParaUstu);
          ekran.text(0, 18, borc);
          ekran.text(0, 24, strParaUstu1);
          ekran.text(0, 30, strParaUstu2);
          ekran.text(0, 36, strParaUstu3);
          ekran.text(36, 42, "BUTON:1 -->>");
          ekran.copyToScreen();
        }

        else if (tempKasaToplam < yuklenen) // KASA TOPLAMI, PARA ÜSTÜ İÇİN YETERSİZ
        {
          ekran.clear();
          ekran.setFont(2);
          ekran.text(0, 0, sayi);
          ekran.text(0, 6, "KASA YETERSIZ!");
          ekran.text(0, 12, strParaUstu);
          ekran.text(0, 18, borc);
          ekran.text(0, 24, strParaUstu1);
          ekran.text(0, 30, strParaUstu2);
          ekran.text(0, 36, strParaUstu3);
          ekran.text(36, 42, "BUTON:1 -->>");
          ekran.copyToScreen();
        }

        else if (paraUstu == yuklenen) //TAM PARA ÜSTÜ
        {
          ekran.clear();
          ekran.setFont(2);
          ekran.text(0, 0, sayi);
          ekran.text(0, 6, "ISLEM BASARILI");
          ekran.setFont(0);
          ekran.text(0, 12, strParaUstu);
          ekran.setFont(2);
          ekran.text(0, 21, strParaUstu1);
          ekran.text(0, 28, strParaUstu2);
          ekran.text(0, 35, strParaUstu3);
          ekran.text(36, 42, "BUTON:1 -->>");
          ekran.copyToScreen();
        }
      }
      delay(500);
      while (digitalRead(buton1) == HIGH)
      {
        kalanHizmetlerEkrani();
      }
      delay(500);
      while (digitalRead(buton1) == HIGH)
      {
        kalanKasaEkrani();
        bittiMi=false;
        basarili=false;
        yuklenen = 0;
        secimleriSifirla();
        paralariSifirla();
      }
      delay(500);
      
    }
  }
  else {}
}
