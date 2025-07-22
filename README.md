# KIDDIE 🎮
**Sistema de Control para Máquinas Recreativas Infantiles**

KIDDIE es un sistema de control completo diseñado específicamente para máquinas recreativas infantiles que operan con monedas. El proyecto integra control de audio, iluminación, motores y gestión de créditos en una solución robusta y fácil de implementar.

## ✨ Características principales

### 🎵 Sistema de Audio
- Reproducción de múltiples canciones temáticas
- Selector de canción en tiempo real
- Control de volumen y duración personalizable
- Soporte para módulos MP3 (DFPlayer Mini)

### 💰 Gestión de Monedas
- Detección automática de monedas con protección anti-rebote
- Contador de créditos persistente (EEPROM)
- Sistema de créditos por juego configurable

### 💡 Control de Iluminación
- Secuencias de luces programables
- Patrones de parpadeo sincronizados
- LED3 con ciclo de 0.5 minutos (encendido/apagado)
- Iluminación sincronizada con estado del motor

### ⚙️ Control de Hardware
- Gestión de motores con arranque/parada automática
- Configuración mediante DIP switches
- Botones de control con realimentación
- Tiempos de juego variables

### 🔧 Configuración Flexible
- Parámetros ajustables por DIP switches
- Tiempos de juego personalizables
- Fácil adaptación a diferentes tipos de máquinas

## 🛠️ Tecnologías utilizadas

- **Lenguaje:** C++ (Arduino Framework)
- **Microcontrolador:** Arduino/ESP32 compatible
- **Módulo de audio:** DFPlayer Mini
- **Almacenamiento:** EEPROM integrada
- **Sensores:** Detectores de monedas, botones, DIP switches

## 📋 Requisitos de Hardware

### Componentes principales
- Microcontrolador Arduino/ESP32
- Módulo MP3 DFPlayer Mini
- Detector de monedas
- Motor de la máquina
- LEDs para iluminación
- DIP switches para configuración
- Botones de control

### Conexiones típicas
```
COIN_PIN: Detector de monedas
SEL_PIN: Selector de canción
BUTTON_IN: Botón de entrada
SMOTOR_PIN: Control del motor
LUZ1_PIN, LUZ2_PIN, LUZ3_PIN: Control de LEDs
DIP switches: Configuración de tiempos
```

## ⚡ Instalación y configuración

### 1. Preparación del entorno
```bash
git clone https://github.com/Rothezee/KIDDIE.git
cd KIDDIE
```

### 2. Configuración del hardware
1. Conecta los componentes según el diagrama de pines
2. Configura los DIP switches para el tiempo de juego deseado
3. Carga las pistas de audio en la tarjeta SD del DFPlayer

### 3. Configuración del código
```cpp
// Ajusta estos parámetros en el código principal
#define COIN_PIN 2          // Pin del detector de monedas
#define SEL_PIN 3           // Pin del selector
#define SMOTOR_PIN 4        // Pin del motor
// ... otros pines según tu configuración
```

### 4. Compilación y carga
1. Abre el proyecto en Arduino IDE o PlatformIO
2. Selecciona tu placa y puerto
3. Compila y carga el programa

## 🎮 Funcionamiento

### Estados del sistema
1. **IDLE**: Esperando monedas
2. **PLAY_SALUDO**: Reproduciendo saludo inicial
3. **PLAY_THEME**: Juego activo con música y luces

### Flujo de operación
1. Inserción de moneda → Incrementa créditos
2. Inicio automático si hay créditos disponibles
3. Reproducción de saludo → Activación del motor
4. Juego principal con música seleccionable
5. Finalización por tiempo o fin de canción

## 📊 Configuración de tiempos

Los tiempos de juego se configuran mediante DIP switches:
- **Switch 1-4**: Diferentes duraciones predefinidas
- Valores típicos: 60s, 90s, 120s, 180s

## 🔧 Personalización

### Agregar nuevas canciones
1. Copia los archivos MP3 a la tarjeta SD
2. Actualiza el array `DUR_THEMES[]` con las duraciones
3. Incrementa el límite en el selector de canciones

### Modificar patrones de luces
```cpp
// Personaliza los intervalos y secuencias en loop()
static int blinkPhase = 0;
unsigned long interval = 1000; // Ajusta la velocidad
```

## 🐛 Solución de problemas

### Problemas comunes
- **No detecta monedas**: Verifica conexión del COIN_PIN
- **No reproduce audio**: Revisa la tarjeta SD y conexiones del DFPlayer
- **Motor no arranca**: Comprueba la conexión del SMOTOR_PIN
- **Créditos no se guardan**: Verifica la funcionalidad de EEPROM

### Debug por Serial
El sistema incluye mensajes de debug extensivos:
```
*** MONEDA INSERTADA ***
Cancion cambiada a: X
Tiempo de juego: Xs
*** JUEGO TERMINADO ***
```

## 📖 Documentación adicional

- `docs/circuit-diagram.png`: Diagrama de conexiones
- `docs/user-manual.pdf`: Manual de usuario completo
- `docs/troubleshooting.md`: Guía de solución de problemas

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Para contribuir:

1. Fork el proyecto
2. Crea una rama para tu feature (`git checkout -b feature/AmazingFeature`)
3. Commit tus cambios (`git commit -m 'Add some AmazingFeature'`)
4. Push a la rama (`git push origin feature/AmazingFeature`)
5. Abre un Pull Request

### Reportar bugs
Usa el sistema de issues de GitHub incluyendo:
- Descripción del problema
- Pasos para reproducir
- Hardware utilizado
- Mensajes de error (si los hay)

## 📝 Changelog

### v1.2
- ✅ LED3 con ciclo de 0.5 minutos sincronizado con motor
- ✅ Mejora en la detección de monedas
- ✅ Optimización del manejo de estados

### v1.1
- ✅ Sistema de créditos persistente
- ✅ Selector de canciones en tiempo real
- ✅ Control mejorado de tiempos

## 📄 Licencia

Este proyecto está bajo la licencia MIT. Ver `LICENSE` para más detalles.

## 👨‍💻 Autor

Desarrollado por [Rothezee](https://github.com/Rothezee)

---

⭐ Si este proyecto te resulta útil, no olvides darle una estrella en GitHub!
