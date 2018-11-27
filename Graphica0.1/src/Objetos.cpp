#include "Objetos.h"
void Object::dibujar()
{
	glBindVertexArray(this->VAO);
	if (type == 1) //dibujar un prisma cuadrangular
	{
		glDrawElements(GL_TRIANGLE_STRIP, 24, GL_UNSIGNED_INT, 0);
		return;
	}
	if (type == 2) //dibujar una pirámide
	{
		glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
		return;
	}
	if (type == 3) //dibujar una recta
	{
		glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
		return;
	}
}
void Object::definirCubo(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 color)
{
	///ingresar todos los vértices del cubo, determinados a partir de los dos puntos opuestos
	vertices.push_back(ver1.x); //vértice A (pos 0)
	vertices.push_back(ver1.y);
	vertices.push_back(ver1.z);

	vertices.push_back(color.x); //color del cuerpo
	vertices.push_back(color.y);
	vertices.push_back(color.z);

	vertices.push_back(ver2.x); //vértice B (pos 1)
	vertices.push_back(ver2.y);
	vertices.push_back(ver2.z);

	vertices.push_back(color.x); //color del cuerpo
	vertices.push_back(color.y);
	vertices.push_back(color.z);

	vertices.push_back(ver2.x); //vértice C (pos 2)
	vertices.push_back(ver2.y);
	vertices.push_back(ver1.z);

	vertices.push_back(color.x); //color del cuerpo
	vertices.push_back(color.y);
	vertices.push_back(color.z);

	vertices.push_back(ver1.x); //vértice D (pos 3)
	vertices.push_back(ver2.y);
	vertices.push_back(ver2.z);

	vertices.push_back(color.x); //color del cuerpo
	vertices.push_back(color.y);
	vertices.push_back(color.z);

	vertices.push_back(ver2.x); //vértice E (pos 4)
	vertices.push_back(ver1.y);
	vertices.push_back(ver2.z);

	vertices.push_back(color.x); //color del cuerpo
	vertices.push_back(color.y);
	vertices.push_back(color.z);

	vertices.push_back(ver1.x); //vértice F (pos 5)
	vertices.push_back(ver1.y);
	vertices.push_back(ver2.z);

	vertices.push_back(color.x); //color del cuerpo
	vertices.push_back(color.y);
	vertices.push_back(color.z);

	vertices.push_back(ver1.x); //vértice G (pos 6)
	vertices.push_back(ver2.y);
	vertices.push_back(ver1.z);

	vertices.push_back(color.x); //color del cuerpo
	vertices.push_back(color.y);
	vertices.push_back(color.z);

	vertices.push_back(ver2.x); //vértice H (pos 7)
	vertices.push_back(ver1.y);
	vertices.push_back(ver1.z);

	vertices.push_back(color.x); //color del cuerpo
	vertices.push_back(color.y);
	vertices.push_back(color.z);

	///determinar las caras como cuadrados delimitados por cierta combinación de vértices, representados mediante sus posiciones en el arreglo
	///se dibujan mediante TRIANGLE_STRIP, que es un modo de dibujo de OpenGL en el que se cada 3 vértices adyacentes forman un triangulo
	indices.push_back(3); //cara frontal
	indices.push_back(5);
	indices.push_back(1);
	indices.push_back(4);

	indices.push_back(1);//cara derecha
	indices.push_back(4);
	indices.push_back(2);
	indices.push_back(7);

	indices.push_back(6);
	indices.push_back(0);
	indices.push_back(3); //cara izquierda
	indices.push_back(5);

	indices.push_back(7);
	indices.push_back(2);
	indices.push_back(6); //cara trasera
	indices.push_back(0);

	indices.push_back(6);
	indices.push_back(3);
	indices.push_back(2); //cara superior
	indices.push_back(1);

	indices.push_back(5);
	indices.push_back(0);
	indices.push_back(4); //cara inferior
	indices.push_back(7);

	verOrig = vertices;
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size()*sizeof(float), this->vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size()*sizeof(float), this->indices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //definir que elementos son posiciones de vértices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //definir que elementos son color de vértices
	glEnableVertexAttribArray(1);
}

