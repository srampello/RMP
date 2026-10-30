# Proyectos de RMP Robotics

Catálogo técnico de los proyectos activos. La documentación completa y los archivos de cada robot se mantienen en su repositorio correspondiente.

## Seguidores de línea

### Deimonyag

Seguidor de línea de competición con sistema de succión y telemetría.

| Área | Configuración actual |
|---|---|
| Controlador | ESP32-C3 Super Mini |
| Sensores | 12 × QRE1113GR mediante CD74HC4067 |
| Tracción | 2 × JSumo ProFast 12 V 3600 RPM |
| Drivers | 2 × IFX9201SG |
| Succión | EDF27 brushless con ESC |
| Energía | LiPo 3S 450 mAh + Matek MICRO BEC a 5 V |
| Software | Firmware Wi-Fi, aplicación Python, WebSocket y registro CSV |
| Estado | Hardware, firmware y telemetría en desarrollo |

- **Ficha técnica:** [srampello.github.io/RMP/robots/deimonyag](https://srampello.github.io/RMP/robots/deimonyag/)
- **Repositorio:** [srampello/RMP_Deimonyag](https://github.com/srampello/RMP_Deimonyag)
- **Visor de hardware:** [srampello.github.io/RMP_Deimonyag](https://srampello.github.io/RMP_Deimonyag/)

### UNICUM

Seguidor de línea compacto con barra recta de doce sensores, multiplexor, selector de perfiles por DIP switch y largada con MicroStart.

| Área | Configuración actual |
|---|---|
| Controlador | ESP32-C3 Super Mini |
| Sensores | 12 × QRE1113GR con 74HC4067, lectura por ADC1 |
| Driver | TB6612FNG |
| Tracción | 2 × N20 6 V 2000 RPM |
| Energía | LiPo 2S con reguladores propuestos de 6 V (motores) y 5 V (placa) |
| Interfaz | DIP switch de 3 vías para perfiles y MicroStart JSumo |
| Control | PD con período de 5 ms, calibración guardada y telemetría web |
| Estado | Firmware v0.2; PCB todavía sin rutear |

La v0.2 reemplaza a la versión ESP32-WROOM con sensores directos; su cableado no es compatible.

- **Ficha técnica:** [srampello.github.io/RMP/robots/unicum](https://srampello.github.io/RMP/robots/unicum/)
- **Repositorio:** [srampello/RMP_unicum](https://github.com/srampello/RMP_unicum)

## Micromouse y laberinto

### LEA_PAREDES

Micromouse propio de competición desarrollado por RMP Robotics.

| Área | Configuración actual |
|---|---|
| Controlador | ESP32-C3 Super Mini |
| Sensores | 4 pares infrarrojos: 2 frontales y 2 laterales |
| Driver | TB6612FNG |
| Tracción | 2 × motores Pololu 1000 RPM, actualmente sin encoders |
| Succión | Motor coreless 8520 con turbina de 30 mm |
| Energía | LiPo 2S + Matek MICRO BEC a 5 V |
| Mecánica | Base impresa en 3D y PCB superior |
| Estado | Diseño mecánico, electrónico, PCB y navegación inicial |

La estrategia inicial sigue la pared derecha con detección frontal y giros temporizados, dejando preparada una evolución posterior hacia navegación con mapa y encoders.

- **Repositorio:** [srampello/RMP_Lea_Paredes](https://github.com/srampello/RMP_Lea_Paredes)
- **Ficha técnica:** [srampello.github.io/RMP/robots/lea-paredes](https://srampello.github.io/RMP/robots/lea-paredes/)

### AUS_KIM

Robot de laberinto prestado y utilizado como plataforma de aprendizaje, diagnóstico y desarrollo de navegación.

| Área | V1 (probada) | V2 (en calibración) |
|---|---|---|
| Controlador | ESP32-S3 SuperMini | ESP32-C3 Super Mini |
| Sensores | 4 × Sharp GP2Y0E03 | 3 × Sharp GP2Y0E03 + QRE1113GR de piso |
| Driver | DRV8833 | DRV8833 con jumper JP1 de habilitación |
| Tracción | Micromotores con encoder (solo diagnóstico) | 2 × N20 1500 RPM con encoder A/B |
| Energía | LiPo 2S 250 mAh + BEC | LiPo 2S + BEC 5 V |
| Navegación | Pared derecha con PID y giros temporizados | Prevista: mapeo con BFS / Flood Fill y encoders |

El Test Suite es la fuente de verdad del firmware V1. La versión Race se genera automáticamente y comparte la misma lógica de sensores, motores y navegación. La V2 tiene su propio sketch de calibración.

- **Ficha técnica:** [srampello.github.io/RMP/robots/aus-kim](https://srampello.github.io/RMP/robots/aus-kim/)
- **Repositorio:** [srampello/AUS_KIM](https://github.com/srampello/AUS_KIM)

## Comparación general

| Proyecto | Categoría | MCU | Sensores principales | Driver |
|---|---|---|---|---|
| Deimonyag | Seguidor con succión | ESP32-C3 | 12 QRE1113GR + MUX | IFX9201SG |
| UNICUM | Seguidor con MUX | ESP32-C3 | 12 QRE1113GR + MUX | TB6612FNG |
| LEA_PAREDES | Micromouse propio | ESP32-C3 | 4 pares IR | TB6612FNG |
| AUS_KIM | Micromouse de pruebas | ESP32-C3 (V2) | 3 Sharp GP2Y0E03 + piso | DRV8833 |

## Alta de un nuevo proyecto

1. Crear un repositorio independiente para el robot.
2. Usar la estructura disponible en [templates/robot-project](templates/robot-project/README.md).
3. Completar una ficha técnica a partir de [templates/FICHA_TECNICA.md](templates/FICHA_TECNICA.md).
4. Añadir el proyecto a este catálogo y a la tabla principal de [README.md](README.md).
5. Mantener en este repositorio solamente los recursos realmente reutilizables.
6. Guardar cada archivo `.ino` dentro de una carpeta con exactamente el mismo nombre.
