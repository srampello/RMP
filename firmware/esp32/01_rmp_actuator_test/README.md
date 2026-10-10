# RMP ESP32 Universal Test Bench

Firmware genérico para probar actuadores y sensores de distintos robots sin recompilar cada vez que cambia el pinout.

## Compatibilidad

- ESP32-C3 Super Mini
- ESP32-S3 SuperMini
- Arduino IDE con el paquete **ESP32 by Espressif Systems**
- No requiere librerías externas

## Funciones

La interfaz web utiliza la identidad visual de RMP Robotics: negro, violeta y blanco.

### 1. Configuración

Permite elegir y guardar en la memoria del ESP32:

- MUX CD74HC4067:
  - S0, S1, S2 y S3
  - SIG / ADC
  - EN opcional
  - 4, 8, 12 o 16 canales
  - cantidad de muestras y período de actualización
- LED:
  - GPIO
  - activo en HIGH o LOW
- Botón:
  - GPIO
  - entrada flotante, INPUT_PULLUP o INPUT_PULLDOWN
  - activo en HIGH o LOW
- Dos motores:
  - GPIO de control
  - inversión individual
  - ENABLE / STBY opcional
  - tres tipos de driver

La configuración se almacena mediante Preferences y permanece después de apagar el ESP32.

### 2. Actuadores

- Encender y apagar el LED.
- Ver en tiempo real el estado del botón físico y contar pulsaciones.
- Probar cada motor por separado.
- Mover ambos motores hacia adelante, atrás o girar.
- Ajustar el PWM de cada motor entre 0 y 255.

Los botones de movimiento son momentáneos: el motor funciona mientras se mantiene presionado. Además, el firmware detiene los motores si deja de recibir comandos durante 1,5 segundos.

### 3. Sensores MUX

- Lectura simultánea de hasta 16 canales.
- Conservación del valor ADC crudo de 12 bits, entre 0 y 4095.
- Conversión de cada lectura a un nivel de blanco entre 0 y 1000: `0 = negro` y `1000 = blanco`.
- Valor normalizado, ADC crudo y clasificación BLANCO/NEGRO para cada sensor.
- Umbral de separación blanco/negro configurable entre 0 y 1000.
- Polaridad configurable: se indica si el blanco produce un ADC alto o bajo.
- Orden físico configurable: S0 puede mostrarse a la izquierda o a la derecha.
- Marcadores de izquierda, centro y derecha para localizar la línea sobre la barra.
- Métricas de máximo, mínimo, promedio y rango sobre la escala 0–1000.
- Perfil instantáneo de todos los canales mediante un gráfico de barras con línea de umbral.
- Historial temporal normalizado de dos canales seleccionables.
- Controles para pausar, reanudar y limpiar el historial.

Si el blanco produce un ADC alto, el monitor usa `blanco = ADC × 1000 / 4095`. Si produce un ADC bajo, invierte el resultado con `blanco = 1000 - (ADC × 1000 / 4095)`. El umbral, la polaridad y el orden físico se guardan en el navegador; no modifican la lectura ADC original enviada por el ESP32.

## Modos de motor

| Modo | Pin A | Pin B | Pin PWM adicional | Ejemplos |
|---|---|---|---|---|
| IN1 + IN2 | IN1 | IN2 | No usado | DRV8833, RZ7889 |
| PWM + DIR | PWM | DIR | No usado | IFX9201, BTN9960 |
| IN1 + IN2 + PWM | IN1 | IN2 | PWM | TB6612FNG |

En un TB6612FNG se debe colocar el pin STBY en el campo ENABLE / STBY y mantener la opción **activo en HIGH**.

## Puesta en marcha

1. Abrir `01_rmp_actuator_test.ino` en Arduino IDE.
2. Seleccionar la placa ESP32-C3 o ESP32-S3 correspondiente.
3. Compilar y cargar el firmware.
4. Conectarse a la red:

   - **SSID:** RMP_TEST
   - **Clave:** RMP2026

5. Abrir http://192.168.4.1.
6. Completar todos los GPIO en la primera pestaña.
7. Usar -1 únicamente para EN del MUX o ENABLE / STBY cuando no estén conectados.
8. Guardar. El ESP32 se reinicia y conserva el pinout.

## Consideraciones

- El pin SIG debe ser un GPIO con capacidad ADC en la placa utilizada.
- No repetir GPIO entre funciones.
- Revisar los pines de arranque, USB y UART antes de asignarlos.
- El ESP32 entrega señales lógicas; los motores siempre deben conectarse mediante su driver.
- Todas las masas deben estar en común.
- La interfaz permite comprobar el botón; un botón físico no puede accionarse eléctricamente desde el ESP32.
- Para cambiar de robot, solo se modifica la primera pestaña y se guarda nuevamente.
