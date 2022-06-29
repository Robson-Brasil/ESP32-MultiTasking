/*Objetivo: Iniciar uma quantidade de tarefas e criar um semaforo para que todas sejam concluidas, antes que se acesse o loop. */


int j = 0;

/*Cria um identificador de Mutex*/
SemaphoreHandle_t xMutex;
 
void setup() 
{ 
  Serial.begin(115200);
  delay(1000);
 
  Serial.println("Iniciando pra executar as funcoes.."); 

  //criando o MUTEX
  xMutex = xSemaphoreCreateMutex();

  //Criando as 4 tarefas
  xTaskCreatePinnedToCore
    (
      &Tarefa1,        /* Tarefa ou Função  */
      "Tarefa1",      /* Nome da Tarefa ou Função  */
      10000,          /* Tamanho da Pilha  */
      NULL,           /* Parâmetro de Entrada  */
      1,              /* Prioridade da Tarefa  */
      NULL,           /* Identificador da Tarefa  */
      1               /* Núcleo onde será executada a função. */
    );

  xTaskCreatePinnedToCore
    (
      &Tarefa2,        /* Tarefa ou Função  */
      "Tarefa2",      /* Nome da Tarefa ou Função  */
      10000,          /* Tamanho da Pilha  */
      NULL,           /* Parâmetro de Entrada  */
      1,              /* Prioridade da Tarefa  */
      NULL,           /* Identificador da Tarefa  */
      1               /* Núcleo onde será executada a função. */
    );

  xTaskCreatePinnedToCore
    (
      &Tarefa3,        /* Tarefa ou Função  */
      "Tarefa3",      /* Nome da Tarefa ou Função  */
      10000,          /* Tamanho da Pilha  */
      NULL,           /* Parâmetro de Entrada  */
      1,              /* Prioridade da Tarefa  */
      NULL,           /* Identificador da Tarefa  */
      1               /* Núcleo onde será executada a função. */
    );

      xTaskCreatePinnedToCore
    (
      &Tarefa4,        /* Tarefa ou Função  */
      "Tarefa4",      /* Nome da Tarefa ou Função  */
      10000,          /* Tamanho da Pilha  */
      NULL,           /* Parâmetro de Entrada  */
      1,              /* Prioridade da Tarefa  */
      NULL,           /* Identificador da Tarefa  */
      1               /* Núcleo onde será executada a função. */
    );
    
}
 
void loop() 
{
  j++;
  delay(1500);
  Serial.print("Loop Funcionando: ");
  Serial.println(j);
}
        
void Tarefa1( void * parametro )
{ 
  while(1)
  {
    if(xMutex!= NULL)
    {
      if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
      {
        Serial.println("-----------------");
        Serial.println("Terefa 1");
        Serial.println("-----------------");        
      }
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void Tarefa2( void * parametro )
{ 
  while(1)
  {
    if(xMutex!= NULL)
    {
      if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
      {
        Serial.println("-----------------");
        Serial.println("Terefa 2");
        Serial.println("-----------------");        
      }
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void Tarefa3( void * parametro )
{ 
  while(1)
  {
    if(xMutex!= NULL)
    {
      if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
      {
        Serial.println("-----------------");
        Serial.println("Terefa 3");
        Serial.println("-----------------");        
      }
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void Tarefa4( void * parametro )
{ 
  while(1)
  {
    if(xMutex!= NULL)
    {
      if(xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE)
      {
        Serial.println("-----------------");
        Serial.println("Terefa 4");
        Serial.println("-----------------");        
      }
      xSemaphoreGive(xMutex);
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}
