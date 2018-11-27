
#include<glad\glad.h> //incluir primero glad que glfw. Glad ya tiene los headers de OpenGL para otras librerias que los requieren
#include<GLFW\glfw3.h>
#include<iostream>
#include<math.h>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>
#include<gl\GL.h>
#include<gl\GLU.h>
#include<string>
#include<algorithm>
#include<imgui\imgui.h>
#include<imgui\imgui_impl_glfw.h> //integración con glfw
#include<imgui\imgui_impl_opengl3.h> //integración con OpenGL3
#include"Shader.h"
#include"menu.h"
#include"Objetos.h"
std::vector<Object>objetos; //vector de objetos
unsigned int ScrW=1280;
unsigned int ScrH=720;
unsigned int objSeleccionado; //indica el ID del objeto que fue seleccionado
bool mainMenu = true;
bool nuevoObjeto = false;
bool visible = true;
bool pressed;
std::vector<float>vPlanoXZ; //vertices que delimitan la cuadrícula sobre el plano XZ
std::vector<unsigned int>iPlanoXZ; //indices de los vértices

///declarar matrices de proyección, inicializarlas como matriz identidad
   //SUPER IMPORTANTE!!!!!!
   //A partir de glm 0.9.9, las matrices al declararlas no inician como matriz identidad, entonces hay que indicarlo con glm::mat4 matriz = glm::mat4(1.0f)
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);
//TO-DO: matrices de transformación, producto cruz, producto punto
float ejes[] =
{	///vertices				//colores
	-50.0f, 0.0f, 0.0f,		1.0f,0.0f,0.0f,	//eje X: rojo
	 50.0f, 0.0f, 0.0f,		1.0f,0.0f,0.0f,
	 49.0f, 1.0f, 0.0f,		1.0f,0.0f,0.0f,
	 49.0f,-1.0f, 0.0f,		1.0f,0.0f,0.0f,

	 0.0f, 0.0f, 50.0f,		0.0f,0.0f,1.0f, //eje Z: azul
	 0.0f, 0.0f,-50.0f,		0.0f,0.0f,1.0f,
	 0.0f, 1.0f, 50.0f,		0.0f,0.0f,1.0f,
	 0.0f,-1.0f, 50.0f,		0.0f,0.0f,1.0f,

	 0.0f, 100.0f, 0.0f,	0.0f,1.0f,0.0f, //eje Y: verde
	 0.0f,-100.0f,0.0f,		0.0f,1.0f,0.0f,
	 1.0f, 100.0f,0.0f,		0.0f,1.0f,0.0f,
	-1.0f, 100.0f,0.0f,		0.0f,1.0f,0.0f
};
unsigned int iEjes[] = { 0,1,	1,2,	1,3,	4,5,	4,6,	4,7,	8,9,	8,10,	8,11 };
void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	glViewport(0,0, w, h);
	return;
}
void processInput(GLFWwindow* window);
float magnitud(glm::vec3 v1, glm::vec3 v2)
{
	return sqrtf((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y) + (v1.z - v2.z)*(v1.z - v2.z));
}

///crear la cámara
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f); 

///valores para la cámara
float fov = 45.0f; //field of view, maneja el angulo de visión y el zoom de la cámara
float circleY;
float circleX;
float radioCam = 10.0f;
float inicialX, inicialY;
float lastPosX = (float)ScrW / 2.0f; //centrar la cámara al inicio
float lastPosY = (float)ScrH / 2.0f;
float yaw; //angulo para rotar respecto el eje y
float pitch; //angulo para rotar respecto al plano xz (pero tambien en el eje xz)
///variables para detección de frames: debido a que las computadoras tiene diferentes potencias de procesamiento, se utilizan para que los inputs (entradas del usuario) sean uniformes para los frames.
///Cada frame tarda cierto tiempo en renderizar (ciclos), y durante cada frame se llama al procedimiento que procesa los inputs, por lo que algunos equipos que renderizan más o menos frames en un segundo llaman
///más o menos veces a dicho procedimiento, lo que afecta las entradas. Con esto, se quiere calcular el tiempo entre frames y así utilizarlo para uniformizar las entradas.
float deltaTime = 0.0f;   
float lastFrameTime = 0.0f;
bool firstPress = false;

void mouse_callback(GLFWwindow* window, double xpos, double ypos); //callback hacia la funcion definida abajo
void scroll_callback(GLFWwindow* window, double despX, double despY); //funcion que maneja el zoom al girar la rueda del raton, definida abajo
void datosHex(std::vector<Object>::iterator it);
void datosRecta(std::vector<Object>::iterator it);
void datosPiramide(std::vector<Object>::iterator it);



