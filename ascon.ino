#include <ASCON.h>

#include <Arduino.h>
#include <ascon/hash.h>
#include <ascon/aead.h>

//imprimir en hexadecimal
void print_hex(const uint8_t *buf, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (buf[i] < 16) Serial.print("0");
    Serial.print(buf[i], HEX);
  }
  Serial.println();
}

// ----------------------------------------------------
// Cifrado ASCON128 AEAD con 16MHZ
// ----------------------------------------------------
float medir_ascon128_aead_grande(size_t msg_len) {

  const size_t BLOQUE = 256;     
  uint8_t key[16] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
  };

  uint8_t nonce[16] = {
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF
  };

  uint8_t ad[3] = {0x10, 0x11, 0x12};
  uint8_t tag[16];

  // mensaje grande
  static uint8_t pt[700];   
  static uint8_t ct[700];

  if (msg_len > sizeof(pt)) msg_len = sizeof(pt);

  for (size_t i = 0; i < msg_len; i++)
    pt[i] = (uint8_t)(i & 0xFF);

  ascon128_state_t st;

  // inicio 
  ascon128_aead_init(&st, nonce, key);
  ascon128_aead_start(&st, ad, sizeof(ad));

  unsigned long t0 = micros();

  //cifrado por bloques
  size_t procesado = 0;
  while (procesado < msg_len) {
    size_t chunk = msg_len - procesado;
    if (chunk > BLOQUE) chunk = BLOQUE;

    ascon128_aead_encrypt_block(&st,
        pt + procesado,
        ct + procesado,
        chunk
    );

    procesado += chunk;
  }

//fucnion para encriptar
  ascon128_aead_encrypt_finalize(&st, tag);

  unsigned long t1 = micros();

  Serial.print("Mensaje cifrado de ");
  Serial.print(msg_len);
  Serial.print(" bytes en ");
  Serial.print((t1 - t0) / 1000.0, 3);
  Serial.println(" ms");

  return (t1 - t0) / 1000.0; // ms
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("\n=== ASCON-128 AEAD ===");

  medir_ascon128_aead_grande(128);    // 128 bytes
  medir_ascon128_aead_grande(256);    // 256 bytes
  medir_ascon128_aead_grande(512);    // 512 bytes
  medir_ascon128_aead_grande(1024);   // 1 KB no tengo espacio pa 1kB pone al final 700B


 
}

void loop() {}
