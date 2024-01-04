#ifndef LIGHT_H
#define LIGHT_H

#include "headers.h"


// VertexP Declarations.
struct VertexP
{
	VertexP() { position = glm::vec3(0.0f, 0.0f, 0.0f); }
	VertexP(glm::vec3 p) { position = p; }
	glm::vec3 position;
};

// PointLight Declarations.
class PointLight
{
public:
	// PointLight Public Methods.
	PointLight() {
		position = glm::vec3(1.5f, 1.5f, 1.5f);
		intensity = glm::vec3(1.0f, 1.0f, 1.0f);
		CreateVisGeometry();
	}
	PointLight(const glm::vec3 p, const glm::vec3 I) {
		position = p;
		intensity = I;
		CreateVisGeometry();
	}

	glm::vec3 GetPosition()  const { return position;  }
	glm::vec3 GetIntensity() const { return intensity; }
	
	void Draw() {
		glPointSize(16.0f);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexP), 0);
		glDrawArrays(GL_POINTS, 0, 1);
		glDisableVertexAttribArray(0);
		glPointSize(1.0f);
	}

	void MoveLeft (const float moveSpeed) { position += moveSpeed * glm::vec3(-0.1f,  0.0f, 0.0f); }
	void MoveRight(const float moveSpeed) { position += moveSpeed * glm::vec3( 0.1f,  0.0f, 0.0f); }
	void MoveUp   (const float moveSpeed) { position += moveSpeed * glm::vec3( 0.0f,  0.1f, 0.0f); }
	void MoveDown (const float moveSpeed) { position += moveSpeed * glm::vec3( 0.0f, -0.1f, 0.0f); }

protected:
	// PointLight Protected Methods.
	void CreateVisGeometry() {
		VertexP lightVtx = glm::vec3(0, 0, 0);
		const int numVertex = 1;
		glGenBuffers(1, &vboId);
		glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VertexP) * numVertex, &lightVtx, GL_STATIC_DRAW);
	}

	// PointLight Private Data.
	GLuint vboId;
	glm::vec3 position;
	glm::vec3 intensity;
};



// SpotLight Declarations.
class SpotLight : public PointLight
{
public:
	// SpotLight Public Methods.
	SpotLight() {
		position = glm::vec3(0.0f, 2.0f, 0.0f);
		intensity = glm::vec3(1.0f, 1.0f, 1.0f);
		direction = glm::vec3(0.0f);
		position = glm::vec3(0.0f, 2.0f, 0.0f);
		intensity = glm::vec3(1.0f, 1.0f, 1.0f);
		// -------------------------------------------------------
		// Add your initialization code here.
		// -------------------------------------------------------
		CreateVisGeometry();
	}
	SpotLight(const glm::vec3 p, const glm::vec3 I, const glm::vec3 D, const float cutoffDeg, const float totalWidthDeg) {
		position = p;
		intensity = I;
		direction = D;
		cutoff = cutoffDeg;
		totalWidthD = totalWidthDeg;
		// -------------------------------------------------------
		// Add your initialization code here.
		// -------------------------------------------------------
		CreateVisGeometry();
	}
	glm::vec3 GetDirection() const { return direction; }
	float GetCutoff() const { return cutoff; }
	float GetTotalWidthD() const { return totalWidthD; }

	// -------------------------------------------------------
	// Add your methods or data here.
	// -------------------------------------------------------

private:
	glm::vec3 direction;
	float cutoff;
	float totalWidthD;
	// SpotLight Public Data.
	// -------------------------------------------------------
	// Add your methods or data here.
	// -------------------------------------------------------
};



// DirectionalLight Declarations.
class DirectionalLight
{
public:
	// DirectionalLight Public Methods.
	DirectionalLight() {
		direction = glm::vec3(1.5f, 1.5f, 1.5f);
		radiance = glm::vec3(1.0f, 1.0f, 1.0f);
	};
	DirectionalLight(const glm::vec3 dir, const glm::vec3 L) {
		direction = glm::normalize(dir);
		radiance = L;
	}

	glm::vec3 GetDirection() const { return direction; }
	glm::vec3 GetRadiance()  const { return radiance;  }

private:
	// DirectionalLight Private Data.
	glm::vec3 direction;
	glm::vec3 radiance;
};

#endif
