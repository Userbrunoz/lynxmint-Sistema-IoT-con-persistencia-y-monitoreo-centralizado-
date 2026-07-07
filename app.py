from flask import Flask, render_template, request, redirect, url_for
import paho.mqtt.client as mqtt
from datetime import datetime

app = Flask(__name__)

# --- ARCHITECTURAL MEMORY (DELIVERANCE STRUCTURE) ---
estado_sistema = {
    "led_seleccionado": "LED 1 (Rojo)",
    "ultimo_comando": "SISTEMA INICIALIZADO",
    "historial": []  
}

# --- CONFIGURACIÓN DE RED CON IP ACTUALIZADA ---
MQTT_BROKER = "172.24.230.143"  # <--- Tu IP actual de red del celular
MQTT_PORT = 1883
MQTT_TOPIC = "sistema/comandos"

# Compatibilidad estricta con Paho-MQTT v2.0+
mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, "Servidor_Central_Flask")

def inicializar_mqtt():
    """Establece la conexión asíncrona con el Broker Mosquitto local."""
    try:
        mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
        mqtt_client.loop_start()
        print(f"[BROKER OK] -> Conectado exitosamente en {MQTT_BROKER}:{MQTT_PORT}")
    except Exception as e:
        print(f"[CRITICAL ERROR] -> No se pudo conectar al Broker MQTT: {e}")

# Instanciamos la conexión de red
inicializar_mqtt()

@app.route('/')
def index():
    return render_template('index.html', estado=estado_sistema)

@app.route('/enviar_comando', methods=['POST'])
def enviar_comando():
    led = request.form.get('led_seleccionado')
    accion = request.form.get('accion')  
    
    estado_sistema["led_seleccionado"] = led
    
    nombre_comando = ""
    if "Rojo" in led:
        nombre_comando = f"LED_ROJO_{'ON' if accion == 'ENCENDER' else 'OFF'}"
    elif "Amarillo" in led:
        nombre_comando = f"LED_AMARILLO_{'ON' if accion == 'ENCENDER' else 'OFF'}"
    elif "Verde" in led:
        nombre_comando = f"LED_VERDE_{'ON' if accion == 'ENCENDER' else 'OFF'}"
        
    estado_sistema["ultimo_comando"] = nombre_comando
    
    try:
        # Aseguramos el envío al primer clic con QoS 1
        mqtt_client.publish(MQTT_TOPIC, nombre_comando, qos=1).wait_for_publish()
    except Exception as error_pub:
        print(f"[ERROR DE TRANSMISIÓN] -> {error_pub}")
    
    timestamp = datetime.now().strftime("[%Y-%m-%d %H:%M:%S]")
    log_linea = f"{timestamp} -> LED {led.split(' ')[2]} cambiado a {'ENCENDIDO' if accion == 'ENCENDER' else 'APAGADO'} por [INTERFAZ_WEB_LYNXMINT]"
    
    estado_sistema["historial"].insert(0, log_linea)
    
    return redirect(url_for('index'))

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
