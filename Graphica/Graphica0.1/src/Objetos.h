#ifndef OBJETOS_H
#define OBJETOS_H
#include<glad/glad.h>
#include<vector>
#include<set>
#include<glm/glm.hpp>
#include "Shader.h"
	//std::set<std::pair<float, std::pair<float, float>>>colorMarks; //guarda el color ya utilizado
	class Object
	{
	public:
		float anguloRotacionX,anguloRotacionY,anguloRotacionZ;
		float tx, ty, tz;
		float ex, ey, ez;
		int ti;
		bool eliminado;
		bool update; //indica si se deben recalcular los vertices o no debido a una transformacion
		glm::vec3 puntoRecta,pOrig;
		unsigned int VAO, VBO, EBO; //variables que guardan nuestro objeto como un espacio en la memoria
		int ID; //id específica del objeto, identificador
		std::string label;
		unsigned int type; //tipo del objeto. 1 = hexaedro, 2= pirámide regular, 3=recta
		std::vector<float>vertices,verOrig;
		std::vector<unsigned int>indices;
		glm::vec3 color;
		Object(unsigned int tipo, glm::vec3 verA, glm::vec3 verB, glm::vec3 acolor, int id,std::string* name)
		{
			bool eliminado=false;
			bool update=false;
			ex = 1.0f;
			ey = 1.0f;
			ez = 1.0f;
			tx = 0.0f;
			ty = 0.0f;
			tz = 0.0f;
			anguloRotacionX = 0.0f;
			anguloRotacionY = 0.0f;
			anguloRotacionZ = 0.0f;
			label = *name;
			ID = id;
			type = tipo;
			color = acolor;
			if (type == 1)
			{
				verA.y += 0.01f;
				verB.y += 0.01f;
				definirCubo(verA, verB, acolor);
			}
			else
			{
				puntoRecta = verA;
				glm::vec3 direcc = glm::vec3(verB-verA);
				definirRecta(verA, direcc, acolor);
			}
		}
		Object(unsigned int tipo, glm::vec3 verA, glm::vec3 verB, glm::vec3 verC, float h, glm::vec3 acolor, int id, std::string* name)
		{
			bool eliminado = false;
			bool update = false;
			ex = 1.0f;
			ey = 1.0f;
			ez = 1.0f;
			tx = 0.0f;
			ty = 0.0f;
			tz = 0.0f;
			anguloRotacionX = 0.0f;
			anguloRotacionY = 0.0f;
			anguloRotacionZ = 0.0f;
			label = *name;
			ID = id;
			type = tipo;
			color = acolor;
			verA.y += 0.01f;
			verB.y += 0.01f;
			verC.y += 0.01f;
			definirPiramide(verA,verB,verC,acolor,h);
		}
		Object(unsigned int tipo, glm::vec3 verA, glm::vec3 direcc, glm::vec3 acolor, int id, unsigned int dummy, std::string* name)
		{
			bool eliminado = false;
			bool update = false;
			ex = 1.0f;
			ey = 1.0f;
			ez = 1.0f;
			tx = 0.0f;
			ty = 0.0f;
			tz = 0.0f;
			anguloRotacionX = 0.0f;
			anguloRotacionY = 0.0f;
			anguloRotacionZ = 0.0f;
			puntoRecta = verA;
			label = *name;
			//dummy se utiliza para diferenciar el llamado donde se proporcionan dos puntos y el llamado donde se proporciona un punto y un vector direccional
			ID = id;
			type = tipo;
			color = acolor;
			definirRecta(verA, direcc, acolor);
		}
		void dibujar();
		void editarColor();
		void rotar();
		void traslacion();
		void escalar();
		void transformar();
	private:
		void definirCubo(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 color);
		void definirPiramide(glm::vec3 ver1, glm::vec3 ver2, glm::vec3 ver3, glm::vec3 color, float h);
		void definirRecta(glm::vec3 punto, glm::vec3 vecDireccion, glm::vec3 color);

		//void construirCubo()
	};

#endif