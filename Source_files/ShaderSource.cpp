#include "../Header_files/ShaderSource.h"




string ShaderSource::get_file_content(string file_path) {
	
	std::ifstream in(file_path.c_str(), std::ios::binary);

	if (in) {
		
		string content;

		in.seekg(0, std::ios::end);
		content.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&content[0], content.size());
		in.close();
		
		return content;
	}

	throw(errno);

}
