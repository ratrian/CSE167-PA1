#include "PointCloud.h"

PointCloud::PointCloud(std::string objFilename, GLfloat pointSize)
	: pointSize(pointSize)
{
	/*
	 * TODO: Section 2: Currently, all the points are hard coded below.
	 * Modify this to read points from an obj file.
	 */

	std::ifstream objFile(objFilename); // The obj file we are reading.

	// Check whether the file can be opened.
	if (objFile.is_open())
	{
		std::string line; // A line in the file.

		// Read lines from the file.
		while (std::getline(objFile, line))
		{
			// Turn the line into a string stream for processing.
			std::stringstream ss;
			ss << line;

			// Read the first word of the line.
			std::string label;
			ss >> label;

			// If the line is about vertex (starting with a "v").
			if (label == "v")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 point;
				ss >> point.x >> point.y >> point.z;

				// Process the point.
				points.push_back(point);
			}
		}
	}
	else
		std::cerr << "Can't open the file " << objFilename << std::endl;

	objFile.close();

	/*
	 * TODO: Section 4, you will need to normalize the object to fit in the
	 * screen.
	 */

	GLfloat minX = points[0].x;
	GLfloat maxX = points[0].x;
	GLfloat minY = points[0].y;
	GLfloat maxY = points[0].y;
	GLfloat minZ = points[0].z;
	GLfloat maxZ = points[0].z;

	int numPoints = points.size();

	for (int i = 0; i < numPoints; i++) {
		if (minX > points[i].x)
			minX = points[i].x;
		if (maxX < points[i].x)
			maxX = points[i].x;
		if (minY > points[i].y)
			minY = points[i].y;
		if (maxY < points[i].y)
			maxY = points[i].y;
		if (minZ > points[i].z)
			minZ = points[i].z;
		if (maxZ < points[i].z)
			maxZ = points[i].z;
	}

	GLfloat centX = (minX + maxX) / 2;
	GLfloat centY = (minY + maxY) / 2;
	GLfloat centZ = (minZ + maxZ) / 2;

	for (int i = 0; i < numPoints; i++) {
		points[i].x -= centX;
		points[i].y -= centY;
		points[i].z -= centZ;
	}

	GLfloat maxDist = sqrt((points[0].x) * (points[0].x) + (points[0].y) * (points[0].y) + (points[0].z) * (points[0].z));

	for (int i = 0; i < numPoints; i++) {
		if (maxDist < sqrt((points[i].x) * (points[i].x) + (points[i].y) * (points[i].y) + (points[i].z) * (points[i].z)))
			maxDist = sqrt((points[i].x) * (points[i].x) + (points[i].y) * (points[i].y) + (points[i].z) * (points[i].z));
	}

	for (int i = 0; i < numPoints; i++) {
		points[i].x *= 9.5 / maxDist;
		points[i].y *= 9.5 / maxDist;
		points[i].z *= 9.5 / maxDist;
	}

	// Set the model matrix to an identity matrix. 
	model = glm::mat4(1);

	// Set the color. 
	color = glm::vec3(1, 0, 0);

	// Generate a Vertex Array (VAO) and Vertex Buffer Object (VBO)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind VAO
	glBindVertexArray(VAO);

	// Bind VBO to the bound VAO, and store the point data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), points.data(), GL_STATIC_DRAW);
	// Enable Vertex Attribute 0 to pass point data through to the shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

PointCloud::~PointCloud() 
{
	// Delete the VBO and the VAO.
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void PointCloud::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader)
{
	// Actiavte the shader program 
	glUseProgram(shader);

	// Get the shader variable locations and send the uniform data to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));
	glUniform1f(glGetUniformLocation(shader, "pointSize"), pointSize);

	// Bind the VAO
	glBindVertexArray(VAO);

	// Set point size
	glPointSize(pointSize);

	// Draw the points 
	glDrawArrays(GL_POINTS, 0, points.size());

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

void PointCloud::update()
{
	// Spin the cube by 1 degree
	spin(0.1f);
}

void PointCloud::updatePointSize(GLfloat size) 
{
	/*
	 * TODO: Section 3: Implement this function to adjust the point size.
	 */
	pointSize = size;
}

void PointCloud::spin(float deg)
{
	// Update the model matrix by multiplying a rotation matrix
	model = model * glm::rotate(glm::radians(deg), glm::vec3(0.0f, 1.0f, 0.0f));
}
