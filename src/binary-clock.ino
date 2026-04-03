#include <EEPROM.h>

#define NUM_SAMPLES 10
#define DEBOUNCE_DELAY 50

#define MATRIX_SIZE 4
#define BTN_PIN A5
#define POT_PIN A4


/** variables et fonctions **/

/* broches */
const uint8_t row_pins[MATRIX_SIZE] = {3, 5, 10, 11};
const uint8_t col_pins[MATRIX_SIZE] = {2, 4,  7,  8};

/* ordonnancement de l'horloge et des tâches avec le bouton/potentiomètre */
static int clock_ints[3] = {7, 49, 01};

static unsigned long last_clock_upd = 0;
static unsigned long last_if_upd = 0;

static uint8_t brightness = 255;
static bool led_state = 1;

/* la matrice comme elle apparaît sur l'appareil */
static bool matrix[MATRIX_SIZE][MATRIX_SIZE] =
{
  {1, 1, 
   1, 1},
  {1,  1, 1, 
   1},{1, 1, 
   1,  1,},{1, 
   1,  1,   1}
};

/* chaque colonne physique de DEL avec les bits correspondants (utilisé dans le placement des numbres dans la matrice) en ordre croissant
   ligne, colonne */
static const int8_t led_bitmap_h[4][2] {
  {0, 3}, {2, 3}, {1, 3}, {3, 3}
};

static const int8_t led_bitmap_m[6][2] {
  {2, 1}, {1, 2}, {3, 1}, {2, 2}, {0, 2}, {3, 2}
};

static const int8_t led_bitmap_s[6][2] {
  {0, 0}, {3, 0}, {1, 0}, {0, 1}, {2, 0}, {1, 1}
};

/* intéractions avec les interfaces électroniques */
bool read_btn(void);
int read_brightness(void);
void set_led(int pin);
void init_pins(void);

/* calcul de temps */
void set_clock(void); /* marquer le temps du début par communication avec l'ordinateur*/
void increment(int *clock);                /* incrémenter l'horloge */
void display_clock(void);            /* montrer l'horloge */
void n_to_mx(int clock[3], bool matrix[MATRIX_SIZE][MATRIX_SIZE]);

/** définitions **/

/* lire le potentiomètre en tentant d'éliminer les imperfections */
/* lecture exponentielle */
int read_brightness(void) 
{
  static float filtered = 0;

  int16_t raw = analogRead(POT_PIN);

  float alpha = 0.1;  // contrôle du niveau de lissage exponentiel
  filtered = alpha * raw + (1 - alpha) * filtered;

  return (int)(filtered / 4);
}

/* lire le bouton interrupteur avec une fonctionnalité debounce */
bool read_btn(void) 
{
  static bool state = false; 
  static unsigned long last_debounce = 0;
  static bool last_btn_state = LOW;

  bool reading = digitalRead(BTN_PIN);

  if (reading != last_btn_state)
    last_debounce = millis();

  if ((millis() - last_debounce) > DEBOUNCE_DELAY)
    if (last_btn_state == HIGH && reading == LOW)
      state == !state;
  
  last_btn_state = reading;
  return state;
}

/* convertir un entier en un tableau de bits */
void n_to_bits(int value, byte *array, int size)
{
  for (int i = 0; i < size; i++)
    array[i] = (value >> i) & 1;
}

/* convertir les valeurs numériques en binaire pour les montrer sur la matrice */
void n_to_mx(int clock[3], bool matrix[MATRIX_SIZE][MATRIX_SIZE])
{
  int8_t row, col;
  byte heures[4], minutes[6], secondes[6];
  n_to_bits(clock[0], heures, 4);
  n_to_bits(clock[1], minutes, 6);
  n_to_bits(clock[2], secondes, 6);

  for (int i = 0; i < 4; i++) {
    row = led_bitmap_h[i][0];
    col = led_bitmap_h[i][1];
    matrix[row][col] = heures[i] ? (bool)(1) : (bool)(0);
  }
  for (int i = 0; i < 6; i++) {
    matrix[led_bitmap_m[i][0]][led_bitmap_m[i][1]] = minutes[i];
    matrix[led_bitmap_s[i][0]][led_bitmap_s[i][1]] = secondes[i];
  }
}

/* initialiser touches les broches nécessaires */
void init_pins(void)
{
  for (int i = 0; i < MATRIX_SIZE; i++) {
    pinMode(row_pins[i], OUTPUT);
    pinMode(col_pins[i], OUTPUT);
  }
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(POT_PIN, INPUT_PULLUP);
}

void increment(int *clock)
{
  if (clock[2] == 59) { /* secondes*/
    if (clock[1] == 59) /* minutes */
      clock[0] = (clock[0] + 1) % 12; /* heures*/
    clock[1] = (clock[1] + 1) % 60;
  }
  clock[2] = (clock[2] + 1) % 60;
}

void set_clock()
{
  int cur_time ;
  if (Serial.available()) {
    cur_time = Serial.parseInt();
  }
  clock_ints[0] = (cur_time / 3600) % 24;
  clock_ints[1] = (cur_time / 60) % 60;
  clock_ints[2] = (cur_time % 60);
}

void setup() 
{
  init_pins();

  Serial.begin(9600);
  unsigned long timeout_time = millis();
//  set_clock();
}

void display_clock(bool matrix[MATRIX_SIZE][MATRIX_SIZE]) 
{  
  /* On parcourt la matrice par ligne */
  for (int row = 0; row < MATRIX_SIZE; row++) {
    /* éteindre les lignes d'abord */
    for (int r = 0; r < MATRIX_SIZE; r++)
      digitalWrite(row_pins[r], LOW);

    /* activer les colonnes dont on a besoin */
    for (int col = 0; col < 4; col++)
      digitalWrite(col_pins[col], matrix[row][col] ? LOW : HIGH);

    /* activer la ligne */
    digitalWrite(row_pins[row], HIGH);

    delay(2);
  }
}

void loop()
{
  /* ordonnancement */
  unsigned long now = millis();

  if (millis() - last_clock_upd >= 1000) {
    last_clock_upd = millis();
    increment(clock_ints);
    n_to_mx(clock_ints, matrix);
  
  }
 
  if (millis() - last_if_upd >= 3) {
    last_if_upd = millis();

    led_state = read_btn();
    brightness = read_brightness();
  }
  display_clock(matrix); 

}