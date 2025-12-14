#include <Crypto.h>
#include <ASCON.h>

// ==================================================
// DATOS DE PRUEBA (mensajes MEDIANOS)
// ==================================================

#define MSG_LEN 64   // 64 bytes → seguro para Nano

uint8_t message[MSG_LEN];
uint8_t hash[32];    // ASCON-HASH = 256 bits

// ASCON-128
uint8_t key[16] = {
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
  0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
};

uint8_t nonce[16] = {
  0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
  0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF
};

uint8_t ciphertext[MSG_LEN];
uint8_t tag[16];

// ==================================================
// MEDICIONES
// ==================================================

float medir_ascon_hash_ms() {
  ASCONHash hashFunc;

  unsigned long t1 = micros();
  hashFunc.reset();
  hashFunc.update(message, MSG_LEN);
  hashFunc.finalize(hash, sizeof(hash));
  unsigned long t2 = micros();

  return (t2 - t1) / 1000.0;
}

float medir_ascon128_encrypt_ms() {
  ASCON128 ascon;

  unsigned long t1 = micros();
  ascon.setKey(key, sizeof(key));
  ascon.setIV(nonce, sizeof(nonce));
  ascon.encrypt(ciphertext, message, MSG_LEN);
  ascon.computeTag(tag, sizeof(tag));
  unsigned long t2 = micros();

  return (t2 - t1) / 1000.0;
}

// ==================================================

void setup() {
  Serial.begin(115200);
  delay(1500);

  // Rellenar mensaje
  for (int i = 0; i < MSG_LEN; i++) {
    message[i] = i;
  }

  Serial.println("=== ASCON PRACTICA (Arduino Nano 16 MHz) ===");

  float t_hash = medir_ascon_hash_ms();
  float t_enc  = medir_ascon128_encrypt_ms();

  Serial.println();
  Serial.println("--- Resultados reales ---");

  Serial.print("ASCON-HASH @16 MHz : ");
  Serial.print(t_hash, 3);
  Serial.println(" ms");

  Serial.print("ASCON-128 Encrypt @16 MHz : ");
  Serial.print(t_enc, 3);
  Serial.println(" ms");

  Serial.println("\nListo.");
}

void loop() {}
