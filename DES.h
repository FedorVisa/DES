#include <iostream>
#include "des_key.h"
#include "des_tables.h"

#ifndef DES_DES_H
#define DES_DES_H

class DES_cipher {

private:
    //"Airplane.bmp";
    uint64_t block;
    uint64_t subKey[16];
public:
    DES_cipher(uint64_t key);
    void key_generation(uint64_t key);
    void feistel (uint32_t left, uint32_t right, uint32_t f);
    uint32_t f(uint32_t right, uint64_t key);
    uint64_t ip(uint64_t block);
    uint64_t fp(uint64_t block);
    uint64_t des(uint64_t block, bool mode);
    uint64_t encrypt(uint64_t block);
    uint64_t decrypt(uint64_t block);

};




#endif //DES_DES_H
