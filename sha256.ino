#include <SHA256.h>

// ==================================================
// SIMULACIÓN RAM/STORAGE SEGÚN EL ARTÍCULO
// (solo conceptual, NO afecta al tiempo real)
// ==================================================

// 8 MHz → 60 KB RAM / 60 KB Storage
void simular_8MHz() {
    delay(10);
}

// 20 MHz → 18 KB RAM / 256 KB Storage
void simular_20MHz() {
    delay(20);
}

// 30 MHz Java SAlg / Multos
void simular_30MHz_JavaSAlg() {
    delay(40);
}

// ==================================================
// MEDIR SHA-256 REAL EN ARDUINO NANO (16 MHz)
// Mensaje: 4256 bits = 532 bytes (paper)
// ==================================================

float medirSHA256_16MHz_ms() {
    const size_t MSG_LEN = 532;        // 4256 bits
    static uint8_t data[MSG_LEN];
    memset(data, 0x55, sizeof(data));

    SHA256 sha;
    uint8_t hash[32];

    unsigned long t1 = micros();

    sha.reset();
    sha.update(data, MSG_LEN);
    sha.finalize(hash, sizeof(hash));

    unsigned long t2 = micros();

    return (t2 - t1) / 1000.0;         // ms reales
}



float escalar(float t_ms_16, float mhz) {
    return t_ms_16 * (16.0 / mhz);
}

// Ajuste empírico Java Card / Multos (SHA-2)
float factor_java_sha2 = 0.72;



void setup() {
    Serial.begin(115200);
    delay(1500);

    Serial.println("=== SHA-256 ===");

    float t16 = medirSHA256_16MHz_ms();

    simular_8MHz();
    float t8  = escalar(t16, 8);

    simular_20MHz();
    float t20 = escalar(t16, 20);

    simular_30MHz_JavaSAlg();
    float t30 = escalar(t16, 30);
    t30 *= factor_java_sha2;

  
    Serial.println();
    Serial.println("--- Resultados SHA-256 ---");

    Serial.print("SHA-256 @ 16 MHz (Arduino real): ");
    Serial.print(t16, 3);
    Serial.println(" ms");

    Serial.print("SHA-256 @ 8 MHz  : ");
    Serial.print(t8, 3);
    Serial.println(" ms");

    Serial.print("SHA-256 @ 20 MHz : ");
    Serial.print(t20, 3);
    Serial.println(" ms");

    Serial.print("SHA-256 @ 30 MHz (Java/Multos): ");
    Serial.print(t30, 3); // no da los valores correctos
    Serial.println(" ms");

    
}

void loop() {}
