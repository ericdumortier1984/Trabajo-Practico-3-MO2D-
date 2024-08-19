# include <GL/gl.h> // Incluye la biblioteca OpenGL para gráficos
# include <GL/glu.h> // Incluye la biblioteca GLU para utilidades de OpenGL
# include <GL/glut.h> // Incluye GLUT para la creación de ventanas y manejo de eventos
# include <iostream>  // Incluye la biblioteca estándar de entrada/salida
# include <cmath> // Incluye funciones matemáticas
# include <list> // Incluye la biblioteca para listas

# include "uglyfont.h" // Incluye la fuente UglyFont para renderizar texto
# include "keyboard.h" // Incluye la clase para manejar el teclado

using namespace std;

//------VARIABLES GLOBALES---------
double 
	shipX = 400, // Posición en x
	shipY = 300, // Posición en y
    shipAngle = 0, // Orientación nave
	machineGunAngle = 0, // Orientación del arma
	scale = 1.1; // Escala para el zoom

const double PI = 4*atan(1.0); // Cálculo del valor de PI

const double // Constantes para la posición en z de diferentes elementos
	zBullet = 0.2,
	zBodyShip = 0.3,
	zCabin = 0.6,
	zLeftWing = 0.5,
	zRightWing = 0.5,
	zLeftLight = 0.3,
	zRightLight = 0.3,
	zThrusters = 0.3,
	zRadar = 0.9;

static int AngLineRadar = 0; // Variable estática para el ángulo de rotación de la línea del radar

int // Variables para el tamaño de la ventana y límites de movimiento
	w = 800, // Ancho de la ventana
	h = 600, // Alto de la ventana
	xMax = 750, // Máximo en x
	xmin = 50, // Mínimo en x
	yMax = 550, // Máximo en y
	yMin = 50, // Mínimo en y
	
	mySeconds, // Segundos transcurridos desde el inicio
	myMilliseconds, // Milisegundos transcurridos desde el inicio
	
	myMotor = 100; // Motor inicial

// Flags para información de línea de comando y zoom
bool commandLineinfo = true; // Para mostrar errores en la línea de comando
bool zoom = false; // Estado del zoom

Keyboard keyboard(' ', 'w', 's', 'a', 'd', 'z', 'b'); // Inicializa el teclado con las teclas definidas

//----------------------------------------------------

class Bullet // Clase para las balas
{
private:
	double x; // Posición x de la bala
	double y; // Posición y de la bala
	double dX; // Cambio en x
	double dY; // Cambio en y
public:
	Bullet(double posX, double posY, double distanceX, double distanceY) : x(posX), y(posY), dX(distanceX), dY(distanceY) {}  // Constructor de la clase Bullet
	bool Update() // Actualiza la posición de la bala y verifica si está fuera de pantalla
	{
		x += dX; // Actualiza x
		y += dY; // Actualiza y
		return ( x > w || x < 0 || y > h || y < 0 ); // Retorna true si la bala está fuera de pantalla
	}
	void Draw() // Dibuja la bala en la posición actual
	{
		glVertex2d(x,y); // Dibuja un punto en la posición de la bala
	}
};

list<Bullet> bullet; // Lista para almacenar las balas


// Renderiza texto en pantalla usando UglyFont
void drawText(string _string, float x, float y, float textScale = 1.0, float red = 1.0, float green = 1.0, float blue = 1.0, float alpha = 1.0, float textAngle = 0.0, int textCenter = 0, float textWidth = 1.0 )
{
	glPushAttrib(GL_ALL_ATTRIB_BITS); // Guarda todos los atributos actuales
	glLineWidth(textWidth); // Establece el ancho de la línea del texto
	glColor4f(red, green, blue, alpha); // Establece el color del texto
	glPushMatrix(); // Guarda la matriz actual
	glTranslated(x, y, 0); // Mueve la posición del texto
	glRotatef(textAngle, 0, 0, 1); // Rota el texto
	glScalef(textScale, textScale, textScale); // Escala el texto
	YsDrawUglyFont(_string.c_str(), textCenter); // Dibuja el texto
	glPopMatrix(); // Restaura la matriz
	glPopAttrib();  // Restaura los atributos
}

