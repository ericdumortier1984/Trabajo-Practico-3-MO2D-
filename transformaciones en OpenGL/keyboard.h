#include <GL/gl.h>      // Incluye la biblioteca OpenGL para gr�ficos
#include <GL/glu.h>     // Incluye la biblioteca GLU para utilidades de OpenGL
#include <GL/glut.h>    // Incluye GLUT para la creaci�n de ventanas y manejo de eventos
#include <iostream>      // Incluye la biblioteca est�ndar de entrada/salida

class Keyboard // Clase Keyboard para manejar la entrada del teclado
{
private:
	static bool keyboard[256]; // Array est�tico que indica si cada tecla (0-255) est� presionada
	static bool exit; // Indica si se presion� la combinaci�n Alt+F4
	unsigned char map[8]; // Mapa de teclas personalizadas (8 teclas)
	
public:
	// Constructor que inicializa el mapa de teclas con los caracteres dados
	Keyboard(unsigned char key1, unsigned char key2, unsigned char key3, 
			 unsigned char key4, unsigned char key5, unsigned char key6, 
			 unsigned char key7, unsigned char key8);
	
	void InitKeyboard(); // M�todo para inicializar el estado del teclado
	
	// M�todos de callback para manejar eventos de teclado
	static void KeyPressed_cb(unsigned char key, int x, int y); // Callback para tecla presionada
	static void KeyRelease_cb(unsigned char key, int x, int y); // Callback para tecla liberada
	static void Special_cb(int key, int xmodif, int ymodif); // Callback para teclas especiales (como flechas)
	
	int xmodif = 0; // Modificaci�n de posici�n en x
	int ymodif = 0; // Modificaci�n de posici�n en y
	
	// M�todos para verificar el estado de las teclas
	bool Shoot(); // Verifica si se debe disparar
	bool Fordward(); // Verifica si se debe avanzar
	bool Back(); // Verifica si se debe retroceder
	bool Left(); // Verifica si se debe girar a la izquierda
	bool Right(); // Verifica si se debe girar a la derecha
	bool Zoom(); // Verifica si se debe hacer zoom
	bool Boost(); // Verifica si se debe aumentar la velocidad
	bool Lights(); // Verifica si se deben encender los faros de la nave
	bool Exit(); // Verifica si se debe salir del programa
};
