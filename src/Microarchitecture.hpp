/* Nome do arquivo: Microarchitecture.hpp
 * Autoria: Larissa Gondim, Laura Morais e Maria Luiza Uchoa
 * Data: 18/06/2026
 * Descrição: protótipo da Microarquitetura
 * Versão: 1.0
 */
#ifndef MICROARCHITECTURE_HPP
#define MICROARCHITECTURE_HPP

#include <cstdint>
#include <vector>
#include <string>

/**
 * @brief Classe que representa a unidade de armazenamento e controle de fluxo da microarquitetura Mic-1.
 * * Gerencia o estado físico da máquina, incluindo os registradores internos de 32 bits, 
 * o registrador MBR de 8 bits, a memória principal de dados e o chaveamento dos barramentos internos.
 */
class Microarchitecture {
public:
    /**
     * @name Registradores Internos da Mic-1
     * @{
     */
    uint32_t mar; ///< Memory Address Register: Armazena o endereço de memória para leitura/escrita.
    uint32_t mdr; ///< Memory Data Register: Armazena o dado lido da memória ou pronto para ser escrito.
    uint32_t pc;  ///< Program Counter: Contador de programa (ponteiro para a instrução atual da ISA).
    uint32_t sp;  ///< Stack Pointer: Ponteiro para o endereço do topo da pilha de execução.
    uint32_t lv;  ///< Local Variable: Ponteiro base para o quadro de variáveis locais.
    uint32_t cpp; ///< Constant Pool Pointer: Ponteiro para a base do bloco de constantes da memória.
    uint32_t tos; ///< Top Of Stack: Armazena uma cópia local em cache do valor contido no endereço apontado por SP.
    uint32_t opc; ///< Old Program Counter: Armazena temporariamente o PC anterior durante desvios e saltos.
    uint32_t h;   ///< Holding Register: Registrador de retenção que serve obrigatoriamente como a Entrada A da ULA.
    uint8_t mbr;  ///< Memory Byte Register: Registrador de 8 bits que armazena bytes brutos de argumentos vindos do código.
    /** @} */

    /**
     * @brief Memória de dados principal mapeada por vetor.
     * * Cada índice simula uma linha de endereço físico contendo uma palavra de dado de 32 bits em complemento de dois.
     */
    std::vector<uint32_t> dataMemory;

    /**
     * @brief Construtor padrão da microarquitetura.
     * * Inicializa todos os registradores com zero absoluto e aloca o espaço inicial da memória de dados.
     */
    Microarchitecture();

    /**
     * @brief Inicializa os valores padrão de todos os registradores a partir de um arquivo textual.
     * * @param filepath Caminho completo para o arquivo txt contendo os estados iniciais no formato de atribuição binária.
     */
    void loadRegisters(const std::string& filepath);

    /**
     * @brief Inicializa e popula as linhas da memória de dados a partir de um arquivo textual de carga.
     * * @param filepath Caminho completo para o arquivo txt contendo a lista sequencial de dados em binário puro de 32 bits.
     */
    void loadMemory(const std::string& filepath);

    /**
     * @brief Simula o decodificador de 4 bits encarregado de disponibilizar o dado de um registrador para o Barramento B.
     * * @param decoderSelection Valor numérico de 4 bits correspondente ao registrador escolhido para a leitura.
     * @return uint32_t O valor de 32 bits injetado no Barramento B (aplica extensão de sinal caso o MBR seja escolhido).
     */
    uint32_t readB_Bus(uint8_t decoderSelection);

    /**
     * @brief Simula o seletor de 9 bits responsável por direcionar o resultado do barramento C para escrita nos registradores ativos.
     * * @param encoderSelection Máscara de bits de 9 posições, onde cada bit em nível alto habilita a escrita em seu respectivo registrador.
     * @param value O dado numérico de 32 bits oriundo da saída do circuito ULA/Deslocador.
     */
    void writeC_Bus(uint16_t encoderSelection, uint32_t value);
    
    /**
     * @brief Sincroniza e executa as operações de barramento da memória principal (Leitura / Escrita).
     * * @note Este ciclo deve obrigatoriamente ocorrer de maneira síncrona, somente após os cálculos da ULA
     * e o encerramento do processo de gravação do Barramento C nos registradores.
     * * @param readSignal Sinal lógico de leitura (READ). Se igual a 1, copia o conteúdo de Memory[MAR] para o MDR.
     * @param writeSignal Sinal lógico de escrita (WRITE). Se igual a 1, grava o conteúdo do MDR em Memory[MAR].
     */
    void executeMemoryCycle(uint8_t readSignal, uint8_t writeSignal);
};

#endif