int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)  ///poner esta linea en lugar de int main() para esconder la consola
{
	//FreeConsole();  o descomentar esta linea para esconder la consola (se muestra al inicio)
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Indican la versión en la que se va a trabajar
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Major es el primer valor de la version, menor el segundo, separados por un punto (version major.menor)	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //seleccionar el perfil core
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  necesario en macOS

	///Crear la ventana
	GLFWwindow* window = glfwCreateWindow(ScrW, ScrH, "Graphica 0.1", NULL, NULL); //la funcion devuelve la direccion de la ventana guardada en el puntero window
	if (window == NULL) //si no se creo la ventana
	{
		std::cout << "Fallo al crear la ventana.\n";
		glfwTerminate();
	}
	glfwMakeContextCurrent(window); //hacer el context del window el contexto principal

	///inicializar GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) //le decimos a GLAD que cargue las direcciones de las funciones de los punteros de las funciones de OpenGL, que son OS-specific
	{														 //glfw define la funcion correcta basada en el OS en que compilemos
		std::cout << "Fallo al inicializar GLAD.\n";
		return -1;
	}

	///darle a OpenGL el tamaño de la rendering Window
	glViewport(0, 0, ScrW, ScrH); //x,y de la esq. inf. izq,ancho y alto de la ventana. 
								//la viewport define que parte de la ventana es en la que se va a ver la scene de OpenGL
								//Se puede mostrar la escena en solo una parte de la ventana y mostrar otras cosas en el espacio restante

	
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	
	///Calcular los puntos que forman las lineas de la cuadrícula sobre XZ
	for (int i = -50; i <= 50; i++)
	{
		if (!i)
			continue;
		vPlanoXZ.push_back(50.0f);
		vPlanoXZ.push_back(0.0f); //primer punto de la línea paralela al eje X con z variable
		vPlanoXZ.push_back((float)i);

		vPlanoXZ.push_back(0.8f);
		vPlanoXZ.push_back(0.8f); //color blanco
		vPlanoXZ.push_back(0.8f);

		vPlanoXZ.push_back(-50.0f);
		vPlanoXZ.push_back(0.0f); //segundo punto de la línea paralela al eje X con z variable
		vPlanoXZ.push_back((float)i);

		vPlanoXZ.push_back(0.8f);
		vPlanoXZ.push_back(0.8f); //color blanco
		vPlanoXZ.push_back(0.8f);
	}
	for (unsigned int i = 0; i < 198; i+=2)
	{
		iPlanoXZ.push_back(i);
		iPlanoXZ.push_back(i+1);
	}
	for (int i = -50; i <= 50; i++)
	{
		if (!i)
			continue;
		vPlanoXZ.push_back((float)i);
		vPlanoXZ.push_back(0.0f); //primer punto de la línea paralela al eje X con z variable
		vPlanoXZ.push_back((50.0f));

		vPlanoXZ.push_back(0.8f);
		vPlanoXZ.push_back(0.8f); //color blanco
		vPlanoXZ.push_back(0.8f);

		vPlanoXZ.push_back((float)i);
		vPlanoXZ.push_back(0.0f); //segundo punto de la línea paralela al eje X con z variable
		vPlanoXZ.push_back(-50.0f);

		vPlanoXZ.push_back(0.8f);
		vPlanoXZ.push_back(0.8f); //color blanco
		vPlanoXZ.push_back(0.8f);
	}
	for (unsigned int i = 0; i < 198; i += 2)
	{
		iPlanoXZ.push_back(i + 200);
		iPlanoXZ.push_back(i + 201);
	}

	unsigned int VAOplane,VBOplane,EBOplane,VAOejes, VBOejes, EBOejes; //variables que guardan los objetos de la cuadrícula y los ejes

	///Generar los buffers
	///copiar nuestros arreglos de vértices en un buffer para que OpenGL lo pueda usar, en el Vertex Array cargado (VAO)
	///Unir los Buffers al tipo que pertenecen e indicar los datos que van a dibujar 
	///Establecer los punteros de los atributos de los vértices. Describir como están compuestos en nuestro arreglo
	///Habilitar que inicie desde la posición 0// en el offset se especifica que los colores van después de las posiciones///Habilitar que inicie desde la posición 1

	///crear vao de plano
	glGenVertexArrays(1, &VAOplane);
	glGenBuffers(1, &EBOplane);
	glGenBuffers(1, &VBOplane);
	glBindVertexArray(VAOplane);
	glBindBuffer(GL_ARRAY_BUFFER, VBOplane);
	glBufferData(GL_ARRAY_BUFFER, vPlanoXZ.size()*sizeof(float), vPlanoXZ.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOplane);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, iPlanoXZ.size()*sizeof(float), iPlanoXZ.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //el offset señala cuanto se tiene que desplazar desde el inicio del arreglo para encontrar 
	glEnableVertexAttribArray(1);																	//el inicio de lo que señala
	
	///crear vao de ejes 
	glGenVertexArrays(1, &VAOejes);
	glGenBuffers(1, &VBOejes);
	glGenBuffers(1, &EBOejes);
	glBindVertexArray(VAOejes);
	glBindBuffer(GL_ARRAY_BUFFER, VBOejes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ejes), ejes, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOejes);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iEjes), iEjes, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
	glEnableVertexAttribArray(1);

	///Compilar y linkear shaders al Shader Program
	//IMPORTANTE: Incluir la carpeta de shaders en la carpeta del exe
	Shader faceBorder("shaders/vertexShader.vs", "shaders/blackBorderShader.fs");
	Shader planeShader("shaders/planeShader.vs", "shaders/planeShader.fs");
	Shader faceColor("shaders/vertexShader.vs", "shaders/fragmentShader.fs");
	Shader pickingShader("shaders/vertexShader.vs", "shaders/colorPickingShader.fs");

	///cada vez que la ventana se resizea, se llama a la funcion que ajusta el Viewport
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  //funciones callback determinan que hacer en caso de que se triggeree un evento
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	///inicializar la cámara viendo hacia el origen
	glm::vec3 camTarget = glm::vec3(0.0f, 0.0f, 0.0f); //la cámara siempre apunta al origen de corrdenadas
	///obtener la posicion inicial de la cámara mediante trigonometria, obteniendo la posicion dado un radio y grados recorridos (en este caso, 0)
	camPos = glm::vec3(sin(glm::radians(circleX)) * cos(glm::radians(circleY)) * radioCam, sin(glm::radians(circleY)) * radioCam, cos(glm::radians(circleX))*cos(glm::radians(circleY)) * radioCam); 
	view = glm::lookAt(camPos, camTarget, camUp);

	///crear la GUI (interfaz gráfica de usuario)
	ImGui::CreateContext();
	ImGui_ImplOpenGL3_Init("#version 130");
	ImGui_ImplGlfw_InitForOpenGL(window, true); //inicializar ImGui para OpenGL
	ImGui::StyleColorsDark();
	ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	ImGuiIO &io = ImGui::GetIO();
	ImFont* pFont = io.Fonts->AddFontFromFileTTF("trebuc.ttf",15.0f);	//IMPORTANTE: Si se usa font personalizada, incluirla en la carpeta del exe

	///Loop: mientras no se señale que la ventana se debe cerrar
	while (!glfwWindowShouldClose(window))
	{
		///calcular tiempo entre frames (delta frame)
		float currentFrameTime = glfwGetTime(); //obtener en qué tiempo vamos
		deltaTime = currentFrameTime - lastFrameTime; //obtener cuantos tiempo ha pasado entre un frame y el anterior
		lastFrameTime = currentFrameTime; //marcar el tiempo actual como el anterior

		///input
		processInput(window);
	
		///draw or render
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		///variar el color verde
		float timeV = glfwGetTime();  //obtener el tiempo
		float Tone = (sin(timeV) / 2.0f) + 1.0f; //calcular el tono mediantela función ciclica de seno
		projection = glm::perspective(glm::radians(fov), (float)ScrW / (float)ScrH, 0.1f, 100.0f);

		///crear el marco de ImGui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		///modificar el uniform
		faceColor.use();
		faceColor.setVec4f("ourColor", glm::vec4(Tone, Tone, Tone, 1.0f));//modificar el uniform en la dirección obtenida (solo se puede modificar una vez que se carga el programa)

		float radius = 10.0f;
		faceColor.use();
		faceColor.setMat4f("view", view);
		faceColor.setMat4f("projection", projection);
		faceBorder.use();
		faceBorder.setMat4f("view", view);
		faceBorder.setMat4f("projection", projection);
		planeShader.use();
		planeShader.setMat4f("view", view);
		planeShader.setMat4f("projection", projection);

		///dibujar el plano y ejes
		planeShader.use();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(VAOplane);
		glDrawElements(GL_LINES, 396, GL_UNSIGNED_INT, 0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(VAOejes);
		glDrawElements(GL_LINES, 18, GL_UNSIGNED_INT, 0);

		for (std::vector<Object>::iterator it=objetos.begin(); it!=objetos.end();it++)
		{
			if (it->eliminado==true)
				continue;
			if (it->ID == objSeleccionado)
				faceColor.use();
			else
				planeShader.use();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			it->dibujar();
			faceBorder.use();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			it->dibujar();
		}

		if(visible)
			MenuPrincipal(window);
		
		///dibujar la GUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		ImGui::EndFrame();
		///check events and swap buffers
		glfwSwapBuffers(window); //Cambiar los buffers para actualizar la imagen
		glfwPollEvents(); //Checa si hay eventos que se triggerearon, llama al procedimiento en caso de los mismos (configurados mediante callbacks) y actualiza la ventana
	}
	///limpiar la gui al acabar
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
			pressed = true;
		if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE && pressed)
		{
			visible ^= 1;
			pressed = false;
		}
	}
	return;


}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
		{
			if (!firstPress)
			{
				firstPress = true;
				inicialX = xpos;
				inicialY = ypos;
				return;
			}
			else if (firstPress)
			{
				float despX = xpos - inicialX;
				float despY = inicialY - ypos; //invertido porque para la ventana, las y aumentan hacia abajo
				inicialX = xpos;
				inicialY = ypos;
				float sensibilidadX = 0.5f;
				float sensibilidadY = 0.1f;
				despX *= sensibilidadX *1.0f;
				despY *= sensibilidadY;
				///la variable circle maneja el angulo de giro respecto a Y, y circleY el giro de la cámara hacia arriba
				circleY += despY;
				circleX += despX;
				///se reinicia la variable circle cada vez que complete el periodo de seno y coseno (360 grados) para que no se desborde
				if (circleX > 360.0f)
					circleX -= 360.0f;
				if (circleX < -360.0f)
					circleX += 360.0f;
				if (circleY > 89.0f)
					circleY = 89.0f;
				if (circleY < -89.0f)
					circleY = -89.0f;
				float posX = sin(glm::radians(circleX)) * cos(glm::radians(circleY)) * radioCam;
				float posZ = cos(glm::radians(circleX))*cos(glm::radians(circleY)) * radioCam;
				float posY = sin(glm::radians(circleY)) * radioCam;
				camPos = glm::vec3(posX, posY, posZ);
				view = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), camUp);
			}
		}
		else
		{
			firstPress = false;
		}
}

