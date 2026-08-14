#include "ShaderSource.h"

#include <random>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <mutex>
#include <random>


GaussianNoise::GaussianNoise(string file_path, float omega) : ShaderSource(file_path),
                                                                    deviation{omega} {}

string GaussianNoise::set_compute_shader_values(string source_file_path){
string comp_shader_source = ShaderSource::get_file_content(source_file_path.c_str());

string st_deviation = "STANDARD_DEVIATION";

int index = comp_shader_source.find(st_deviation);

comp_shader_source = comp_shader_source.replace(index, st_deviation.size(), std::to_string(deviation));


//std::cout << "GAUSSIAN NOISE SHADER : \n" << comp_shader_source << '\n';

return comp_shader_source;

}

/*

uint32_t rot_bits_left_32(uint32_t in, uint32_t s) {

    return (in << s) | (in >> (32 - s)) & 0xFFFF;

}

struct Xoshiro {
    std::vector<uint32_t> s;

    Xoshiro() : s(4) {}
};

uint32_t Xoshiro_32(std::vector<uint32_t>& seed) {
    uint32_t res = rot_bits_left_32(seed[1] * 5, 7) * 9;
    uint32_t t = (seed[1]) << 17;

    seed[2] ^= seed[0];
    seed[3] ^= seed[1];
    seed[1] ^= seed[2];
    seed[0] ^= seed[3];

    seed[2] ^= t;
    seed[3] ^= rot_bits_left_32(seed[3], 45);

    return res;
}

std::vector<uint32_t> generate_rands_xoshiro_32(int num_rands) {
    std::vector<uint32_t> rands(num_rands);
    const auto num_proc = std::thread::hardware_concurrency();
    std::vector<std::thread> thrds(num_proc);
    int num = num_rands % num_proc;
    int num_rands_per_proc = num_rands / num_proc;
    std::cout << "NUM RANDS PER PROC = " << num_rands_per_proc << '\n';
    int mod = num;
    int add = 0;
    if (num) {
        add = num;
    }



    for (int i = 0; i < num_proc; i++) {

        Xoshiro x;
        std::seed_seq s{ 1 + i, 2 + i, 3 + i, 4 + i };
        std::vector<uint32_t> seeds(4);
        s.generate(seeds.begin(), seeds.end());
        x.s = seeds;

        thrds[i] = std::thread([&rands, num_rands_per_proc, add, x, i] mutable {

            int add_0 = (i == 12 - 1) ? add : 0;

            for (int k = 0; k < num_rands_per_proc + add_0; k++) {

                rands[i * num_rands_per_proc + k] = Xoshiro_32(x.s);

            }

            });
    }

    for (int i = 0; i < 12; i++) {

        thrds[i].join();

    }

    return rands;

}

*/