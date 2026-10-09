# Firmware genérico

Biblioteca de sketches y bases reutilizables para pruebas de banco. El firmware final de cada robot permanece en su repositorio específico.

## Firmwares disponibles

- [RMP ESP32 Universal Test Bench](esp32/01_rmp_actuator_test/): banco web configurable para probar MUX CD74HC4067, LED, botón y dos motores en ESP32-C3/S3 Super Mini.

## Organización prevista

```text
firmware/
├── esp32/
│   ├── gpio/
│   ├── motores/
│   ├── encoders/
│   ├── sensores/
│   ├── wifi/
│   └── telemetria/
├── arduino-nano/
│   ├── motores/
│   ├── sensores/
│   └── botones/
└── README.md
```

## Qué debe ser genérico

Un sketch puede entrar en esta biblioteca cuando:

- tiene un bloque claro de pines y parámetros;
- no contiene geometría ni calibraciones de un robot particular;
- documenta el driver o sensor compatible;
- puede probarse de forma independiente;
- incluye instrucciones y resultado esperado.

## Qué no debe copiarse aquí

- PID definitivo de un robot.
- Mapa de pines final de una PCB.
- Umbrales obtenidos para un sensor montado en una posición específica.
- Estrategias de competición.
- Interfaces que dependan de un protocolo exclusivo del proyecto.

## Firmwares candidatos para incorporar

- Prueba de dos motores con DRV8833.
- Prueba de dos motores con TB6612FNG.
- Lectura de encoder en cuadratura.
- Lectura ADC filtrada.
- Barrido de multiplexor CD74HC4067.
- Prueba de PWM y rampa de aceleración.
- Punto de acceso Wi-Fi con panel mínimo.
- Base de telemetría por WebSocket.
- Botón de inicio, MicroStart y parada controlada.

Cada firmware nuevo debe tener su propia carpeta y un `README.md`.