void drawShipCabin() // Dibuja la cabina de la nave
{
	glColor3f(0.678f, 0.847f, 0.902f); // Color de la cabina
	double cabinRadius = 10; // Radio de la cabina
	int levelDetail = 30; // nivel de detalle (smooth)
	GLUquadricObj* q = gluNewQuadric(); // Crea un objeto de cuadrica (superficie curva) que se utilizará para dibujar la cabina
	gluQuadricDrawStyle(q, GLU_FILL); // Estilo de bibujo "relleno"
	gluDisk(q, 0, cabinRadius, levelDetail, levelDetail); // Dibuja un disco (la cabina de la nave) con el radio especificado, tercer y cuarto parámetro son segmentos(slices) y anillos(loops) = levelDetail
	gluDeleteQuadric(q); // Elimina el objeto de cuadrica.
}

void drawLeftWing() // Dibuja el ala izquierda
{
	glColor3f(1.0f, 1.0f, 1.0f); // Color del ala
	glBegin(GL_TRIANGLES); // Inicia el dibujo de triángulos
	glVertex2d(0, -10); // Punta del ala
	glVertex2d(-20, 10); // Esquina izquierda
	glVertex2d(20, 10); // Esquina derecha
	glEnd(); // Finaliza el dibujo
} 

void drawRightWing() // Dibuja el ala derecha
{
	glColor3f(1.0f, 1.0f, 1.0f); // Color del ala
	glBegin(GL_TRIANGLES); // Inicia el dibujo de triángulos
	glVertex2d(0, -10); // Punta del ala
	glVertex2d(-20, 10); // Esquina izquierda
	glVertex2d(20, 10); // Esquina derecha
	glEnd(); // Finaliza el dibujo
}

void drawLeftLight() // Dibuja la luz trasera izquierda
{
	glColor3f(1.0, 0.0, 0.0); // Color rojo
	glBegin(GL_QUADS); // Inicia el dibujo de un cuadrado
	glVertex2d(-20, -10); // Esquina inferior izquierda
	glVertex2d(-15, -10); // Esquina inferior derecha
	glVertex2d(-15, -5);  // Esquina superior derecha
	glVertex2d(-20, -5);  // Esquina superior izquierda
	glEnd(); // Finaliza el dibujo
}

void drawRightLight()
{
	glColor3f(1.0, 0.0, 0.0); // Color rojo
	glBegin(GL_QUADS); // Inicia el dibujo de un cuadrado
	glVertex2d(-20, -10); // Esquina inferior izquierda
	glVertex2d(-15, -10); // Esquina inferior derecha
	glVertex2d(-15, -5);  // Esquina superior derecha
	glVertex2d(-20, -5);  // Esquina superior izquierda
	glEnd(); // Finaliza el dibujo
}

void drawThrusters() // Dibuja los propulsores
{
	glColor3f(1.0, 0.5, 0.0); // Color naranja
	glBegin(GL_QUADS); // Inicia el dibujo de un cuadrado
	for (int i = 0; i < 6; ++i) // Dibuja varios propulsores
	{
		// Calcula la posición de cada propulsor
		double Posx = -20 + i * 5; // Espaciado de 5 unidades
		glVertex2d(Posx, -10);     // Esquina inferior izquierda
		glVertex2d(Posx + 5, -10); // Esquina inferior derecha
		glVertex2d(Posx + 5, -5);  // Esquina superior derecha
		glVertex2d(Posx, -5);      // Esquina superior izquierda
	}
	glEnd(); // Finaliza el dibujo
}

