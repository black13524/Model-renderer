#include"trianglemesh.h"
#include<iostream>
#include<array>
#include <fstream> 
#include<sstream>
#include<unordered_set>
#include<algorithm>
// Constructor of a triangle mesh.
TriangleMesh::TriangleMesh()
{
	// -------------------------------------------------------
	// Add your initialization code here.
	// -------------------------------------------------------
}

// Destructor of a triangle mesh.
TriangleMesh::~TriangleMesh()
{
	// -------------------------------------------------------
	// Add your release code here.
	// -------------------------------------------------------
	subMeshes.clear();
	vertices.clear();
}
static void gl_log_error(const char* func, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cerr << "[OpenGL Error] (" << error << "): " << func << " " << file
			<< ": " << line << std::endl;
	}
}

#define GL_CALL(x)                                                             \
  do {                                                                         \
    x;                                                                         \
    gl_log_error(#x, __FILE__, __LINE__);                                      \
  } while (0)
void TriangleMesh::loadmtl(const std::string& filePath) {
	//std::cout << "dlkfsjlkjsd" << std::endl;
	//std::cout << filePath << std::endl;
	std::ifstream file(filePath.c_str());
	if (!file.is_open()) {
		std::cout << "Error: Cannot open file " << filePath << std::endl;
		return;
	}
	std::string line;
	std::string name;
	int index;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string type;
		iss >> type;
		if (type == "newmtl") {
			iss >> name;
			for (int i = 0; i < subMeshes.size(); i++) {
				if (subMeshes[i].material->GetName() == name) {
					index = i;
					//subMeshes[i].first = true;
				}
			}
			std::cout << index << std::endl;
		}
		else if (type == "Ka") {
			float kax, kay, kaz;
			iss >> kax >> kay >> kaz;
			subMeshes[index].material->SetKa(glm::vec3(kax, kay, kaz));
		}
		else if (type == "Kd") {
			float kdx, kdy, kdz;
			iss >> kdx >> kdy >> kdz;
			subMeshes[index].material->SetKd(glm::vec3(kdx, kdy, kdz));
		}
		else if (type == "Ks") {
			float ksx, ksy, ksz;
			iss >> ksx >> ksy >> ksz;
			subMeshes[index].material->SetKs(glm::vec3(ksx, ksy, ksz));
		}
		else if (type == "Ns") {
			float ns;
			iss >> ns;
			subMeshes[index].material->SetNs(ns);
		}
		else if (type == "map_Kd") {
			std::string mapKd;
			iss >> mapKd;
			int pos = filePath.find_last_of('/');
			std::string mapPath = filePath.substr(0, pos + 1) + mapKd;
			//std::cout << mapPath << std::endl;
			ImageTexture* tex = new ImageTexture(mapPath);
			subMeshes[index].material->SetMapKd(tex);
			//std::cout << index << std::endl;
		}
	}


}

void TriangleMesh::Draw(PhongShadingDemoShaderProg* phongShadingShader)
{
	for (int i = 0; i < subMeshes.size(); i++)
	{
		// Material properties.
		glUniform3fv(phongShadingShader->GetLocKa(), 1, glm::value_ptr(subMeshes[i].material->GetKa()));
		glUniform3fv(phongShadingShader->GetLocKs(), 1, glm::value_ptr(subMeshes[i].material->GetKs()));
		glUniform1f(phongShadingShader->GetLocNs(), subMeshes[i].material->GetNs());
		if (subMeshes[i].material->GetMapKd() != nullptr) {
			subMeshes[i].material->GetMapKd()->Bind(GL_TEXTURE0);
			glUniform1i(phongShadingShader->GetLocMapKd(), 0);
			subMeshes[i].material->SethaveMapKd(1);
			glUniform1f(phongShadingShader->GetLocHaveMapKd(), subMeshes[i].material->GethaveMapKd());
		}

			glUniform3fv(phongShadingShader->GetLocKd(), 1, glm::value_ptr(subMeshes[i].material->GetKd()));
		
		// Get the submesh material.
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), 0);
		GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)12));
		GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTN), (const GLvoid*)24));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[i].iboId);
		glDrawElements(GL_TRIANGLES, subMeshes[i].vertexIndices.size(), GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		
	}
}

