#include <string>

class Shader {
public:

	Shader();

	void loadShader(const std::string& vertexPath, const std::string& fragmentPath);
	unsigned int getProgramHandle() const { return programHandle; }
private:
	unsigned int programHandle;
};