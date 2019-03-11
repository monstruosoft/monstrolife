:octocat::octocat::octocat: To read the english version of this document [click here](./README.md). :octocat::octocat::octocat:
- - -
# monstrolife
*monstrolife* es una versión básica en Allegro 5 del juego de la vida de Conway. *monstrolife* fue programado originalmente en 24 horas para un reto de programación.

![monstrolife](./data/monstrolife-1.gif)

![monstrolife](./data/monstrolife-2.gif)

- - -
## Características
* Modo interactivo usando el mouse.
* Patrones preconfigurados (usa la tecla **P**).
* Soporte para cargar patrones RLE desde la línea de comandos (usa la tecla **P** para usar el patrón *Custom*).

## Compilar
En **Linux**, el archivo `CMakeLists.txt` incluído debería ser suficiente para compilar el proyecto si se encuentran instaladas las librerías requeridas.

* Asegúrate de tener los siguientes paquetes instalados:
  * **CMake**
  * **Allegro 5** versión para desarrolladores
* Crea una carpeta con el nombre *build* dentro de la carpeta del proyecto y compila usando *CMake + make*:
```
monstruosoft@PC:~$ cd monstrolife
monstruosoft@PC:~/monstrolife$ mkdir build
monstruosoft@PC:~/monstrolife$ cd build
monstruosoft@PC:~/monstrolife/build$ cmake ..
monstruosoft@PC:~/monstrolife/build$ make
```

En **Windows** debería ser posible compilar el juego usando *CMake y MinGW* pero buena suerte con eso ya que yo no puedo probar a compilarlo en Windows.

