# Proyectos de RMP Robotics

Este archivo funciona como catálogo general. La documentación completa y los archivos de cada robot se mantienen en su repositorio correspondiente.

## Deimonyag

Robot seguidor de línea de competición con sistema de succión.

- **Controlador:** ESP32-C3 Super Mini
- **Sensores:** barra de QRE1113GR mediante CD74HC4067
- **Tracción:** dos motores JSumo ProFast
- **Drivers:** IFX9201SG
- **Succión:** EDF27 brushless con ESC
- **Batería:** LiPo 3S
- **Repositorio:** [srampello/RMP_Deimonyag](https://github.com/srampello/RMP_Deimonyag)
- **Visor web:** [srampello.github.io/RMP_Deimonyag](https://srampello.github.io/RMP_Deimonyag/)

## AUS_KIM

Micromouse prestado utilizado como plataforma de aprendizaje, diagnóstico y desarrollo de navegación.

- **Controlador:** ESP32-S3 SuperMini
- **Sensores:** cuatro Sharp GP2Y0E03
- **Driver:** DRV8833
- **Motores:** dos micromotores con encoders
- **Batería:** LiPo 2S
- **Áreas de trabajo:** pruebas Wi-Fi, sensores, encoders, PID de pared y resolución de laberinto
- **Repositorio:** [srampello/AUS_KIM](https://github.com/srampello/AUS_KIM)

## LEA_PAREDES

Micromouse de competición desarrollado por RMP Robotics.

- **Controlador:** ESP32-C3 Super Mini
- **Sensores:** cuatro pares IR
- **Driver:** TB6612FNG
- **Motores:** dos Pololu 1000 RPM
- **Succión:** motor coreless 8520 con turbina de 30 mm
- **Batería:** LiPo 2S
- **Áreas de trabajo:** PCB, mecánica, impresión 3D, firmware y navegación
- **Repositorio:** [srampello/RMP_Lea_Paredes](https://github.com/srampello/RMP_Lea_Paredes)

## Alta de un nuevo proyecto

1. Crear un repositorio independiente para el robot.
2. Usar la estructura disponible en [templates/robot-project](templates/robot-project/README.md).
3. Completar una ficha técnica a partir de [templates/FICHA_TECNICA.md](templates/FICHA_TECNICA.md).
4. Añadir el proyecto a este catálogo y a la tabla principal de [README.md](README.md).
5. Mantener en este repositorio solamente los recursos que sean realmente reutilizables.