void scroll_callback(GLFWwindow* window, double despX, double despY)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		if (fov >= 1.0f && fov <= 90.0f) //aumenta o disminuye el angulo de vision segun el desplazamiento de la rueda del ratón
			fov -= despY*2.0f;
		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 90.0f)
			fov = 90.0f;
	}
}

void MenuPrincipal(GLFWwindow* window)
{
	using namespace ImGui;
	Begin("Graphica Alpha 0.1", &mainMenu);
	Text("Bienvenido a Graphica (Alpha v0.1)");
	Text("Disenado y programado por Felix Garduza Gutierrez.");
	Text("(c) 2018.");
	Separator();
	Text("Manten presionado Click Derecho y arrastra el raton para mover la camara.");
	Text("Presiona Alt + V para activar o desactivar los menus.");
	Text("Mientras mantienes presionado Alt, gira el Scroll del raton hacia");
	Text("adelante para acercar la vista o hacia atras para alejarla.");
	Separator();
	if (ImGui::CollapsingHeader("Lista de Objetos"))
	{
		
		for (std::vector<Object>::iterator it = objetos.begin(); it != objetos.end(); it++)
		{
			if (it->eliminado==true)
				continue;
			if (TreeNode(it->label.c_str()))
			{
				objSeleccionado = it->ID;
				if (it->type == 1)
				{
					Text("Tipo: Hexaedro");
					datosHex(it);
				}
				if (it->type == 2)
				{
					Text("Tipo: Piramide");
					datosPiramide(it);
				}
				if (it->type == 3)
				{
					Text("Tipo: Recta");
					datosRecta(it);
				}
				TreePop();
			}
			Separator();
		}
	}
	Separator();
	if (ImGui::Button("Crear nuevo objeto",ImVec2(150.0f,25.0f)))
			nuevoObjeto = true;
	SameLine();
	if (ImGui::Button("Salir", ImVec2(100.0f, 25.0f)))
		glfwSetWindowShouldClose(window, true);
	ImGui::End();

	if (nuevoObjeto)
		MenuNuevoObjeto();
}

