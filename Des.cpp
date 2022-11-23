#include "DES.h"
#include "des_key.h"
#include "des_tables.h"

    DES_cipher::DES_cipher(uint64_t key)
    {
        key_generation(key);
    }

    uint64_t DES_cipher::encrypt(uint64_t block)
    {
        return des(block, false);
    }

    uint64_t DES_cipher::decrypt(uint64_t block)
    {
        return des(block, true);
    }


    uint64_t DES_cipher::des(uint64_t block, bool mode)
    {

        block = ip(block);


        uint32_t left = (uint32_t) (block >> 32) & L64_MASK;
        uint32_t right = (uint32_t) (block & L64_MASK);


        for (uint8_t i = 0; i < 16; i++)
        {
            uint32_t func = mode ? f(right, subKey[15-i]) : f(right, subKey[i]);
            feistel(left, right, func);
        }


        block = (((uint64_t) right) << 32) | (uint64_t) left;

        return fp(block);
    }

    void DES_cipher::key_generation(uint64_t key) {

        uint64_t permuted_choice_1 = 0;
        for (uint8_t  i = 0; i < 56; i++)
        {
            permuted_choice_1 <<= 1;
            permuted_choice_1 |= (key >> (64-PC1[i])) & LB64_MASK;
        }

        uint32_t C = (uint32_t) ((permuted_choice_1 >> 28) & 0x000000000fffffff);
        uint32_t D = (uint32_t)  (permuted_choice_1 & 0x000000000fffffff);


        for (uint8_t i = 0; i < 16; i++) {


            for (uint8_t j = 0; j < ITERATION_SHIFT[i]; j++) {

                C = (0x0fffffff & (C << 1)) | (0x00000001 & (C >> 27));
                D = (0x0fffffff & (D << 1)) | (0x00000001 & (D >> 27));
            }

            uint64_t permuted_choice_2 = (((uint64_t) C) << 28) | (uint64_t) D;

            subKey[i] = 0;
            for (uint8_t j = 0; j < 48; j++) {

                subKey[i] <<= 1;
                subKey[i] |= (permuted_choice_2 >> (56 - PC2[j])) & LB64_MASK;
            }
        }
    }

    uint64_t DES_cipher::ip(uint64_t block){


    uint64_t result = 0;
    for (uint8_t i = 0; i < 64; i++)
    {
        result <<= 1;
        result |= (block >> (64-IP[i])) & LB64_MASK;
    }
    return result;
}

    uint64_t DES_cipher::fp(uint64_t block){


    uint64_t result = 0;
    for (uint8_t i = 0; i < 64; i++)
    {
        result <<= 1;
        result |= (block >> (64-FP[i])) & LB64_MASK;
    }
    return result;
}

    uint32_t DES_cipher::f(uint32_t right,uint64_t key){
    uint64_t s_input = 0;
    for (uint8_t i = 0; i < 48; i++)
    {
        s_input <<= 1;
        s_input |= (uint64_t) ((right >> (32-EXPANSION[i])) & LB32_MASK);
    }


    s_input = s_input ^ key;


    uint32_t s_output = 0;
    for (uint8_t i = 0; i < 8; i++)
    {

        char row = (char) ((s_input & (0x0000840000000000 >> 6*i)) >> (42-6*i));
        row = (row >> 4) | (row & 0x01);


        char column = (char) ((s_input & (0x0000780000000000 >> 6*i)) >> (43-6*i));

        s_output <<= 4;
        s_output |= (uint32_t) (SBOX[i][16*row + column] & 0x0f);
    }


    uint32_t f_result = 0;
    for (uint8_t i = 0; i < 32; i++)
    {
        f_result <<= 1;
        f_result |= (s_output >> (32 - PBOX[i])) & LB32_MASK;
    }

    return f_result;

}


void DES_cipher::feistel (uint32_t left, uint32_t right, uint32_t f) {
    
    uint32_t temp = right;
    right = left ^ f;
    left = temp;
}