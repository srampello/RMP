# RMP Robotics

Repositorio central de **RMP Robotics**. Funciona como índice de todos los robots, punto de acceso a sus repositorios y biblioteca de firmware, documentación y herramientas reutilizables.

**Sitio web:** [srampello.github.io/RMP](https://srampello.github.io/RMP/)

## Mapa de proyectos

### Seguidores de línea

| Robot | Enfoque | Controlador | Sensores | Estado | Repositorio |
|---|---|---|---|---|---|
| [**Deimonyag**](https://srampello.github.io/RMP/robots/deimonyag/) | Competición con sistema de succión | ESP32-C3 Super Mini | 12 × QRE1113GR con CD74HC4067 | Hardware, firmware Wi-Fi y telemetría en desarrollo | [RMP_Deimonyag](https://github.com/srampello/RMP_Deimonyag) |
| [**UNICUM**](https://srampello.github.io/RMP/robots/unicum/) | Seguidor compacto con selector DIP y MicroStart | ESP32-C3 Super Mini | 12 × QRE1113GR con 74HC4067 | Firmware v0.2, PCB sin rutear | [RMP_unicum](https://github.com/srampello/RMP_unicum) |

### Micromouse y laberinto

| Robot | Enfoque | Controlador | Sensores | Estado | Repositorio |
|---|---|---|---|---|---|
| [**LEA_PAREDES**](https://srampello.github.io/RMP/robots/lea-paredes/) | Micromouse propio de competición | ESP32-C3 Super Mini | 4 pares IR | Diseño mecánico, electrónico y firmware | [RMP_Lea_Paredes](https://github.com/srampello/RMP_Lea_Paredes) |
| [**AUS_KIM**](https://srampello.github.io/RMP/robots/aus-kim/) | Plataforma prestada de aprendizaje y navegación | ESP32-C3 Super Mini (V2) | 3 × Sharp GP2Y0E03 + QRE1113GR de piso | V1 probada en laberinto, V2 en calibración | [AUS_KIM](https://github.com/srampello/AUS_KIM) |

La información ampliada de cada proyecto está en su ficha técnica del [sitio web](https://srampello.github.io/RMP/) y en [PROJECTS.md](PROJECTS.md). El firmware final, los esquemáticos, el CAD y las calibraciones permanecen en el repositorio de cada robot.

## Accesos destacados

| Robot | Ficha técnica | Repositorio | Otros |
|---|---|---|---|
| Deimonyag | [Ficha](https://srampello.github.io/RMP/robots/deimonyag/) | [RMP_Deimonyag](https://github.com/srampello/RMP_Deimonyag) | [Visor de hardware](https://srampello.github.io/RMP_Deimonyag/) |
| UNICUM | [Ficha](https://srampello.github.io/RMP/robots/unicum/) | [RMP_unicum](https://github.com/srampello/RMP_unicum) | |
| LEA_PAREDES | [Ficha](https://srampello.github.io/RMP/robots/lea-paredes/) | [RMP_Lea_Paredes](https://github.com/srampello/RMP_Lea_Paredes) | |
| AUS_KIM | [Ficha](https://srampello.github.io/RMP/robots/aus-kim/) | [AUS_KIM](https://github.com/srampello/AUS_KIM) | |

## Sitio web

Hay dos niveles de páginas, todas generadas desde `tools/web/` con la misma estética (negro, blanco y violeta RMP):

| Página | Dirección | Contenido |
|---|---|---|
| Portada | [srampello.github.io/RMP](https://srampello.github.io/RMP/) | Tarjetas de todos los robots |
| Ficha de cada robot | `srampello.github.io/RMP/robots/<robot>/` | Resumen e informe técnico |
| Sitio de cada robot | `srampello.github.io/<repo del robot>/` | Vitrina por sistemas e informe técnico |

Para regenerar todo, con los repositorios de los robots clonados al lado de este (`../RMP_Deimonyag`, `../AUS_KIM`, etc.):

```bash
python tools/web/build_site.py
```

| Archivo | Contenido |
|---|---|
| `tools/web/src/index.html` | Portada |
| `tools/web/src/robots/<robot>.html` | Informe técnico: datos del hero en el comentario inicial y secciones |
| `tools/web/src/sites/<robot>.html` | Vitrina del sitio propio: logo, frase y secciones con cifras |
| `tools/web/drawings.py` | Dibujos técnicos en SVG de la vista superior de cada robot |
| `tools/web/robot_site.py` | Plantilla del sitio propio de cada robot |
| `assets/css`, `assets/js`, `assets/models` | Estilos, scripts y modelos 3D que usan todas las páginas |

El informe técnico se escribe una sola vez y aparece en la ficha y en el sitio del robot. Los sitios de los robots toman estilos y modelos de este repositorio, así que un cambio de diseño acá se ve en todos. No editar a mano los HTML generados: después de regenerar, hacer commit en cada repositorio que haya cambiado.

Para agregar un robot: crear sus archivos en `src/robots/` y `src/sites/`, sumarlo a `ORDER` en `build_site.py` y agregar su tarjeta en `src/index.html`.

## Recursos compartidos

| Sección | Contenido |
|---|---|
| [Firmware genérico](firmware/README.md) | Pruebas reutilizables de motores, sensores, encoders, PWM, Wi-Fi y telemetría |
| [Hardware](hardware/README.md) | Criterios comunes, referencias y módulos utilizados en varios robots |
| [Herramientas](tools/README.md) | Scripts, utilidades de PC y recursos de diagnóstico |
| [Plantillas](templates/robot-project/README.md) | Estructura recomendada para iniciar un robot nuevo |
| [Convenciones](docs/CONVENCIONES.md) | Nombres, versiones, carpetas y estructura obligatoria de sketches Arduino |
| [Arquitectura](docs/ARQUITECTURA_REPOSITORIOS.md) | Separación entre el repositorio central y cada proyecto |

## Firmware genérico disponible

- [RMP ESP32 Universal Test Bench](firmware/esp32/01_rmp_actuator_test/): banco web configurable para probar MUX CD74HC4067, LED, botón, motores y una barra de hasta 16 sensores.

## Estructura del repositorio central

```text
RMP/
├── README.md
├── PROJECTS.md
├── docs/
├── firmware/
├── hardware/
├── tools/
└── templates/
```

## Criterio de organización

### En RMP central

- Índice y estado general de todos los robots.
- Firmware de prueba aplicable a más de un proyecto.
- Plantillas, fichas técnicas y convenciones.
- Herramientas comunes de diagnóstico y telemetría.
- Referencias de componentes y buenas prácticas compartidas.

### En el repositorio de cada robot

- Firmware final y configuración específica.
- Pinout definitivo y calibraciones.
- Esquemáticos, PCB, CAD, STL y BOM.
- Telemetría, resultados de pruebas e historial de versiones.
- Documentación técnica propia del proyecto.

## Incorporación de proyectos

Todo robot nuevo debe crearse en un repositorio independiente, completar su documentación y luego añadirse a [PROJECTS.md](PROJECTS.md) y a este índice. Los archivos `.ino` deben estar dentro de una carpeta con exactamente el mismo nombre del sketch.