void drawBodyShip() // Dibuja el cuerpo de la nave
{
	glColor3f(1.0f, 1.0f, 1.0f); // Color del cuerpo
	glBegin(GL_QUADS); // Inicia el dibujo de un cuadrado
	glVertex2d(15, 40); // Esquina superior derecha
	glVertex2d(-15, 40); // Esquina superior izquierda
	glVertex2d(-15, -40); // Esquina inferior izquierda
	glVertex2d(15, -40); // Esquina inferior derecha
	glEnd(); // Finaliza el dibujo
	
	glPushMatrix(); // Inicia push para redondear la trompa de la nave
	glTranslated(0, 40, 0); // Translada la posición
	double radius = 15; // Radio de la trompa
	double startAngle = 270; // Ángulo de inicio
	double nextAngle = 180; // Ángulo final
	int levelDetail = 30; // Detalle del dibujo
	GLUquadricObj* q = gluNewQuadric(); // Crea un objeto cuadrático
	gluQuadricDrawStyle(q, GLU_FILL); // Estilo de dibujo
	gluPartialDisk(q, 0, radius, levelDetail, levelDetail, startAngle, nextAngle); // Dibuja una porción de un disco
	gluDeleteQuadric(q); // Elimina el objeto cuadrático
	glPopMatrix(); // Restaura la matriz
}

void drawShip() // Dibuja el modelo completo de la nave
{
	glPushMatrix(); // Inicia push inicial
	glTranslated(shipX, shipY, zBodyShip); // Translada el cuerpo de la nave
	glRotated(shipAngle, 0, 0, 1); // Rota el cuerpo de la nave
	drawBodyShip(); // Dibuja el cuerpo de la nave
	
	// Dibuja el ala izquierda
	glPushMatrix();
	glTranslated(-20, -20, zLeftWing); // Translada ala izquierda
	glRotated(50, 0, 0, 1); // Rota ala izquierda
	drawLeftWing(); // Dibuja el ala izquierda
	glPopMatrix();
	
	// Dibuja el ala derecha
	glPushMatrix();
	glTranslated(20, -20, zRightWing); // Translada ala derecha
	glRotated(-50, 0, 0, 1); // Rota ala derecha
	drawRightWing(); // Dibuja el ala derecha
	glPopMatrix();
	
	// Dibuja la cabina
	glPushMatrix();
	glTranslated(0, 15, zCabin); // Translada la cabina
	drawShipCabin(); // Dibuja la cabina
	glPopMatrix();
	
	// Dibuja la luz trasera izquierda
	glPushMatrix();
	glTranslated(-17, -22, zLeftLight); // Translada luz trasera izquierda
	drawLeftLight(); // Dibuja luz trasera izquierda
	glPopMatrix();
	
	// Dibuja la luz trasera derecha
	glPushMatrix();
	glTranslated(54, -22, zRightLight); // Translada luz trasera derecha
	drawRightLight(); // Dibuja luz trasera derecha
	glPopMatrix();
	
	// Dibuja los propulsores
	glPushMatrix();
	glTranslated(5, -35, zThrusters); // Translada los propulsores
	drawThrusters(); // Dibuja los propulsores
	glPopMatrix();
	
	glPopMatrix(); // Fin push inicial
}

void drawShipBoard() // Dibuja el portador de la nave
{
	glColor3f(0.5f, 0.5f, 0.5f); // Color gris
	glLineWidth(5.0); // Ancho de línea
	glBegin(GL_LINES); // Inicia el dibujo de líneas
	glVertex2i(350, 350); glVertex2i(350, 240); // Línea vertical izquierda
	glVertex2i(450, 350); glVertex2i(450, 240); // Línea vertical derecha
	glVertex2i(350, 240); glVertex2i(450, 240); // Línea horizontal
	glEnd(); // Finaliza el dibujo
}

