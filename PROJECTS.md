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

- **Repositorio:** [srampello/RMP_Deimonyag](https://github.com/srampello/RMP_Deimonyag)
- **Visor de hardware:** [srampello.github.io/RMP_Deimonyag](https://srampello.github.io/RMP_Deimonyag/)

### UNICUM

Seguidor de línea con doce sensores analógicos conectados directamente, sin multiplexor.

| Área | Configuración actual |
|---|---|
| Controlador | ESP32-WROOM / ESP32S con Wi-Fi y Bluetooth 4.2 |
| Sensores | 12 × QRE1113GR, arquitectura inspirada en QTR-8A |
| Driver | TB6612FNG |
| Tracción | 2 × motores Pololu de 6 V |
| Energía | LiPo 2S |
| Interfaz | Tres botones para estrategias, calibración y configuración |
| Control | Calibración, estrategias y PD |
| Estado | Base eléctrica, firmware y documentación v0.1 |

La lectura utiliza ADC1 y ADC2. Como ADC2 no está disponible con Wi-Fi activo en este ESP32, el Wi-Fi se utiliza solamente durante la configuración con el robot detenido y se apaga antes de leer los doce sensores o iniciar la marcha.

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
- **Página del robot:** [RMP Robotics — LEA_PAREDES](https://rmp-robotics.santii-rampe.chatgpt.site/robots/lea-paredes)

### AUS_KIM

Robot de laberinto prestado y utilizado como plataforma de aprendizaje, diagnóstico y desarrollo de navegación.

| Área | Configuración actual |
|---|---|
| Controlador | ESP32-S3 SuperMini |
| Sensores | 4 × Sharp GP2Y0E03 |
| Driver | DRV8833 |
| Tracción | 2 × micromotores con encoders de cuadratura |
| Energía | LiPo CNHL Pizza V2 2S 250 mAh |
| Software | Test Suite web, PID lateral, pruebas de sensores/motores/encoders y Race Control |
| Navegación | Seguimiento de pared derecha y maniobras temporizadas |
| Estado | Calibración y pruebas activas en laberinto |

El Test Suite es la fuente de verdad del firmware. La versión Race se genera automáticamente y comparte la misma lógica de sensores, motores y navegación.

- **Repositorio:** [srampello/AUS_KIM](https://github.com/srampello/AUS_KIM)

## Comparación general

| Proyecto | Categoría | MCU | Sensores principales | Driver |
|---|---|---|---|---|
| Deimonyag | Seguidor con succión | ESP32-C3 | 12 QRE1113GR + MUX | IFX9201SG |
| UNICUM | Seguidor sin MUX | ESP32-WROOM | 12 QRE1113GR directos | TB6612FNG |
| LEA_PAREDES | Micromouse propio | ESP32-C3 | 4 pares IR | TB6612FNG |
| AUS_KIM | Micromouse de pruebas | ESP32-S3 | 4 Sharp GP2Y0E03 | DRV8833 |

## Alta de un nuevo proyecto

1. Crear un repositorio independiente para el robot.
2. Usar la estructura disponible en [templates/robot-project](templates/robot-project/README.md).
3. Completar una ficha técnica a partir de [templates/FICHA_TECNICA.md](templates/FICHA_TECNICA.md).
4. Añadir el proyecto a este catálogo y a la tabla principal de [README.md](README.md).
5. Mantener en este repositorio solamente los recursos realmente reutilizables.
6. Guardar cada archivo `.ino` dentro de una carpeta con exactamente el mismo nombre.