glm::vec3 p1 = glm::vec3(0.0f);
glm::vec3 p2 = glm::vec3(0.0f);
glm::vec3 p3 = glm::vec3(0.0f);
glm::vec3 p4 = glm::vec3(0.0f);
glm::vec3 p5 = glm::vec3(0.0f);
glm::vec3 p6 = glm::vec3(0.0f);
glm::vec3 p7 = glm::vec3(0.0f);
glm::vec3 p8 = glm::vec3(0.0f);
glm::vec3 p9 = glm::vec3(0.0f);
glm::vec3 p10 = glm::vec3(0.0f);
glm::vec3 p11 = glm::vec3(0.0f);
std::string cad,nombre;
char nom[100];
int cont;
float col[3],colP[3],colR[3],h,colAux[3];
void MenuNuevoObjeto()
{
	using namespace ImGui;
	Begin("Crear nuevo objeto");
	if (CollapsingHeader("Hexaedro"))
	{
		InputText("Nombre del Objeto",nom,sizeof(nom));
		nombre = nom;
		Indent();
		Text("Ingresa las coordenadas de los dos puntos opuestos");
		Text("que delimitan el hexaedro.");
		Text("Ingresa las coordenadas del primer punto.");
		InputFloat("X##1", &p1.x);
		InputFloat("Y##1", &p1.y);
		InputFloat("Z##1", &p1.z);

		Text("Ingresa las coordenadas del segundo punto.");
		InputFloat("X##2", &p2.x);
		InputFloat("Y##2", &p2.y);
		InputFloat("Z##2", &p2.z);
		Unindent();

		Text("Elige un color.##1");
		ColorPicker3("Color##1", col);
		if (Button("Crear##1"))
		{
			objetos.push_back(Object(1, p1, p2, glm::vec3(col[0], col[1], col[2]), objetos.size() + 1, &nombre));
			memset(&nom, NULL, 100);
			memset(&col, 0.0f, 3);
			p2 = glm::vec3(0.0f);
			p1 = glm::vec3(0.0f);
			nuevoObjeto = false;
		}
	}
	else if (CollapsingHeader("Piramide Regular"))
	{
		InputText("Nombre del Objeto", nom, sizeof(nom));
		nombre = nom;
		Text("Ingresa las coordenadas de los 3 puntos de la base.");
		Text("Los tres puntos deben formar un triangulo equilatero.");
		Indent();
		Text("Ingresa las coordenadas del primer punto.");
		InputFloat("X##3", &p3.x,0.0f,0.0f,"%.5f");
		InputFloat("Y##3", &p3.y, 0.0f, 0.0f, "%.5f");
		InputFloat("Z##3", &p3.z, 0.0f, 0.0f, "%.5f");
		Text("Ingresa las coordenadas del segundo punto.");
		InputFloat("X##4", &p4.x, 0.0f, 0.0f, "%.5f");
		InputFloat("Y##4", &p4.y, 0.0f, 0.0f, "%.5f");
		InputFloat("Z##4", &p4.z, 0.0f, 0.0f, "%.5f");
		Text("Ingresa las coordenadas del tercer punto.");
		InputFloat("X##5", &p5.x, 0.0f, 0.0f, "%.5f");
		InputFloat("Y##5", &p5.y, 0.0f, 0.0f, "%.5f");
		InputFloat("Z##5", &p5.z, 0.0f, 0.0f, "%.5f");
		Unindent();
		Text("Ingresa la altura.");
		InputFloat("h", &h);
		Text("Elige un color.##2");
		ColorPicker3("Color##2", colP);
		if (Button("Crear##2"))
		{
			if (p3.x > p4.x)
				std::swap(p3, p4);
			if (p3.x > p5.x)
				std::swap(p3, p5);
			if (p4.x > p5.x)
				std::swap(p4, p5);
			if (p3.x > p4.x)
				std::swap(p3, p4);
			objetos.push_back(Object(2, p3, p4, p5,h,glm::vec3(colP[0], colP[1], colP[2]), objetos.size() + 1, &nombre));
			memset(&nom, NULL, 100);
			memset(&colP, 0.0f, 3);
			p3 = glm::vec3(0.0f);
			p4 = glm::vec3(0.0f);
			p5 = glm::vec3(0.0f);
			h = 0.0f;
			nuevoObjeto = false;
		}
	}
	else if (CollapsingHeader("Recta"))
	{
		if (TreeNode("Dados dos puntos"))
		{
			InputText("Nombre del Objeto", nom, sizeof(nom));
			nombre = nom;
			Text("Ingresa las coordenadas de 2 puntos pertenecientes a la recta.");
			Indent();
			Text("Ingresa las coordenadas del primer punto.");
			InputFloat("X##6", &p6.x);
			InputFloat("Y##6", &p6.y);
			InputFloat("Z##6", &p6.z);
			Text("Ingresa las coordenadas del segundo punto.");
			InputFloat("X##7", &p7.x);
			InputFloat("Y##7", &p7.y);
			InputFloat("Z##7", &p7.z);
			ColorPicker3("Color##3", colR);
			if (Button("Crear##3"))
			{
				objetos.push_back(Object(3, p6, p7, glm::vec3(colR[0], colR[1], colR[2]), objetos.size() + 1, &nombre));
				memset(&nom, NULL, 100);
				memset(&colR, 0.0f, 3);
				p6 = glm::vec3(0.0f);
				p7 = glm::vec3(0.0f);
				nuevoObjeto = false;
			}
			TreePop();
		}
		else if (TreeNode("Dado un punto y una direccion"))
		{
			InputText("Nombre del Objeto##4", nom, sizeof(nom));
			nombre = nom;
			Text("Ingresa las coordenada de un punto perteneciente a la recta.");
			Indent();
			InputFloat("X##8", &p8.x);
			InputFloat("Y##8", &p8.y);
			InputFloat("Z##8", &p8.z);
			Unindent();
			Text("Ingresa los coeficientes del vector direccional.");
			Indent();
			InputFloat("i##9", &p9.x);
			InputFloat("j##9", &p9.y);
			InputFloat("k##9", &p9.z);
			Unindent();
			ColorPicker3("Color##9", colR);
			if (Button("Crear##9"))
			{
				objetos.push_back(Object(3, p8, p9, glm::vec3(colR[0], colR[1], colR[2]), objetos.size() + 1,1, &nombre));
				memset(&nom, NULL, 100);
				memset(&colR, 0.0f, 3);
				p8 = glm::vec3(0.0f);
				p9 = glm::vec3(0.0f);
				nuevoObjeto = false;
			}
			TreePop();
		}
		else if (TreeNode("Dada la ecuacion vectorial"))
		{
			InputText("Nombre del Objeto##10", nom, sizeof(nom));
			Text("Dada la ecuacion de la forma");
			Text("(X-a)i + (Y-b)j + (Z-c)k = t(Ei + Fj + Gk)");
			Text("Ingresa los valores de las variables que definen");
			Text("la ecuacion.");
			nombre = nom;
			Indent();
			InputFloat("a##10", &p10.x);
			InputFloat("b##10", &p10.y);
			InputFloat("c##10", &p10.z);
			InputFloat("E##11", &p11.x);
			InputFloat("F##11", &p11.y);
			InputFloat("G##11", &p11.z);
			Unindent();
			ColorPicker3("Color##9", colR);
			if (Button("Crear##9"))
			{
				objetos.push_back(Object(3, p10, p11, glm::vec3(colR[0], colR[1], colR[2]), objetos.size() + 1, 1, &nombre));
				memset(&nom, NULL, 100);
				memset(&colR, 0.0f, 3);
				p10 = glm::vec3(0.0f);
				p11 = glm::vec3(0.0f);
				nuevoObjeto = false;
			}
			TreePop();
		}
	}
	Separator();
	if (Button("Cerrar"))
		nuevoObjeto = false;
	ImGui::End();
}

