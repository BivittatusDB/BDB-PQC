#include "sha3.h"

int main() {
    // Test data
    const char* test_data = "Hello, SHA-3!";
    size_t data_len = sizeof(test_data) - 1;

    // Buffers for hash outputs
    uint8_t hash_256[32];
    uint8_t hash_512[64];
    uint8_t shake128_output[32];
    uint8_t shake256_output[64];

    // For reference, this should be ce4ab23ba79eba9ba2531220a647080bed52619b752df89a0a67fd5973d667f1
    // Test SHA3-256
    sha3_256((const uint8_t*)test_data, data_len, hash_256);
    printf("SHA3-256: ");
    print_hex(hash_256, 32);

    // Test SHA3-512
    sha3_512((const uint8_t*)test_data, data_len, hash_512);
    printf("SHA3-512: ");
    print_hex(hash_512, 64);

    // Test SHAKE128 with 32-byte output
    shake128((const uint8_t*)test_data, data_len, shake128_output, 64);
    printf("SHAKE128 (64 bytes): ");
    print_hex(shake128_output, 64);

    // Test SHAKE256 with 64-byte output
    shake256((const uint8_t*)test_data, data_len, shake256_output, 64);
    printf("SHAKE256 (64 bytes): ");
    print_hex(shake256_output, 64);

    return 0;
}
