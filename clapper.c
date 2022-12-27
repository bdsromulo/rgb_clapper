const int pino_microfone = A0; // pino onde o potenciometro está conectado
int leitura = 0; // variável para armazenar o valor lido pelo ADC
int red = 9;
int green = 10;
int blue = 11;

int corAtual = 0;
char cores[7][3] = {{255,255,255}, // branco
                    {255,255,  0}, // amarelo
                    {255,  0,  0}, // vermelho
                    {255,  0,255}, // roxo
                    {  0,255,255}, // ciano
                    {  0,255,  0}, // verde
                    {  0,  0,255}}; // azul

void trocarCor(char r, char g, char b) {
  analogWrite(red, r);
  analogWrite(green, g);
  analogWrite(blue, b);
}

float amplitude = 0.0; // armazenará o valor de pico a pico da onda
unsigned int valor_max = 0; // armazenará o valor máximo lido pelo sensor
unsigned int valor_min = 1024; // armazenará o valor mínimo lido pelo sensor
float valor_limite = 4.5; // valor mínimo para considerar uma palma (0.0 - 5.0)
unsigned long inicio_amostragem; // armazena o instante que começou a amostragem
const unsigned long tempo_amostragem = 250; // tempo de amostragem em ms
bool ligado = true;

int palmas = 0; // contador de palmas
unsigned long tempo_palmas; // armazena o instante que começou a amostragem
const unsigned long tempo_palmas_max = 500; // período de amostragem de palmas

void setup() {
  // Inicia e configura a Serial
  Serial.begin(9600); // 9600bps

  pinMode(pino_microfone, INPUT); // configura o pino com potenciometro como entrada
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  pinMode(blue,OUTPUT);
  analogWrite(red, 255);
  analogWrite(blue, 255);
  analogWrite(green, 255);
  
  inicio_amostragem = millis(); // reseta o tempo inicial da amostragem
}

void loop() {

  // Coleta dados durante o tempo de amostragem
  if (millis() - inicio_amostragem < tempo_amostragem) {

    // le o valor de tensão no pino do sensor
    leitura = analogRead(pino_microfone);

    // se valor lido for maior que valor máximo
    if (leitura > valor_max) {
      valor_max = leitura; // atualiza valor máximo
    }
     // se valor lido for menor que valor mínimo
    else if (leitura < valor_min) {
      valor_min = leitura; // atualiza valor mínimo
    }

  }
  else { // processa os dados
    inicio_amostragem = millis(); // reseta o tempo inicial da amostragem

    // calcula, converte e imprime o valor de pico-a-pico em tensão elétrica
    amplitude = valor_max - valor_min; // max - min = pico-a-pico, amplitude
    amplitude = (amplitude * 5.0) / 1023.0; // converte para tensão(V)

    Serial.println(amplitude); // imprime a amplitude no Monitor Serial

    // Reseta os valores máximos e mínimos
    valor_max = 0;
    valor_min = 1024;

    // Se o valor de amplitude superar o valor limite, inverte o estado da carga
    if (amplitude > valor_limite) {
      if (palmas == 0){ // se for a primeira palma daquele período
        tempo_palmas = millis(); // reinicia a amostragem de palmas
      }
      palmas++; // incrementa o número de palmas
    }
  }

  // se o tempo de amostragem de palmas é atingido
  if (millis() - tempo_palmas > tempo_palmas_max) {

    tempo_palmas = millis(); // reseta tempo inicial da amostragem de palmas
    // Se o número de palmas captadas naquele periodo for igual a 2
    if(palmas == 2) {
      Serial.println("Duas palmas.");
      if(ligado) {
        analogWrite(red,0);
        analogWrite(green,0);
        analogWrite(blue,0);
        ligado = false;
      }
      else {
        analogWrite(red, cores[corAtual][0]);
        analogWrite(green, cores[corAtual][1]);
        analogWrite(blue, cores[corAtual][2]);
        ligado = true;
      }
    }

    if (palmas == 1 && ligado)
    {
      corAtual++;
      if (corAtual > 6) corAtual = 0;
      
      analogWrite(red, cores[corAtual][0]);
      analogWrite(green, cores[corAtual][1]);
      analogWrite(blue, cores[corAtual][2]);
    }

    if(palmas > 0){
      Serial.println("Palmas resetadas");
      palmas = 0; // reinicia o contador de palmas
    }
  }

}