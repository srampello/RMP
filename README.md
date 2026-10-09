# RMP Robotics

Repositorio central de **RMP Robotics**: punto de entrada a todos los robots, documentación común, firmware de pruebas genérico, herramientas y plantillas reutilizables.

## Proyectos

| Robot | Categoría | Controlador | Estado | Repositorio |
|---|---|---|---|---|
| **Deimonyag** | Seguidor de línea con succión | ESP32-C3 Super Mini | En desarrollo | [RMP_Deimonyag](https://github.com/srampello/RMP_Deimonyag) |
| **AUS_KIM** | Micromouse / plataforma de pruebas | ESP32-S3 SuperMini | Pruebas y navegación | [AUS_KIM](https://github.com/srampello/AUS_KIM) |
| **LEA_PAREDES** | Micromouse de competición | ESP32-C3 Super Mini | Diseño mecánico y electrónico | [RMP_Lea_Paredes](https://github.com/srampello/RMP_Lea_Paredes) |

La ficha ampliada de cada proyecto está en [PROJECTS.md](PROJECTS.md).

## Recursos compartidos

- [Firmware genérico](firmware/README.md): pruebas reutilizables de motores, sensores, encoders, PWM, Wi-Fi y telemetría.
- [Hardware](hardware/README.md): criterios comunes, referencias y módulos usados en distintos robots.
- [Herramientas](tools/README.md): utilidades de PC, scripts y recursos de diagnóstico.
- [Plantillas](templates/robot-project/README.md): estructura recomendada para iniciar un nuevo robot.
- [Convenciones](docs/CONVENCIONES.md): nombres, versiones, carpetas y documentación.
- [Arquitectura del repositorio](docs/ARQUITECTURA_REPOSITORIOS.md): qué se guarda aquí y qué permanece en cada proyecto.

## Estructura

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

**En RMP central**

- Firmware de prueba que pueda reutilizarse en más de un robot.
- Plantillas de proyectos, fichas técnicas y documentación.
- Herramientas de diagnóstico y telemetría genéricas.
- Referencias de componentes y buenas prácticas compartidas.
- Índice y estado general de todos los proyectos.

**En el repositorio de cada robot**

- Firmware final y configuración específica.
- Pinout definitivo.
- Esquemáticos, PCB, CAD, STL y BOM.
- Telemetría y calibraciones propias.
- Historial de pruebas y versiones del robot.

## Enlaces rápidos

- [Deimonyag — documentación y firmware](https://github.com/srampello/RMP_Deimonyag)
- [Deimonyag — visor web](https://srampello.github.io/RMP_Deimonyag/)
- [AUS_KIM — documentación y firmware](https://github.com/srampello/AUS_KIM)
- [LEA_PAREDES — documentación, hardware y diseño 3D](https://github.com/srampello/RMP_Lea_Paredes)

## Próximos pasos

- Incorporar los primeros firmwares genéricos, separando controlador, driver y sensores.
- Crear una base común de telemetría Wi-Fi para ESP32-C3 y ESP32-S3.
- Normalizar las fichas técnicas de los tres robots.
- Agregar nuevos proyectos sin duplicar su contenido técnico.
