# Convenciones de RMP Robotics

## Idioma

La documentación general se escribe en español. Los nombres técnicos establecidos, comandos y APIs pueden conservarse en inglés.

## Nombres

- Carpetas: minúsculas y guiones, por ejemplo `prueba-motores`.
- Sketch de Arduino: carpeta y archivo con el mismo nombre.
- Documentos principales: mayúsculas, por ejemplo `README.md`, `CHANGELOG.md`.
- Evitar espacios y caracteres especiales en rutas nuevas.

## Firmware genérico

Cada prueba debe incluir:

1. Objetivo.
2. Hardware compatible.
3. Diagrama o tabla de conexiones.
4. Bloque de configuración visible al comienzo.
5. Procedimiento de prueba.
6. Resultado esperado.
7. Advertencias eléctricas.
8. Fecha o versión verificada.

Los pines y parámetros deben concentrarse en una sección de configuración. Ningún firmware genérico debe presentarse como universal si depende de un driver o sensor particular.

## Estados sugeridos

- **Idea:** todavía sin diseño validado.
- **En desarrollo:** hardware o firmware en construcción.
- **En pruebas:** prototipo funcional bajo calibración.
- **Competición:** configuración utilizada en pista.
- **Estable:** versión verificada y documentada.
- **Archivado:** proyecto conservado como referencia.

## Commits

Usar mensajes breves en infinitivo:

- `Agregar prueba genérica de encoders`
- `Actualizar ficha de AUS_KIM`
- `Corregir conexiones del TB6612FNG`
