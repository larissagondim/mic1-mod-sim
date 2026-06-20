# Simulador da Microarquitetura Mic-1 Modificada

Este projeto consiste no desenvolvimento de um simulador em nível de microarquitetura correspondente a uma versão modificada da máquina Mic-1, executando instruções da ISA (Architecture de Linha de Comando) da IJVM. O sistema foi projetado de forma modular e extensível utilizando **C++17**, mapeando fielmente os componentes combinacionais (ULA e Deslocador) e os elementos de armazenamento (Registradores e Memória RAM).

Projeto desenvolvido como requisito para a **Segunda Avaliação** da disciplina **Arquitetura de Computadores II** do Centro de Informática da Universidade Federal da Paraíba (CI-UFPB).

## Integrantes do Grupo

- **Larissa Gondim**
- **Laura Morais**
- **Maria Luiza Uchoa**

## Professora

- **Profª. Sarah Pontes Madruga**

---

## Estrutura do Repositório

```
mic1-simulator/
│
├── dados/                                 # Arquivos textuais de entrada e teste
│   └── etapa3/
│       ├── dados_etapa3_tarefa1.txt       # Estado inicial da memória RAM
│       ├── registradores_etapa3_tarefa1.txt # Estado inicial dos registradores
│       └── instrucoes.txt                 # Instruções em alto nível da IJVM (Entrada)
│
├── src/                                   # Código-fonte do simulador em C++
│   ├── ALU.hpp                            # Definição do bloco combinacional (ULA e Shifter)
│   ├── ALU.cpp                            # Implementação lógica das portas e flags
│   ├── Microarchitecture.hpp              # Definição do hardware (Registradores e Memória)
│   ├── Microarchitecture.cpp              # Implementação de barramentos e ciclos de clock
│   ├── Decoder.hpp                        # Fatiador de instruções de 23 bits
│   ├── Translator.hpp                     # Tradutor/Compilador dinâmico de IJVM para microcódigo
│   └── main.cpp                           # Orquestrador do loop de clock e geração do Log
│
├── Makefile                               # Automação de compilação do GCC
└── README.md                              # Documentação do projeto
```

---

## Componentes Mapeados

### 1. Unidade Lógica e Aritmética (ULA) e Deslocador (ALU)

- **ULA**: Implementa as operações através de um decodificador interno controlado pelas flags `F0` e `F1`, suportando as funções `A AND B`, `A OR B`, `NOT B` e `SOMA` (Somador Completo com tratamento do bit de carry `INC`).

- **Deslocador (Shifter)**: Atua logo após a ULA, aplicando deslocamento lógico para a esquerda em 8 bits (`SLL8`) ou deslocamento aritmético para a direita em 1 bit (`SRA1`), mantendo o bit de sinal em complemento de dois.

- **Flags de Estado**: Gera automaticamente os indicadores elétricos `N` (Negativo - se o MSB for 1) e `Z` (Zero - se o resultado final for nulo).

### 2. Caminho de Dados (Microarchitecture)

- **Registradores**: Modela os 9 registradores de 32 bits (`MAR`, `MDR`, `PC`, `SP`, `LV`, `CPP`, `TOS`, `OPC`, `H`) e o registrador de 8 bits `MBR` utilizando tipos primitivos explícitos da `<cstdint>` (`uint32_t` e `uint8_t`).

- **Barramento B**: Controlado por um decodificador de 4 bits. Ao ler o registrador `MBR`, realiza automaticamente a **extensão de sinal** replicando o bit 7, enquanto a seleção de `MBRU` realiza o preenchimento estrito com zeros à esquerda.

- **Barramento C**: Controlado por um seletor de 9 bits, permitindo que múltiplos registradores sejam escritos simultaneamente com o resultado da ULA ao final do ciclo de clock.

- **Ciclo de Memória**: Controlado pelos sinais de 2 bits `READ` e `WRITE`. A comunicação síncrona com a RAM ocorre indexada pelo registrador `MAR` e interage com o `MDR`, ocorrendo estritamente após a atualização dos registradores do Barramento C.

### 3. Caso Especial: BIPUSH Fetch

Seguindo as especificações do projeto, quando o tradutor intercepta a instrução `BIPUSH byte`, a segunda microinstrução ativa os sinais `READ` e `WRITE` simultaneamente (`11`). O simulador desvia do fluxo tradicional da ULA, fazendo com que o registrador `H` absorva imediatamente os 8 bits mais significativos do `IR` (o argumento `byte`), estendendo-o com zeros.

---
## Roteiro de Desenvolvimento e Progresso do Projeto

A implementação seguiu uma metodologia incremental e rigorosamente ordenada de baixo para cima (bottom-up). Cada etapa foi validada através de testes de regressão locais utilizando os gabaritos fornecidos.

### Fase 1: O Núcleo Combinacional (Concluído)

Foco exclusivo na lógica puramente matemática e combinacional dos circuitos integrados, sem dependências de armazenamento ou memória de dados.