std::string let[8] = { "A" , "B", "C", "D", "E", "F", "G", "H" };
void datosHex(std::vector<Object>::iterator it)
{
	using namespace ImGui;
	Text("Largo: ");
	SameLine();
	Text(std::to_string(magnitud(glm::vec3(it->vertices[6], it->vertices[7], it->vertices[8]), glm::vec3(it->vertices[18], it->vertices[19], it->vertices[20]))).c_str()); //largo, distancia entre D y B
	Text("Ancho: ");
	SameLine();
	Text(std::to_string(magnitud(glm::vec3(it->vertices[30], it->vertices[31], it->vertices[32]), glm::vec3(it->vertices[0], it->vertices[1], it->vertices[2]))).c_str()); //ancho, distancia entre A y F
	Text("Altura: ");
	SameLine();
	Text(std::to_string(magnitud(glm::vec3(it->vertices[30], it->vertices[31], it->vertices[32]), glm::vec3(it->vertices[18], it->vertices[19], it->vertices[20]))).c_str()); //altura, distancia entre D y F
	Text("Vertices:");
	cont=0;
	for (int i = 0; i < it->vertices.size(); i += 6)
	{
		cad = let[cont] + " = (" + std::to_string(it->vertices[i]) + ", " + std::to_string(it->vertices[i + 1]) + ", " + std::to_string(it->vertices[i + 2]) + ")";
		Text(cad.c_str());
		cont++;
	}
	Separator();
	Text("Color:");
	colAux[0] = it->color.x;
	colAux[1] = it->color.y;
	colAux[2] = it->color.z;
	if (ColorEdit3("Color del Objeto", colAux, 1))
	{
		it->color.x = colAux[0];
		it->color.y = colAux[1];
		it->color.z = colAux[2];
		it->editarColor();
	}
	Separator();

	Text("Escalacion en X:");
	SameLine();
	Text(std::to_string(it->ex).c_str());
	Text("Escalacion en Y:");
	SameLine();
	Text(std::to_string(it->ey).c_str());
	Text("Escalacion en Z:");
	SameLine();
	Text(std::to_string(it->ez).c_str());
	if (SliderFloat("Escalacion en X", &it->ex, 0.01f, 10.0f))
		it->update = true;										//si se movio alguno de estos valores, marcar que se debe actualizar
	if(SliderFloat("Escalacion en Y", &it->ey, 0.01f, 10.0f)) 
		it->update = true;
	if(SliderFloat("Escalacion en Z", &it->ez, 0.01f, 10.0f))
		it->update = true;
	Separator();

	Text("Traslacion en X:");
	SameLine();
	Text(std::to_string(it->tx).c_str());
	Text("Traslacion en Y:");
	SameLine();
	Text(std::to_string(it->ty).c_str());
	Text("Traslacion en Z:");
	SameLine();
	Text(std::to_string(it->tz).c_str());
	if (SliderFloat("Traslacion en X", &it->tx, -50.0f, 50.0f)) //si se movio alguno de estos valores, marcar que se debe actualizar
		it->update = true;
	if(SliderFloat("Traslacion en Y", &it->ty, -50.0f, 50.0f))
		it->update = true;
	if(SliderFloat("Traslacion en Z", &it->tz, -50.0f, 50.0f))
		it->update = true;
	Separator();

	Text("Angulo de Rotacion X: ");
	SameLine();
	Text(std::to_string(it->anguloRotacionX).c_str());
	Text("Angulo de Rotacion Y: ");
	SameLine();
	Text(std::to_string(it->anguloRotacionY).c_str());
	Text("Angulo de Rotacion Z: ");
	SameLine();
	Text(std::to_string(it->anguloRotacionZ).c_str());
	if (SliderAngle("Rotar respecto a X", &it->anguloRotacionX)) //si se movio alguno de estos valores, marcar que se debe actualizar
		it->update = true;
	if(SliderAngle("Rotar respecto a Y", &it->anguloRotacionY))
		it->update = true;
	if(SliderAngle("Rotar respecto a Z", &it->anguloRotacionZ))
		it->update = true;
	if (it->update)
	{	//para evitar desperdicio de recursos, solo recalcular los vertices si hubo un cambio
		it->update = false;
		it->transformar();
	}
	if (Button("Eliminar objeto"))
		it->eliminado = true;
}