void Object::definirPiramide(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3, glm::vec3 color, float h)
{
	///A = ver1, B = ver2, C = ver3
	///calcular la punta de la pirámide:
	///hallar el punto D, en medio de A y B
	glm::vec3 puntoMedioD = glm::vec3((ver2.x + ver1.x)/2.0f, (ver2.y + ver1.y)/2.0f, (ver2.z + ver1.z)/2.0f);
	std::cout << puntoMedioD.x << " " << puntoMedioD.y << " " << puntoMedioD.z << "\n";
	///hallar el puntoE, en medio de B y C, que es el centro de la base de la pirámide
	glm::vec3 puntoMedioE = glm::vec3((ver3.x + ver2.x) / 2.0f, (ver3.y + ver2.y) / 2.0f, (ver3.z + ver2.z) / 2.0f);
	std::cout << puntoMedioE.x << " " << puntoMedioE.y << " " << puntoMedioE.z << "\n";
	float mEA = (ver1.z- puntoMedioE.z) / (ver1.x - puntoMedioE.x); ///pendiente de la recta EA
	float mDC = (ver3.z - puntoMedioD.z) / (ver3.x - puntoMedioD.x); ///pendiente de la recta DC
	std::cout << mDC << "\n" << mEA << "\n";
	///se obtienen las ecuaciones de las dos rectas, se igualan para hallar el punto medio y se despeja la x.
	float centroX = (puntoMedioE.z - puntoMedioD.z + mDC * puntoMedioD.x - mEA * puntoMedioE.x) / (mDC - mEA); //ver despeje completo en documento de Word
	std::cout << centroX << "\n";
	///sustituir x en alguna de las dos ecuaciones, en este caso, en la recta DC
	float centroZ = mDC * (centroX - puntoMedioD.x) + puntoMedioD.z;
	std::cout << centroZ << "\n";
	///obtener la punta de la pirámide, sumando al punto que está en el centro de la base de la pirámide la altura de la misma
	glm::vec3 punta = glm::vec3(centroX, h, centroZ);

	///introducir los vértices de la pirámide, cada juego de tres valores es una coordenada, ignorando el juego de colores de cada vértice
	vertices.push_back(ver1.x);	//Vértice A (pos 0)
	vertices.push_back(ver1.y);
	vertices.push_back(ver1.z);

	vertices.push_back(color.x); //color
	vertices.push_back(color.y);
	vertices.push_back(color.z);

	vertices.push_back(ver2.x); //Vértice B (pos 1)
	vertices.push_back(ver2.y);
	vertices.push_back(ver2.z);

	vertices.push_back(color.x); //color
	vertices.push_back(color.y); 
	vertices.push_back(color.z);

	vertices.push_back(ver3.x); //Vértice C (pos 2)
	vertices.push_back(ver3.y);
	vertices.push_back(ver3.z);
	 
	vertices.push_back(color.x); //color
	vertices.push_back(color.y);
	vertices.push_back(color.z);

	vertices.push_back(punta.x); //Vértice D o punta de la pirámide (pos 3)
	vertices.push_back(punta.y);
	vertices.push_back(punta.z);

	vertices.push_back(color.x); //color
	vertices.push_back(color.y);
	vertices.push_back(color.z);

	///indicar que vértices delimitan cada cara representandolos por la posición en la que se encuentran
	///Cada tres vertices adyacentes forman un triangulo, utilizando la herramienta TRIANGLE_STRIP de OpenGL
	indices.push_back(1);
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(0);
	verOrig = vertices;
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(float), this->indices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * (sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * (sizeof(float)), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Object::definirRecta(glm::vec3 punto, glm::vec3 vecDireccion, glm::vec3 color)
{
	//calcular cuanto vale t para los límites del espacio (-50 y 50), es decir, cuando x=50 y x=-50
	///despejar t cuando x=50, despeje en documento de word
	float tPos = (50.0f - punto.x) / (vecDireccion.x);
	///sustituir t en ecuaciones parametricas de z y de y
	float pY = punto.y + vecDireccion.y*tPos;
	float pZ = punto.z + vecDireccion.z*tPos;
	///meter el punto recien calculado, es decir, cuando x=50
	vertices.push_back(50.0f);
	vertices.push_back(pY);
	vertices.push_back(pZ);

	vertices.push_back(color.x);
	vertices.push_back(color.y); //color
	vertices.push_back(color.z);

	///despejar t cuando x=-50, despeje en documento de word
	float tNeg = (-50.0f - punto.x) / (vecDireccion.x);
	///sustituir t en ecuaciones parametricas de z y de y
	pY = punto.y + vecDireccion.y*tNeg;
	pZ = punto.z + vecDireccion.z*tNeg;

	///meter el punto recien calculado, es decir, cuando x=-50
	vertices.push_back(-50.0f);
	vertices.push_back(pY);
	vertices.push_back(pZ);

	vertices.push_back(color.x);
	vertices.push_back(color.y); //color
	vertices.push_back(color.z);
	
	indices.push_back(0);
	indices.push_back(1);
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(float), this->indices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * (sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * (sizeof(float)), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Object::editarColor()
{
	for (ti = 3; ti < this->vertices.size(); ti += 6)
	{
		vertices[ti] = color.x;
		vertices[ti+1] = color.y;
		vertices[ti+2] = color.z;
	}
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(float), this->indices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //definir que elementos son posiciones de vértices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //definir que elementos son color de vértices
	glEnableVertexAttribArray(1);
}

void Object::rotar()
{
	float A = cos(anguloRotacionX);
	float B = sin(anguloRotacionX);
	float C = cos(anguloRotacionY);
	float D = sin(anguloRotacionY);
	float E = cos(anguloRotacionZ);
	float F = sin(anguloRotacionZ);
	for (ti = 0; ti < vertices.size(); ti += 6) //rotar respecto a X
	{
		vertices[ti] = C * verOrig[ti] * E + B * D*verOrig[ti + 1] * E + A * D*verOrig[ti + 2] * E - A * F*verOrig[ti + 1] + B * F*verOrig[ti + 2];
		vertices[ti + 1] = A * verOrig[ti + 1] * E - B * verOrig[ti + 2] * E + C * F*verOrig[ti] + B * D*F*verOrig[ti + 1] + A * D*F*verOrig[ti + 2];
		vertices[ti + 2] = D * -1.0f*verOrig[ti] + B * C*verOrig[ti + 1] + A * C*verOrig[ti + 2];
	}
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(float), this->indices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //definir que elementos son posiciones de vértices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //definir que elementos son color de vértices
	glEnableVertexAttribArray(1);
}


void Object::transformar()
{
	float A = cos(anguloRotacionX);
	float B = sin(anguloRotacionX);
	float C = cos(anguloRotacionY);
	float D = sin(anguloRotacionY);
	float E = cos(anguloRotacionZ);
	float F = sin(anguloRotacionZ);
	float xT, yT, zT;
	for (ti = 0; ti < vertices.size(); ti += 6)
	{
		vertices[ti] = ((verOrig[ti] * ex) + tx); //primero se escalan los vertices, luego se suma la traslacion
		vertices[ti + 1] = ((verOrig[ti+1] * ey) + ty); //primero se escalan los vertices, luego se suma la traslacion
		vertices[ti+2] = ((verOrig[ti+2] * ez) + tz); //primero se escalan los vertices, luego se suma la traslacion
		///se rotan las coordenadas ya escaladas y trasladadas
		///se guardan las coordenadas rotadas en variables temporales porque, si se sobreescriben las coordenadas actuales, los puntos cambian y la rotación se hace incorrecta ya que requiere de los puntos "originales"
		xT = C * vertices[ti] * E + B * D*vertices[ti + 1] * E + A * D*vertices[ti + 2] * E - A * F*vertices[ti + 1] + B * F*vertices[ti + 2]; //se rota el elemento
		yT = A * vertices[ti + 1] * E - B * vertices[ti + 2] * E + C * F*vertices[ti] + B * D*F*vertices[ti + 1] + A * D*F*vertices[ti + 2];
		zT = D * -1.0f*vertices[ti] + B * C*vertices[ti + 1] + A * C*vertices[ti + 2];
		///se escriben los valores de las variables temporales con las coordenadas ya calculadas en donde se guardan las coordenadas del vertice real
		vertices[ti] = xT;
		vertices[ti+1] = yT;
		vertices[ti+2] = zT;
	}
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(float), this->indices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //definir que elementos son posiciones de vértices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //definir que elementos son color de vértices
	glEnableVertexAttribArray(1);
}

void Object::traslacion()
{
	for (ti = 0; ti < vertices.size(); ti += 6) 
	{
		vertices[ti] += tx;
		vertices[ti + 1] += ty;
		vertices[ti + 2] += tz;
	}
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(float), this->indices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //definir que elementos son posiciones de vértices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //definir que elementos son color de vértices
	glEnableVertexAttribArray(1);
}

void Object::escalar()
{
	for (ti = 0; ti < vertices.size(); ti += 6)
	{
		vertices[ti] = verOrig[ti] * ex;
		vertices[ti+1] = verOrig[ti+1] * ey;
		vertices[ti+2] = verOrig[ti+2] * ez;
	}
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(float), this->indices.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //definir que elementos son posiciones de vértices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //definir que elementos son color de vértices
	glEnableVertexAttribArray(1);
}