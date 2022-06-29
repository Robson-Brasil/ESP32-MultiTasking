/*Objetivo: Descobrir o núcleo que está sendo executado em uma tarefa na configuração(setup) e no laço infinito(loop)  */
int X = 0;
int Y = -1;

SemaphoreHandle_t xMutex;


void setup() 
{ 
  Serial.begin(112500);
  delay(1000);

  /*Captura o núcleo que está executando o setup. */
  Serial.print("Setup: Executando no Núcleo: ");
  Serial.println(xPortGetCoreID());

  xMutex = xSemaphoreCreateMutex();
 
  xTaskCreatePinnedToCore(Tarefa0,"Tarefa0",10000,NULL,1,NULL,0); 
  xTaskCreatePinnedToCore(Tarefa1,"Tarefa1",10000,NULL,2,NULL,1); 
  //xTaskCreate(Tarefa1,"Tarefa1",10000,NULL,2,NULL);             
  delay(2000); 
}
 
void loop() 
{
  /*Captura o núcleo que está executando o loop. */
  if(Y != X)
  {
    Serial.print("Loop Infinito: Executando no Núcleo: ");
    Serial.println(xPortGetCoreID());
    Serial.print("X: ");Serial.println(X);
    delay(2000);
    Y = X;
  }
}
 
void Tarefa0( void * parametro )
{
  
  while(1)
  {
    if(xMutex != NULL)
    {
      X++;
      delay(1000);
      //Serial.println(uxTaskPriorityGet(NULL));
    }
    xSemaphoreGive(xMutex);
  }
  vTaskDelete(NULL);
}

void Tarefa1( void * parametro )
{
  while(1)
  {
    if(xMutex != NULL)
    {
      X--;
      delay(2000);
      //Serial.println(uxTaskPriorityGet(NULL));
    }
    xSemaphoreGive(xMutex);
  }
    vTaskDelete(NULL);
}
