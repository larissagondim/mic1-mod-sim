# Visão Geral do Sistema

O projeto consiste em um simulador em C++17 baseado na arquitetura Mic-1, adaptada para executar instruções da IJVM (Integer Java Virtual Machine).

A implementação foi organizada em camadas. Primeiro ficam os componentes combinacionais, como a ULA, depois os elementos que armazenam estado, como registradores e memória. Por fim, há a parte responsável por controlar a execução do simulador e gerar os arquivos de saída.

---

# 1. Camada Combinacional: ULA e Deslocador

Arquivos:

* ALU.hpp
* ALU.cpp

Essa camada representa a parte combinacional do processador. Ela não armazena informações entre os ciclos, apenas recebe as entradas e produz uma saída de acordo com os sinais de controle.

O método `ALU::execute` recebe os operandos e os bits de controle da microinstrução e retorna o resultado da operação junto com o carry e as flags N e Z.

O funcionamento ocorre da seguinte forma:

1. Os sinais `ENA` e `ENB` definem se as entradas A e B serão utilizadas ou substituídas por zero.
2. Se `INVA` estiver ativo, a entrada A é invertida.
3. Os bits `F0` e `F1` determinam qual operação será executada:

   * `00`: AND
   * `01`: OR
   * `10`: NOT em B
   * `11`: soma entre A, B e o carry de entrada (`INC`)
4. Depois da operação da ULA, o resultado pode passar pelo deslocador:

   * `SLL8`: desloca 8 bits para a esquerda.
   * `SRA1`: faz deslocamento aritmético de 1 bit para a direita.
5. Por último são calculadas as flags:

   * `Z` indica se o resultado é zero.
   * `N` indica se o bit mais significativo está ligado.

---

# 2. Camada de Hardware: Registradores e Memória

Arquivos:

* Microarchitecture.hpp
* Microarchitecture.cpp

Essa classe representa os registradores, a memória RAM e os barramentos utilizados durante a execução.

As principais funções são:

* Inicializar todos os registradores e a memória.
* Carregar o conteúdo inicial da RAM e dos registradores a partir de arquivos (com suporte flexível ao formato `nome = valor` ou apenas binários sequenciais).
* Selecionar qual registrador será colocado no barramento B.
* Escrever o resultado da ULA nos registradores indicados pelo barramento C.
* Executar as operações de leitura e escrita na memória.

No caso do registrador `MBR`, existem dois modos de leitura:

* `MBR`: faz extensão de sinal para 32 bits.
* `MBRU`: faz extensão com zeros.

As operações de memória utilizam os registradores `MAR` e `MDR`. Primeiro os registradores são atualizados e, somente depois, ocorre a leitura ou escrita da memória, mantendo o comportamento esperado da arquitetura.

---

# 3. Camada de Tradução

Arquivos:

* Decoder.hpp
* Translator.hpp

Essa parte faz a interpretação das microinstruções e a tradução das instruções da IJVM.

O `Decoder` recebe uma palavra de 23 bits e separa seus campos em:

* Controle da ULA.
* Barramento C.
* Controle de memória.
* Barramento B.

Já o `Translator` converte instruções da IJVM em sequências de microinstruções.

Alguns exemplos:

* `DUP`: gera duas microinstruções para duplicar o topo da pilha.
* `BIPUSH`: gera três microinstruções para empilhar um valor constante.
* `ILOAD`: gera uma sequência de microinstruções para acessar uma variável local e colocá-la no topo da pilha.

No caso do `ILOAD`, a quantidade de incrementos em `H` depende do índice informado na instrução.

---

# 4. Execução Principal

Arquivo:

* main.cpp

O arquivo `main.cpp` coordena toda a execução do simulador através de chamadas sequenciais para testar progressivamente cada componente:

1. **Etapa 1:** Testa a ULA isolada lendo sinais básicos de 6 bits.
2. **Etapa 2 - Tarefa 1:** Valida a ULA acoplada ao deslocador e as flags matemáticas usando sinais de 8 bits.
3. **Etapa 2 - Tarefa 2:** Simula o caminho de dados completo com Barramentos B e C (instruções de 21 bits), mas sem tráfego de memória.
4. **Entregável Final (IJVM):** Lê o código de alto nível (`BIPUSH`, `DUP`, `ILOAD`). O `Translator` converte os mnemônicos em microcódigo, e a função `executarMicroinstrucao()` aplica os ciclos de clock registrando as mudanças de memória ao final de cada macroinstrução.
5. **Etapa 3 - Tarefa 1:** Simulação autônoma que consome diretamente instruções binárias de 23 bits, fazendo despejo (dump) da memória a cada microinstrução individual executada.

Durante a simulação principal, o orquestrador encapsulado em `executarMicroinstrucao()` realiza o seguinte fluxo para cada instrução de 23 bits:

* O `Decoder` separa os sinais de controle lógicos.
* O barramento B seleciona e fornece o operando para a ULA.
* A ULA executa a operação matemática, passando pelo Deslocador (Shifter).
* O resultado elétrico é gravado de forma síncrona nos registradores indicados pelo Barramento C.
* O ciclo de memória (`READ`/`WRITE`) ocorre após a gravação dos registradores.
* O estado completo da máquina (todos os 10 registradores e barramentos) é capturado, formatado rigorosamente com cabeçalhos padronizados e salvo nos respectivos logs (ex: `saida_entregavel.txt`).

Há um interceptor de hardware modelado exclusivamente para o *fetch* especial do `BIPUSH`, onde os sinais simultâneos de `READ` e `WRITE` (11) fazem com que o operando seja retido no `MBR` e transferido diretamente ao registrador `H`.

Ao final da execução de todos os lotes, os logs de saída fornecem rastreabilidade completa das operações para auditoria de gabarito.

---

# 5. Makefile

O projeto utiliza um `Makefile` para facilitar a compilação.

As principais opções são:

* `-Wall`: habilita avisos do compilador.
* `-std=c++17`: utiliza o padrão C++17.
* `-O3`: ativa otimizações de compilação.

Os principais alvos são:

* `make`: compila o projeto.
* `make run`: compila e executa o simulador.
* `make clean`: remove o executável e os arquivos de saída gerados durante a execução.