void drawRadar() // Dibuja el radar
{
	glPushMatrix(); // Inicia push para el radar
	
	// Dibuja el radar
	glPushMatrix();
	glColor4f(0.3f, 0.5f, 0.3f, 1.0f); // Color del radar
	glTranslatef(700, 500, zRadar); // Translada la posición del radar
	GLUquadricObj* q = gluNewQuadric(); // Crea un objeto cuadrático
	gluQuadricDrawStyle(q, GLU_FILL); // Estilo de dibujo
	gluDisk(q, 0, 70, 30, 30); // Dibuja el disco del radar
	gluDeleteQuadric(q); // Elimina el objeto cuadrático
	glPopMatrix();
	
	// Dibuja la línea del radar
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f); // Color rojo para la línea
	glTranslatef(700, 500, 0.9); // Translada la posición de la línea
	glRotatef(AngLineRadar, 0.0, 0.0, -1.0); // Rota la línea
	glLineWidth(3); // Ancho de línea
	glBegin(GL_LINES); // Inicia el dibujo de líneas
	glVertex2i(0, 0); glVertex2i(70, 0); // Dibuja la línea
	glEnd(); // Finaliza el dibujo
	glPopMatrix();
	
	glPopMatrix(); // Fin push para el radar
}

void DrawBullet() // Dibuja las balas
{
	glPushMatrix(); // Inicia push para las balas
	glTranslated(0, 0, zBullet); // Translada la posición de las balas
	
	// Dibuja cada bala en la lista
	list<Bullet>::iterator b = bullet.begin();
	glPointSize(3); // Tamaño del punto
	glColor3f(0.5, 0.5, 0.5); // Color de las balas
	glBegin(GL_POINTS); // Inicia el dibujo de puntos
	while (b != bullet.end()) // Itera sobre las balas
	{
		b->Draw(); // Dibuja la bala
		b++;
	}
	glEnd(); // Finaliza el dibujo
	
	glPopMatrix(); // Fin push para las balas
}

void drawLimitsScreen() // Dibuja los límites de la pantalla
{
	glColor3f(0.0, 1.0, 0.0); // Color verde para los límites
	glLineWidth(5.0); // Ancho de línea
	glBegin(GL_LINE_LOOP); // Inicia el dibujo de un bucle de líneas
	glVertex3d(0, 600, 0.9); // Esquina superior izquierda
	glVertex3d(800, 600, 0.9); // Esquina superior derecha
	glVertex3d(800, 0, 0.9); // Esquina inferior derecha
	glVertex3d(0, 0, 0.9); // Esquina inferior izquierda
	glEnd(); // Finaliza el dibujo
}

int integerv(GLenum pname) // Función para obtener valores de estado de OpenGL
{
	int value; // Variable para almacenar el valor
	glGetIntegerv(pname, &value); // Obtiene el valor de OpenGL
	return value; // Retorna el valor
}
# define _PRINT_INT_VALUE(pname) #pname << ": " << integerv(pname) <<endl // Macro para imprimir valores de estado
	
