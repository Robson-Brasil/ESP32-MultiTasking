/*Objetivo: Usar filas para passar informações para uma variavel e plotar na tela*/

/*Identificador de Fila é criado*/
QueueHandle_t fila;
 
void setup() 
{ 
  Serial.begin(115200); 
  /*Criação da fila com até 10 elementos e o tamanho em bytes de cada elemento*/
  fila = xQueueCreate( 10, sizeof( int ) ); 
  if(fila == NULL)
  {
    Serial.println("Erro ao criar a fila");
  } 
}
 
void loop()
{ 
  if(fila == NULL)return; 
  for(int i = 0; i<10; i++)
  {
    /* Essa função insere um item na fila por determinado tempo. */
    xQueueSend(fila, &i, portMAX_DELAY);
  } 
  int elemento; 
  for(int i = 4; i< 10; i++)
  {
    /*Essa função recebe um item da fila e insere no item elemento*/
    xQueueReceive(fila, &elemento, portMAX_DELAY);
    Serial.print(elemento);
    Serial.print("|");
  } 
  if(fila == NULL)return; 
  for(int i = 0; i < 5; i++)
  {
    /* Essa função insere um item na fila por determinado tempo. */
    xQueueSend(fila, &i, portMAX_DELAY);
  } 
   elemento; 
  for(int i = 0; i<10; i++)
  {
    /*Essa função recebe um item da fila e insere no item elemento*/
    xQueueReceive(fila, &elemento, portMAX_DELAY);
    Serial.print(elemento);
    Serial.print("|");
  }   
  Serial.println();
  delay(1000);
}
