# Relatório de Análise: Sistema Multitarefa com FreeRTOS

**Autor:** Antonio Crepaldi
**Projeto:** embarcatech-freertos-tarefa-1
**Data:** 14 de Junho de 2025

---

## 1. O que acontece se todas as tarefas tiverem a mesma prioridade?

Quando as tarefas possuem a mesma prioridade, o escalonador do FreeRTOS utiliza a política **Round-Robin com Fatiamento de Tempo (Time Slicing)**.

Isso significa que o sistema operacional divide o tempo de processamento da CPU em pequenas "fatias" e as distribui de forma justa e alternada entre todas as tarefas prontas. Essa abordagem garante que nenhuma tarefa monopolize o processador e cria a ilusão de que todas estão sendo executadas simultaneamente.

---

## 2. Qual tarefa consome mais tempo da CPU?

A **`button_task`** é a que mais consome tempo da CPU.

A razão para isso é a sua **frequência de execução**. Enquanto as tarefas do LED e do buzzer acordam apenas 2 vezes por segundo, a `button_task` acorda **10 vezes por segundo** para realizar a verificação periódica (*polling*) dos botões. Essa maior frequência de ativação e troca de contexto resulta em um maior consumo acumulado dos recursos da CPU.

---

## 3. Quais seriam os riscos de usar polling?

A abordagem de *polling* (verificação periódica), embora funcional, apresenta três riscos principais em sistemas embarcados:

* **Desperdício de CPU e Energia:** É a maior desvantagem. A tarefa acorda e processa constantemente, mesmo quando nenhuma ação ocorre, gastando ciclos de processador e energia desnecessariamente.

* **Alta Latência:** A resposta a um evento não é imediata. O sistema pode levar até 100 ms (o intervalo de polling) para detectar que um botão foi pressionado, o que pode ser lento demais para algumas aplicações.

* **Risco de Perda de Eventos:** Um evento muito rápido, como um toque no botão que dure menos de 100 ms, pode ocorrer no intervalo entre duas verificações e ser completamente ignorado pelo sistema.

A alternativa técnica superior ao polling é o uso de **interrupções**, que resolvem esses três problemas ao executar código apenas quando o evento realmente ocorre.