void info() // Función para mostrar información de OpenGL
{
	if(commandLineinfo) // Verifica si se debe mostrar información
	{ 
		cout << "Vendor:         " << glGetString(GL_VENDOR) << endl // Muestra el proveedor de OpenGL
			<< "Renderer:       " << glGetString(GL_RENDERER) << endl // Muestra el renderizador
			<< "GL_Version:     " << glGetString(GL_VERSION) << endl // Muestra la versión de OpenGL
			<< "GL_Extensions:  " << glGetString(GL_EXTENSIONS) << endl // Muestra las extensiones de OpenGL
			<< "GLU_Version:    " << gluGetString(GLU_VERSION) << endl // Muestra la versión de GLU
			<< "GLU_Extensions: " << gluGetString(GLU_EXTENSIONS) << endl // Muestra las extensiones de GLU
			<< _PRINT_INT_VALUE(GL_DOUBLEBUFFER) // Muestra si se está usando doble buffer
			<< _PRINT_INT_VALUE(GL_STEREO) // Muestra si se está usando estereoscopía
			<< _PRINT_INT_VALUE(GL_AUX_BUFFERS) // Muestra el número de buffers auxiliares
			<< _PRINT_INT_VALUE(GL_RED_BITS) // Muestra los bits de rojo
			<< _PRINT_INT_VALUE(GL_GREEN_BITS) // Muestra los bits de verde
			<< _PRINT_INT_VALUE(GL_BLUE_BITS) // Muestra los bits de azul
			<< _PRINT_INT_VALUE(GL_ALPHA_BITS) // Muestra los bits de alfa
			<< _PRINT_INT_VALUE(GL_DEPTH_BITS) // Muestra los bits de profundidad
			<< _PRINT_INT_VALUE(GL_STENCIL_BITS) // Muestra los bits de stencil
			<< _PRINT_INT_VALUE(GL_ACCUM_RED_BITS) // Muestra los bits acumulativos de rojo
			<< _PRINT_INT_VALUE(GL_ACCUM_GREEN_BITS) // Muestra los bits acumulativos de verde
			<< _PRINT_INT_VALUE(GL_ACCUM_BLUE_BITS) // Muestra los bits acumulativos de azul
			<< _PRINT_INT_VALUE(GL_ACCUM_ALPHA_BITS) << endl; // Muestra los bits acumulativos de alfa
	}
	
	// Informa sobre las teclas que se pueden utilizar en el juego
	cout << "Teclas a utilizar:" << endl;
	cout << "w: avanza" << endl; // Tecla para avanzar
	cout << "s: retrocede" << endl; // Tecla para retroceder
	cout << "d: gira en sentido horario" << endl; // Tecla para girar a la derecha
	cout << "a: gira en sentido antihorario" << endl; // Tecla para girar a la izquierda
	cout << "barra espaciadora: dispara" << endl; // Tecla para disparar
	cout << "z: zoom on/off" << endl; // Tecla para activar/desactivar zoom
	cout << "b: boost" << endl; // Tecla para aumentar velocidad
}

void checkErrors() // Función para verificar errores de OpenGL
{
	int error = glGetError(); // Obtiene el último error de OpenGL
	while(error != GL_NO_ERROR) // Mientras haya errores
	{
		if (commandLineinfo) // Si se deben mostrar errores
		{
			// Verifica el tipo de error y lo imprime
			if(error == GL_INVALID_ENUM) // Error por valor de enumeración no válido
				cout << "GL_INVALID_ENUM" << endl; 
			else if(error == GL_INVALID_VALUE) // Error por valor numérico no válido
				cout << "GL_INVALID_VALUE" << endl;
			else if (error == GL_INVALID_OPERATION) // Error por operación no válida
				cout << "GL_INVALID_OPERATION" << endl;
			else if (error == GL_STACK_OVERFLOW) // Error por desbordamiento de pila
				cout << "GL_STACK_OVERFLOW" << endl;
			else if (error == GL_STACK_UNDERFLOW) // Error por subdesbordamiento de pila
				cout << "GL_STACK_UNDERFLOW" << endl;
			else if (error == GL_OUT_OF_MEMORY) // Error por falta de memoria
				cout << "GL_OUT_OF_MEMORY" << endl;
		}
		error = glGetError(); // Obtiene el siguiente error
	}
}

//--------CALLBACKS------------

void reshape_cb(int w, int h) // Función de callback para cambiar el tamaño de la ventana
{
	if (w == 0 || h == 0) return; // Evita división por cero
	glViewport(0, 0, w, h); // Establece el área de visualización
	glMatrixMode(GL_PROJECTION); // Cambia a la matriz de proyección
	glLoadIdentity(); // Carga la matriz identidad
	if (!zoom) // Si no está activado el zoom
	{
		glOrtho(0, w, 0, h, 1, 3); // Establece la proyección ortográfica
	}
	else // Si está activado el zoom
	{
		glOrtho(0 - (w / scale), w + (w / scale), 0 - (h / scale), h + (h / scale), 1, 3); // Aplica el zoom
	}
	glMatrixMode(GL_MODELVIEW); // Cambia a la matriz de modelo
	glLoadIdentity(); // Carga la matriz identidad
	gluLookAt(0, 0, 2, 0, 0, 0, 0, 1, 0); // Ubica la cámara
	glutPostRedisplay(); // Solicita la redistribución
}

