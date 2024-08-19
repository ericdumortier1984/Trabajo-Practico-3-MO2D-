#include "Keyboard.h" // Incluye la definición de la clase Keyboard

bool Keyboard::keyboard[256]; // Inicializa el array estático que indica el estado de las teclas (presionadas o no)
bool Keyboard::exit; // Inicializa el booleano que indica si se presionó Alt+F4

// Constructor de la clase Keyboard que inicializa las teclas mapeadas
Keyboard::Keyboard(unsigned char shoot, unsigned char forward, unsigned char back, 
				   unsigned char left, unsigned char right, unsigned char zoom, 
				   unsigned char boost)
{
	for(int i = 0; i < 256; i++) // Itera sobre todas las teclas posibles
	{
		Keyboard::keyboard[i] = false; // Marca todas las teclas como no presionadas
		
		// Guarda el mapeo de teclas en el array 'map'
		map[0] = shoot; // Tecla para disparar
		map[1] = forward; // Tecla para avanzar
		map[2] = back; // Tecla para retroceder
		map[3] = left; // Tecla para girar a la izquierda
		map[4] = right; // Tecla para girar a la derecha
		map[5] = zoom; // Tecla para hacer zoom
		map[6] = boost; // Tecla para aumentar velocidad
	}
}

void Keyboard::InitKeyboard() // Esta función registra los callbacks para manejar eventos de teclado
{
	glutIgnoreKeyRepeat(true); // Ignora repeticiones de teclas (no registra múltiples presiones)
	glutKeyboardFunc(Keyboard::KeyPressed_cb); // Registra el callback para teclas presionadas
	glutKeyboardUpFunc(Keyboard::KeyRelease_cb); // Registra el callback para teclas liberadas
	glutSpecialFunc(Keyboard::Special_cb); // Registra el callback para teclas especiales
}

void Keyboard::KeyPressed_cb(unsigned char key, int x, int y) // Callback que se llama cuando una tecla es presionada
{
	Keyboard::keyboard[key] = true; // Marca la tecla 'key' como presionada en el array 'keyboard'
	if(key == 27) // Si la tecla presionada es ESC (código 27)
	{
		exit = true; // Marca la variable 'exit' como verdadera
	}
}

void Keyboard::KeyRelease_cb(unsigned char key, int x, int y) // Callback que se llama cuando una tecla es liberada
{
	Keyboard::keyboard[key] = false; // Marca la tecla 'key' como no presionada en el array 'keyboard'
}

void Keyboard::Special_cb(int key, int xmodif, int ymodif) // Callback para manejar teclas especiales (como F4)
{
	if(key == GLUT_KEY_F4 && glutGetModifiers() == GLUT_ACTIVE_ALT) // Verifica si se presiona Alt + F4
	{
		exit = true; // Marca la variable 'exit' como verdadera
	}
}

// Métodos que verifican si las teclas específicas están presionadas
bool Keyboard::Shoot() 
{
	return keyboard[map[0]]; // Retorna verdadero si la tecla de disparo está presionada
}
bool Keyboard::Fordward()
{
	return keyboard[map[1]]; // Retorna verdadero si la tecla de avanzar está presionada
}
bool Keyboard::Back()
{
	return keyboard[map[2]]; // Retorna verdadero si la tecla de retroceder está presionada
}
bool Keyboard::Left()
{
	return keyboard[map[3]]; // Retorna verdadero si la tecla de girar a la izquierda está presionada
}
bool Keyboard::Right() 
{
	return keyboard[map[4]]; // Retorna verdadero si la tecla de girar a la derecha está presionada
}
bool Keyboard::Zoom()
{
	return keyboard[map[5]]; // Retorna verdadero si la tecla de zoom está presionada
}
bool Keyboard::Boost()
{
	return keyboard[map[6]]; // Retorna verdadero si la tecla de aumentar velocidad está presionada
}
bool Keyboard::Exit() 
{
	return exit; // Retorna el estado de la variable 'exit'
}
