#include <Servo.h>
#include <AFMotor.h>

// Definicion de pines
#define Echo A0 //
#define Trig A1 //
#define motor 10 //
#define buzzer 30  // Buzzer que emitira las alertas
#define Speed 170 // Ajuste de velocidad de los motres
#define spoint 80 // Posicion inicial del servomotor (calibrado a 80 grados como posicion central)

// Variables de control y distancia
char value;
int distancia;
int L;
int R;
bool Encendido = false; // Estado del robot (apagado por defecto)

// Motores y servomotor
Servo servo;
AF_DCMotor M1(1);
AF_DCMotor M2(2);
AF_DCMotor M3(3);
AF_DCMotor M4(4);

// Configuración inicial 
void setup() {
    Serial.begin(9600); //Inicia comunicacion serial para recibir los comandos
    
    pinMode(Trig, OUTPUT); // Trig como salida
    pinMode(Echo, INPUT); // Echo como entrada
    pinMode(buzzer, OUTPUT); // Buzzer como salida

    servo.attach(motor); //Conecta el servo al pin correspondiente (en este caso al pin de "motor")

     //Establece la velocidad de los motores
    M1.setSpeed(Speed);
    M2.setSpeed(Speed);
    M3.setSpeed(Speed);
    M4.setSpeed(Speed);

    digitalWrite(buzzer, LOW); // Asegurar que el buzzer inicie apagado
}

// Bucle Principal
void loop() {
       //lee los comandos desde el monitor serial
    if (Serial.available() > 0) {
        value = Serial.read(); // Lee los comandos
        Serial.println(value); // Imprime el comando en el monitor serial
        
        // Control de encendido y apagado
        if (value == 'O') {
            Encendido = true;
            Serial.println("Robot Encendido");
            beepInicio(); // Sonido de encendido
        } 
        else if (value == 'X') {
            Encendido = false;
            Serial.println("Robot Apagado");
            beepApagado(); // Sonido de apagado
            Detener();v// Detiene el movimiento de los motores
        }
    }

    if (Encendido) {
        Avance(); // Si el robot esta encendido, comienza a avanzar
    }
}

// Movimiento automatico
void Avance() {
    adelante();
    distancia = ultrasonic();

    if (distancia <= 15) { // Calibrado a 15 cm para detección
        Detener();
        beepObstaculo(); // Pitidos de advertencia

        atras();
        delay(300);
        Detener();

        L = miraizquierda();
        servo.write(spoint);
        delay(500);
        R = miraderecha();
        servo.write(spoint);

        // Decide la dirección de giro según la distancia detectada
        if (L < R) {
            izquierda();
        } 
        else {
            derecha();
        }
        
        delay(600);
        Detener();
    }
}

// Configuracion del sensor ultrasonico
int ultrasonic() {
    digitalWrite(Trig, LOW);
    delayMicroseconds(2);
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW);

    long t = pulseIn(Echo, HIGH);
    return t / 29 / 2; // Conversión a cm
}

// Sonidos del buzzer

// Sonido de encendido
void beepInicio() {
    tone(buzzer, 850, 300); // Sonido agudo corto
    delay(350);
    noTone(buzzer);
}

// Sonido de apagado
void beepApagado() {
    tone(buzzer, 500, 300); // Sonido grave corto
    delay(350);
    noTone(buzzer);
}

// Sonido de obstáculo detectado
void beepObstaculo() {
    for (int i = 0; i < 3; i++) {
        tone(buzzer, 1000, 200);
        delay(250);
        noTone(buzzer);
        delay(100);
    }
}

// Control de movimiento
void adelante() {
    M1.run(FORWARD);
    M2.run(FORWARD);
    M3.run(FORWARD);
    M4.run(FORWARD);
}

void atras() {
    M1.run(BACKWARD);
    M2.run(BACKWARD);
    M3.run(BACKWARD);
    M4.run(BACKWARD);
}

void izquierda() {
    M1.run(BACKWARD);
    M2.run(BACKWARD);
    M3.run(FORWARD);
    M4.run(FORWARD);
}

void derecha() {
    M1.run(FORWARD);
    M2.run(FORWARD);
    M3.run(BACKWARD);
    M4.run(BACKWARD);
}

void Detener() {
    M1.run(RELEASE);
    M2.run(RELEASE);
    M3.run(RELEASE);
    M4.run(RELEASE);
}

// Exploracion a los costados para rodear obstaculo
int miraizquierda() {
    servo.write(10);
    delay(500);
    return ultrasonic();
}

int miraderecha() {
    servo.write(150);
    delay(500);
    return ultrasonic();
}