void display_cb() // Función de callback para mostrar el contenido de la ventana
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpia el buffer de color y profundidad
	drawLimitsScreen(); // Dibuja los límites de la pantalla
	drawShip(); // Dibuja la nave
	drawRadar(); // Dibuja el radar
	drawShipBoard(); // Dibuja el portador de la nave
	DrawBullet(); // Dibuja las balas
	
	// Muestra la cantidad de segundos transcurridos
	char st1[30] = "Segundos Transcurridos: "; // Mensaje inicial
	char st2[10] = ""; // Buffer para segundos
	sprintf(st2, "%d", mySeconds); // Formatea los segundos como cadena
	strcat(st1, st2); // Concatenar el número de segundos 
	drawText(st1, 20.0, 550.0, 15.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0, 1.0); // Dibuja el texto en pantalla
	
	char st3[10] = "Motor: "; // Mensaje inicial para el motor
	char st4[10] = ""; // Buffer para el estado del motor
	sprintf(st4, "%d", myMotor); // Formatea el estado del motor como cadena
	strcat(st3, st4); // Concatenar el estado del motor
	
	// Determina el color basado en el valor de myMotor
	float r = 0.0, g = 0.0, b = 0.0; // Inicializa colores
	if (myMotor <= 100 && myMotor >= 51 ) 
	{
		g = 1.0; // Verde
	} 
	else if (myMotor <= 50 && myMotor >= 10) 
	{
		r = 1.0; // Amarillo (rojo + verde)
		g = 1.0;
	} 
	else if (myMotor <= 10) 
	{
		r = 1.0; // Rojo
	}
	
	drawText(st3, 20.0, 530.0, 15.0, r, g, b, 1.0, 0.0, 0, 1.0); // Dibuja el texto del motor
	
	char st5[25] = "Eric Dumortier"; // Nombre a mostrar
	sprintf(st5, "%s", "Eric Dumortier"); // Formatea el nombre (aunque no es necesario aquí)
	drawText(st5, 20.0, 570.0, 15.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0, 1.0); // Dibuja el nombre en pantalla
	
	glutSwapBuffers(); // Intercambia los buffers de doble visualización
}

