# Arquitectura de repositorios

## Objetivo

Evitar que el repositorio central se convierta en una copia desactualizada de cada robot. RMP actúa como portal y biblioteca compartida; los repositorios de proyecto son la fuente de verdad de cada robot.

## Modelo

```text
RMP
├── catálogo de robots
├── firmware genérico
├── herramientas comunes
├── referencias de hardware
└── plantillas
    ├── RMP_Deimonyag
    ├── AUS_KIM
    └── RMP_Lea_Paredes
```

Los enlaces representan relación documental, no submódulos Git. De esta manera cada proyecto puede evolucionar con independencia y el repositorio central sigue siendo sencillo de mantener.

## Regla práctica

Un archivo pertenece a **RMP** si puede utilizarse sin conocer un robot concreto o si documenta a la organización completa.

Un archivo pertenece al **repositorio del robot** si contiene pines, medidas, calibraciones, parámetros, piezas o decisiones específicas de ese proyecto.

## Versionado recomendado

- Usar ramas cortas para cambios experimentales importantes.
- Mantener el firmware estable identificable en la rama principal.
- Registrar cambios relevantes en un `CHANGELOG.md`.
- Etiquetar hitos funcionales con versiones, por ejemplo `v0.1.0`.
- No copiar firmware final entre repositorios: extraer solamente la parte reutilizable y documentar sus supuestos.
