#include "DES.h"

#include "bitmap/bitmap_image.hpp"
#include <ctime>
#include <iostream>
#include <random>


int main() {
    //load picture
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

  //  uint64_t keyText = static_cast<uint64_t>(0x0f5ab26f3fff3ff1);
    uint64_t keyText = dis(gen);
    std::ofstream filekey("keys.txt");
    filekey << keyText;
    DES_cipher deso(keyText);
    std::string status;
    bool encrypt = false;
    std::cout << "enter des regime encryption - e decryption - d ";
    std::cin >> status;
    if(status=="e") encrypt = true;
   // std::cout << status << " " << encrypt;

    //"Airplane.bmp";
    bitmap_image pic("res.bmp");
    if (!pic) {
        return -1;
    }
    bitmap_image pic_crypt(pic.width(), pic.height());


    time_t start = time(nullptr);


    uint64_t bytes = (uint64_t)pic.width() * pic.height() * 3;
    uint64_t blocks_num = bytes / 8;
    unsigned short excess = bytes % 8;
    if (excess > 0) {
        blocks_num++;
    }
    uint8_t** vec8 = new uint8_t*[blocks_num];
    for (uint64_t i = 0; i < blocks_num; i++) {
        vec8[i] = new uint8_t[8];
    }
    if (excess > 0) {
        for (int i = excess; i < 8; i++) vec8[blocks_num - 1][i] = 0;
    }
    rgb_t pixel;

    for (auto y = 0; y < pic.height(); y++) {
        for (auto x = 0; x < pic.width(); x++) {
            pixel = pic.get_pixel(x, y);
            uint32_t coord = (y * pic.width() + x)*3;
            for (auto t = 0; t < 3; t++) {
                switch (t) {
                    case 0:
                        vec8[(coord + t) / 8][(coord + t) % 8] = pixel.red;
                        break;
                    case 1:
                        vec8[(coord + t) / 8][(coord + t) % 8] = pixel.green;
                        break;
                    case 2:
                        vec8[(coord + t) / 8][(coord + t) % 8] = pixel.blue;
                        break;
                    default:
                        break;
                }
            }

        }
    }
    std::cout << "\n";
    pic.clear();


    time_t end = time(nullptr) - start;
    if ((end%60) < 10) std::cout << "time: " << end/60 << ":0" << end % 60 << "\n";
    else std::cout << "time: " << end / 60 << ":" << end % 60 << "\n";


    uint64_t iv = static_cast<uint64_t>(0x0f14526f3fff3ff1);
    uint64_t vector_cfb =static_cast<uint64_t>(0x0000000000000000); ;
    uint64_t encrypt_cfb;


    if(encrypt) {
        for (auto i = 0; i < blocks_num; i++) {

            iv = deso.encrypt(iv);
            for (auto j = 7; j >= 0; j--) {
                vector_cfb = vector_cfb << 8 | (vec8[i][7 - j]);
            }

            iv = vector_cfb = iv ^ vector_cfb;

            for (auto j = 7; j >= 0; j--) {
                vec8[i][j] = (vector_cfb & static_cast<uint64_t>(255) << (8 * (7 - j))) >> (8 * (7 - j));
            }


        }
    } else {  //UwU
        std::ifstream from_file("keys.txt");
        from_file >> keyText;

        for (auto i = 0; i < blocks_num; i++) {

            iv = deso.decrypt(iv);
            for (auto j = 7; j >= 0; j--) {
                vector_cfb = vector_cfb << 8 | (vec8[i][7 - j]);
            }

            encrypt_cfb = vector_cfb;
            vector_cfb = iv ^ vector_cfb;

            for (auto j = 7; j >= 0; j--) {
                vec8[i][j] = (vector_cfb & static_cast<uint64_t>(255) << (8 * (7 - j))) >> (8 * (7 - j));
            }
            iv = encrypt_cfb;

        }
    }



    for (auto y = 0; y < pic_crypt.height(); y++) {
        for (auto x = 0; x < pic_crypt.width(); x++) {
            uint32_t coord = (y * pic_crypt.width() + x) * 3;
            for (auto t = 0; t < 3; t++) {
                switch (t) {
                    case 0:
                        pixel.red = vec8[(coord + t) / 8][(coord + t) % 8];
                        break;
                    case 1:
                        pixel.green = vec8[(coord + t) / 8][(coord + t) % 8];
                        break;
                    case 2:
                        pixel.blue = vec8[(coord + t) / 8][(coord + t) % 8];
                        break;
                    default:
                        break;
                }
            }
            pic_crypt.set_pixel(x, y, pixel);
            //std::cout << "step: " << coord/3 << "\\" << pic.width() * pic.height() << "\r";
        }
    }

    pic_crypt.save_image("res1.bmp");
    printf("\a");
    return 0;
}