void idle_cb() // Función de callback para la lógica del juego
{
	double angle = shipAngle * PI / 180.0; // Convierte shipAngle de grados a radianes
	double ang2; // Variable para el ángulo de disparo
	static unsigned int lt = 0; // Variable para el tiempo
	int dt = glutGet(GLUT_ELAPSED_TIME) - lt; // Calcula el tiempo transcurrido
	if(dt > 60) // Si ha pasado más de 60 ms
	{
		lt = glutGet(GLUT_ELAPSED_TIME); // Actualiza el tiempo
		
		AngLineRadar = (AngLineRadar + 2) % 360; // Línea del radar gira 360°
		
		// Actualiza las posiciones de las balas
		list<Bullet>::iterator b = bullet.begin();
		while(b != bullet.end()) // Si está fuera del mapa, eliminamos la bala
		{
			if(b->Update()) // Actualiza la bala
				b = bullet.erase(b); // Elimina la bala si está fuera
			else
				b++; // Avanza al siguiente
		}
		
		// Teclado
		if(keyboard.Fordward()) // Adelante
		{
			shipX -= 5 * sin(angle); // Actualiza la posición x
			shipY += 5 * cos(angle); // Actualiza la posición y
		}
		if(keyboard.Back()) // Atrás
		{
			shipX += 5 * sin(angle); // Actualiza la posición x
			shipY -= 5 * cos(angle); // Actualiza la posición y
		}
		if(keyboard.Left()) // Izquierda
		{
			shipAngle += 0.8; // Gira a la izquierda
		}
		if(keyboard.Right()) // Derecha
		{
			shipAngle -= 0.8; // Gira a la derecha
		}
		if(keyboard.Shoot()) // Dispara
		{
			ang2 = (shipAngle + machineGunAngle) * PI / 180.0; // Calcula el ángulo de disparo
			bullet.push_back(Bullet((shipX), (shipY), -50 * sin(ang2), 50 * cos(ang2))); // Crea la bala
		}
		if(keyboard.Zoom()) // Zoom
		{
			zoom = !zoom; // Alterna el estado del zoom
			if(!zoom)
			{  
				cout << "Escala natural: 1:1" << endl; // Muestra mensaje de escala natural
			}
			else 
			{
				cout << "Zoom All" << endl; // Muestra mensaje de zoom activado
			}
			// Rehace la matriz de proyección
			reshape_cb(w, h);
		}
		if(keyboard.Boost()) // Aumenta la velocidad
		{
			if(myMotor > 0) // Si el motor tiene energía
			{
				myMotor -= 1; // Reduce la energía del motor
			}
			shipX -= 10 * sin(angle); // Aumenta la velocidad en la dirección actual
			shipY += 10 * cos(angle); // Aumenta la velocidad en la dirección actual
		}
		
		// Controla que no salga de la pantalla
		if(shipX < xmin) shipX = 50; // Limite izquierdo
		if(shipX > xMax) shipX = 750; // Limite derecho
		if(shipY < yMin) shipY = 50; // Limite inferior
		if(shipY > yMax) shipY = 550; // Limite superior
	}
	glutPostRedisplay(); // Solicita la redistribución
	
	// Verifica si myMilliseconds es un múltiplo de 1000 para ejecutar lógica de temporización
	myMilliseconds = glutGet(GLUT_ELAPSED_TIME); // Obtiene el tiempo transcurrido en milisegundos
	if (myMilliseconds % 1000 == 0) // Verifica si el tiempo total es un múltiplo de 1000 (es decir, cada segundo)
	{
		mySeconds = myMilliseconds / 1000; // Calcula el número de segundos transcurridos
		if (mySeconds >= 30) // Si pasaron 30 segundos, finaliza el programa
		{
			display_cb(); // Muestra la pantalla
			cout << "Termino el tiempo!" << endl; // Mensaje de fin de tiempo
			cout << "Pasaron " << mySeconds << " segundos!" << endl; // Muestra los segundos transcurridos
			exit(EXIT_SUCCESS); // Finaliza el programa
		}
		if (myMotor == 0) // Si el motor se ha roto
		{
			display_cb(); // Muestra la pantalla
			cout << "Rompiste el motor!" << endl; // Mensaje de motor roto
			exit(EXIT_SUCCESS); // Finaliza el programa
		}
	}
}

void initialize() // Función para inicializar la ventana y el contexto de OpenGL
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // Establece el modo de visualización (RGB, doble buffer y zBuffer)
	glutInitWindowSize(800, 600); // Establece el tamaño de la ventana
	glutInitWindowPosition(500, 50); // Establece la posición de la ventana
	glutCreateWindow("TRABAJO PRACTICO NUMERO 2 MO2D"); // Crea la ventana con el título
	info(); // Muestra la información de OpenGL
	checkErrors(); // Verifica errores de OpenGL
	glutDisplayFunc(display_cb); // Establece la función de callback para mostrar el contenido
	glutReshapeFunc(reshape_cb); // Establece la función de callback para cambiar el tamaño de la ventana
	glutIdleFunc(idle_cb); // Establece la función de callback para la lógica del juego
	keyboard.InitKeyboard(); // Inicializa el teclado
	glEnable(GL_DEPTH_TEST); // Activa la prueba de profundidad
	glDepthFunc(GL_LEQUAL); // Establece la función de comparación de profundidad
	glClearColor(0.f, 0.f, 0.f, 0.f); // Establece el color de fondo a negro
}

int main (int argc, char **argv) // Función principal
{
	glutInit(&argc, argv); // Inicializa GLUT
	initialize(); // Llama a la función de inicialización
	glutMainLoop(); // Entra en el bucle principal de GLUT
	
	return 0; // Retorna 0 al finalizar
}
