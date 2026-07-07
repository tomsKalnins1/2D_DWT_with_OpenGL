#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "Texture.h"
#include "vector"

using std::string, std::string_view, std::map, std::ostream, std::vector;

class Wavelet {

public:

	enum class Wavelets {

		HAAR = 'h'

	};
	Wavelet::Wavelets name;
	Texture input_img;
	vector<float>  low_pass_h0();
	vector<float>  high_pass_h1();
	vector<float>  low_pass_g0();
	vector<float>  high_pass_g1();

	

//	Wavelet() = delete;
	Wavelet(string path_to_img, Wavelet::Wavelets name, int support);
	//create texture
	//set support

	virtual void compute_filter() =0;

	friend ostream& operator<<(ostream& out, const Wavelets w);
};

ostream& operator<<(ostream& out, const Wavelet:: Wavelets w) {

	static auto names = [] {

		map<Wavelet::Wavelets, std::string_view> result;
#define INSERT_ELEM(p) result.emplace(p, #p);
		INSERT_ELEM(Wavelet::Wavelets::HAAR);
#undef INSERT_ELEM;
		return result;

		}();

	return out << names[w];

}


class Haar : public Wavelet {

public:

	Wavelets type = Wavelets::HAAR;

	

};