unsigned int TriangleMesh::findPTN(VertexPTN ptn) {

	for (int i = 0; i < vertices.size(); i++) {
		if (ptn == vertices[i]) {
			return i;
		}
	}
	vertices.push_back(ptn);
	return vertices.size() - 1;
}
// Load the geometry and material data from an OBJ file.
bool TriangleMesh::LoadFromFile(const std::string& filePath, const bool normalized)
{	
	std::ifstream file(filePath);
	
	if (!file.is_open())
	{
		std::cerr << "Failed to open the file: " << filePath << std::endl;
		return false;
	}
	std::string mtllib;
	std::string line;
	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec3> tempNormals;
	std::vector<glm::vec2> tempTexCoords;
	std::vector<unsigned int> tempIndices;
	std::vector<PhongMaterial*>tempMaterial;
	glm::vec3 minma = glm::vec3(1e10f, 1e10f, 1e10f);
	glm::vec3 maxma = glm::vec3(-1e10f, -1e10f, -1e10f);
	glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
	bool ismtl = 0;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string type;
		if (!(iss >> type)) {
			continue;
		}
		if (type == "v") {
			float x, y, z;
			iss >> x >> y >> z;
			maxma.x = std::max(maxma.x, x);
			maxma.y = std::max(maxma.y, y);
			maxma.z = std::max(maxma.z, z);
			minma.x = std::min(minma.x, x);
			minma.y = std::min(minma.y, y);
			minma.z = std::min(minma.z, z);
			//std::cout << x << std::endl;
			tempVertices.push_back(glm::vec3(x, y, z));
			numVertices++;
		}
		else if (type == "vn") {
			float nx, ny, nz;
			iss >> nx >> ny >> nz;
			tempNormals.push_back(glm::vec3(nx, ny, nz));
		}
		else if (type == "vt") {
			float u, v;
			iss >> u >> v;
			tempTexCoords.push_back(glm::vec2(u, v));
			//std::cout << u << ' ' << v << std::endl;
		}
		else if (type == "usemtl") {
			std::string materialName;
			iss >> materialName;
			//std::cout << materialName << std::endl;
			PhongMaterial* material = new PhongMaterial();
			material->SetName(materialName);
			tempMaterial.push_back(material);
			//numOfSubMesh.resize(numSubMeshes + 10);
			//subMeshes[numSubMeshes].material->SetName(materialName);
			if (ismtl) {
				//std::cout << numSubMeshes << "dkfj" << std::endl;
				bool exist = 0;
				subMeshes.push_back(SubMesh());
				subMeshes[numSubMeshes].vertexIndices = tempIndices;
				/*
				for (auto& i : tempIndices) {
					std::cout << i << std::endl;

				}
				std::cout << " hufhdsk" << std::endl;
				*/
				tempIndices.clear();
				numSubMeshes++;
			}
			ismtl = 1;
		}
		else if (type == "f") {
			//numOfSubMesh[numSubMeshes]++;
			std::vector<int*> face;
			std::string vertex;
			int pnt_cnt = 0;
			while (iss >> vertex) {
				std::replace(vertex.begin(), vertex.end(), '/', ' ');
				std::istringstream vertexStream(vertex);
				int* vertexIndex = new int[3];
				vertexStream >> vertexIndex[0] >> vertexIndex[1] >> vertexIndex[2];
				vertexIndex[0]--; vertexIndex[1]--; vertexIndex[2]--;
				//std::cout << vertexIndex[0] << " " << vertexIndex[1] << " " << vertexIndex[2] << std::endl;
				unsigned int index = findPTN(VertexPTN(tempVertices[vertexIndex[0]], tempNormals[vertexIndex[2]], tempTexCoords[vertexIndex[1]]));
				//std::cout << index << std::endl;
				if (pnt_cnt < 3)
					tempIndices.push_back(index);
				else {
					tempIndices.push_back(*(tempIndices.end() - 3));
					tempIndices.push_back(*(tempIndices.end() - 2));
					tempIndices.push_back(index);
					numTriangles++;
				}
				pnt_cnt++;
				//face.push_back(vertexIndex);
			}
			numTriangles++;

		}
		else if (type == "mtllib") {
			iss >> mtllib;
		}
	}
	subMeshes.push_back(SubMesh());
	subMeshes[numSubMeshes].vertexIndices = tempIndices;
	numSubMeshes++;
	for (int i = 0; i < numSubMeshes; i++) {
		subMeshes[i].material = tempMaterial[i];
	}
	if (mtllib != "default") {
		int pos = filePath.find_last_of('/');
		std::string mtlPath = filePath.substr(0, pos + 1) + mtllib;
		loadmtl(mtlPath);
	}
	file.close();

	if (normalized) {
		objCenter = maxma + minma;
		objCenter /= 2.0f;
		objExtent = maxma - minma;
		for (int i = 0; i < vertices.size(); i++) {
			vertices[i].position -= objCenter;
			vertices[i].position /= std::max(objExtent.x, std::max(objExtent.y, objExtent.z));
		}
	}

	return true;
}
void TriangleMesh::createBuffer() {
	//std::cout << "fdkjsj" << std::endl;
	GL_CALL(glGenBuffers(1, &vboId));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vboId));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPTN) * vertices.size(), std::data(vertices), GL_STATIC_DRAW));
	for (int i = 0; i < subMeshes.size(); i++) {
		GL_CALL(glGenBuffers(1, &subMeshes[i].iboId));
		GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMeshes[i].iboId));
		GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * subMeshes[i].vertexIndices.size(), std::data(subMeshes[i].vertexIndices), GL_STATIC_DRAW));
	}

}

// Show model information.
void TriangleMesh::ShowInfo()
{
	std::cout << "# Vertices: " << numVertices << std::endl;
	std::cout << "# Triangles: " << numTriangles << std::endl;
	std::cout << "Total " << subMeshes.size() << " subMeshes loaded" << std::endl;
	for (unsigned int i = 0; i < subMeshes.size(); ++i) {
		const SubMesh& g = subMeshes[i];
		std::cout << "SubMesh " << i << " with material: " << g.material->GetName() << std::endl;
		std::cout << "Num. triangles in the subMesh: " << g.vertexIndices.size() / 3 << std::endl;
	}
	std::cout << "Model Center: " << objCenter.x << ", " << objCenter.y << ", " << objCenter.z << std::endl;
	std::cout << "Model Extent: " << objExtent.x << " x " << objExtent.y << " x " << objExtent.z << std::endl;
}