void datosPiramide(std::vector<Object>::iterator it)
{
	using namespace ImGui;
	Text("Longitud de cada lado de la base: ");
	SameLine();
	Text(std::to_string(magnitud(glm::vec3(it->vertices[0],it->vertices[1],it->vertices[2]), glm::vec3(it->vertices[6], it->vertices[7], it->vertices[8]))).c_str());
	cont = 0;
	for (int i = 0; i < it->vertices.size(); i += 6)
	{
		cad = let[cont] + " = (" + std::to_string(it->vertices[i]) + ", " + std::to_string(it->vertices[i + 1]) + ", " + std::to_string(it->vertices[i + 2]) + ")";
		Text(cad.c_str());
		cont++;
	}
	Text("Color:");
	colAux[0] = it->color.x;
	colAux[1] = it->color.y;
	colAux[2] = it->color.z;
	if (ColorEdit3("Color del Objeto", colAux, 1))
	{
		it->color.x = colAux[0];
		it->color.y = colAux[1];
		it->color.z = colAux[2];
		it->editarColor();
	}
	Separator();

	Text("Escalacion en X:");
	SameLine();
	Text(std::to_string(it->ex).c_str());
	Text("Escalacion en Y:");
	SameLine();
	Text(std::to_string(it->ey).c_str());
	Text("Escalacion en Z:");
	SameLine();
	Text(std::to_string(it->ez).c_str());
	if (SliderFloat("Escalacion en X", &it->ex, 0.01f, 10.0f))
		it->update = true;										//si se movio alguno de estos valores, marcar que se debe actualizar
	if (SliderFloat("Escalacion en Y", &it->ey, 0.01f, 10.0f))
		it->update = true;
	if (SliderFloat("Escalacion en Z", &it->ez, 0.01f, 10.0f))
		it->update = true;
	Separator();

	Text("Traslacion en X:");
	SameLine();
	Text(std::to_string(it->tx).c_str());
	Text("Traslacion en Y:");
	SameLine();
	Text(std::to_string(it->ty).c_str());
	Text("Traslacion en Z:");
	SameLine();
	Text(std::to_string(it->tz).c_str());
	if (SliderFloat("Traslacion en X", &it->tx, -50.0f, 50.0f)) //si se movio alguno de estos valores, marcar que se debe actualizar
		it->update = true;
	if (SliderFloat("Traslacion en Y", &it->ty, -50.0f, 50.0f))
		it->update = true;
	if (SliderFloat("Traslacion en Z", &it->tz, -50.0f, 50.0f))
		it->update = true;
	Separator();

	Text("Angulo de Rotacion X: ");
	SameLine();
	Text(std::to_string(it->anguloRotacionX).c_str());
	Text("Angulo de Rotacion Y: ");
	SameLine();
	Text(std::to_string(it->anguloRotacionY).c_str());
	Text("Angulo de Rotacion Z: ");
	SameLine();
	Text(std::to_string(it->anguloRotacionZ).c_str());
	if (SliderAngle("Rotar respecto a X", &it->anguloRotacionX)) //si se movio alguno de estos valores, marcar que se debe actualizar
		it->update = true;
	if (SliderAngle("Rotar respecto a Y", &it->anguloRotacionY))
		it->update = true;
	if (SliderAngle("Rotar respecto a Z", &it->anguloRotacionZ))
		it->update = true;
	if (it->update)
	{	//para evitar desperdicio de recursos, solo recalcular los vertices si hubo un cambio
		it->update = false;
		it->transformar();
	}
	if (Button("Eliminar objeto"))
		it->eliminado = true;
}
float vI, vJ, vK;
std::string xString, yString, zString;
void datosRecta(std::vector<Object>::iterator it)
{
	using namespace ImGui;
	Text("Vector direccional: ");
	SameLine();
	vI = it->vertices[0] - it->vertices[6];
	vJ = it->vertices[1] - it->vertices[7];
	vK = it->vertices[2] - it->vertices[8];
	while ((int)vI % 2 == 0 && (int)vJ % 2 == 0 && (int)vK % 2 == 0)
	{
		vI /= 2.0f;
		vJ /= 2.0f;
		vK /= 2.0f;
	}
	while ((int)vI % 3 == 0 && (int)vJ % 3 == 0 && (int)vK % 3 == 0)
	{
		vI /= 3.0f;
		vJ /= 3.0f;
		vK /= 3.0f;
	}
	while ((int)vI % 5 == 0 && (int)vJ % 5 == 0 && (int)vK % 5 == 0)
	{
		vI /= 5.0f;
		vJ /= 5.0f;
		vK /= 5.0f;
	}
	while ((int)vI % 7 == 0 && (int)vJ % 7 == 0 && (int)vK % 7 == 0)
	{
		vI /= 7.0f;
		vJ /= 7.0f;
		vK /= 7.0f;
	}

	cad = "" + std::to_string(vI) + "i";
	if (vJ < 0)
		cad += std::to_string(vJ) + "j";
	else
		cad += " + " + std::to_string(vJ) + "j";
	if (vK < 0)
		cad += std::to_string(vK) + "k";
	else
		cad += " + " + std::to_string(vK) + "k";
	Text(cad.c_str());

	xString = "X - (" + std::to_string(it->puntoRecta.x) + ") = t(" + std::to_string(vI).c_str() + ")";
	yString = "Y - (" + std::to_string(it->puntoRecta.y) + ") = t(" + std::to_string(vJ).c_str() + ")";
	zString = "Z - (" + std::to_string(it->puntoRecta.z) + ") = t(" + std::to_string(vK).c_str() + ")";

	Text("Ecuacion parametrica: ");
	Text(xString.c_str());
	Text(yString.c_str());
	Text(zString.c_str());

	cad = "(X - (" + std::to_string(it->puntoRecta.x) + ")i + (Y - (" + std::to_string(it->puntoRecta.y) + ")j + (Z - (" + std::to_string(it->puntoRecta.z) + ")j) = t(" + cad + ")";
	Text("Ecuacion Vectorial :");
	Text(cad.c_str());

	Text("Color:");
	colAux[0] = it->color.x;
	colAux[1] = it->color.y;
	colAux[2] = it->color.z;
	if (ColorEdit3("Color del Objeto", colAux, 1))
	{
		it->color.x = colAux[0];
		it->color.y = colAux[1];
		it->color.z = colAux[2];
		it->editarColor();
	}
	if (Button("Eliminar objeto"))
		it->eliminado = true;
}