- Estruturação do arquivo de assinaturas lógicas `src/ALU.hpp`.
- Implementação dos modificadores de entrada e linhas de controle `ENA`, `ENB` e `INVA`.
- Implementação do decodificador interno (Multiplexador) para operações `AND`, `OR`, `NOT B`.
- Modelagem do Somador Completo em 64 bits para captura nativa do bit de Vai-um (`carryOut`).
- Acoplamento do bloco do Deslocador com tratamento para shifts de 8 bits à esquerda (`SLL8`) e deslocamento aritmético à direita (`SRA1`).
- Cálculo e atualização em tempo real das flags de estado `flagZ` (Zero) e `flagN` (Negativo).
- Validação da Fase 1 através do processamento sequencial e simulação do arquivo `programa_etapa1.txt`.

### Fase 2: Gerenciamento do Caminho de Dados (Concluído)

Introdução dos estados físicos da máquina (registradores de tamanhos fixos) e barramentos internos de interconexão.

- Declaração dos 9 registradores de 32 bits e do registrador `MBR` de 8 bits em `src/Microarchitecture.hpp`.
- Desenvolvimento do Decodificador de 4 bits para chaveamento do Barramento B.
- **Tratamento Crítico de Sinais**: Implementação da extensão de sinal em complemento de dois para a leitura de `MBR` e preenchimento estrito com zeros para `MBRU`.
- Desenvolvimento do Seletor de 9 bits para gravação simultânea em múltiplos registradores habilitados pelo Barramento C.
- Validação da Fase 2 através do mapeamento de 21 bits e verificação do comportamento no log do arquivo `programa_etapa2_tarefa2.txt`.

### Fase 3: Subsistema de Memória e Tradução IJVM (Concluído)

Integração com a memória principal de dados, isolamento síncrono dos ciclos de clock e interpretação das macroinstruções de texto da ISA.

- Alocação estruturada do vetor de memória principal (`dataMemory`) para espelhar as palavras de dados físicas.
- Criação das sub-rotinas de carregamento e população automatizada de registradores e dados a partir de arquivos externos.
- Implementação síncrona dos sinais `READ` e `WRITE` de memória, garantindo execução restrita após as gravações no Barramento C.
- Desenvolvimento do componente `src/Decoder.hpp` para desmembramento por fatiamento de strings de 23 bits.
- Codificação do motor do compilador dinâmico `src/Translator.hpp` para tradução de mnemônicos textuais (`DUP`, `BIPUSH`, `ILOAD`) em conjuntos de microinstruções binárias.
- Interpolação e tratamento do caso especial de Fetch combinado para a instrução `BIPUSH` com carregamento imediato no registrador `H`.

---

## Validação de Gabaritos de Teste

Para garantir a precisão cirúrgica do simulador, o sistema foi homologado com base na correspondência exata das matrizes de logs geradas localmente em relação aos gabaritos da disciplina:

| Cenário de Teste | Arquivo de Entrada | Arquivo de Carga | Saída Esperada | Status do Log |
| :--- | :--- | :--- | :--- | :--- |
| **Etapa 1 (ULA)** | `programa_etapa1.txt` | N/A | `saida_etapa1.txt` | 100% Compatível |
| **Etapa 2 (Caminho)** | `programa_etapa2_tarefa2.txt` | `registradores_etapa2_tarefa2.txt` | `saida_etapa2_tarefa2.txt` | 100% Compatível |
| **Etapa 3 (Memória)** | `microinstrucoes_etapa3_tarefa1.txt` | `dados_etapa3_tarefa1.txt` | `saida_etapa3_tarefa1.txt` | 100% Compatível |
| **Final (Entregável)** | `instrucoes.txt` | Multi-Carga Etapa 3 | Log Unificado (Português) | Pronto para Avaliação |

---

## Como Compilar e Executar

O projeto foi testado e homologado para ambientes Linux (como Ubuntu e Zorin OS) utilizando o compilador `g++`.

### Pré-requisitos

Certifique-se de ter o compilador GCC e a ferramenta Make instalados em sua distribuição:

```bash
sudo apt update
sudo apt install build-essential
```

### Comandos de Automação (Makefile)

Dentro da pasta raiz do projeto, você pode gerenciar a execução através dos seguintes comandos automatizados:

1. **Compilar o projeto:**
   Gera o executável otimizado `mic1_simulator` na raiz.
   ```bash
   make
   ```

2. **Compilar e Executar automaticamente:**
   Garante a build atualizada e dispara o simulador imediatamente.
   ```bash
   make run
   ```

3. **Limpar o ambiente de build:**
   Apaga o executável e limpa logs residuais antigos de execuções anteriores.
   ```bash
   make clean
   ```

---

## Formato do Log de Saída

Ao rodar a simulação, o arquivo de saída será gerado em **português** no caminho `dados/etapa3/saida_etapa3_tarefa1.txt`. O arquivo utiliza recuos de tabulação (`\t`) para uma visualização clara e legível da hierarquia de hardware:

- Estado inicial completo da memória RAM e de todos os 10 registradores.
- **Por Ciclo de Execução:**
  - Palavra do Registrador de Instrução (`ir`) desmembrada.
  - Estado do chaveamento dos Barramentos B e C.
  - Estado de todos os 10 registradores **antes** do clock combinacional.
  - Estado dos registradores atualizados **após** a borda de subida do clock.
  - Dump completo das linhas da memória RAM atualizadas após o ciclo de escrita/leitura.