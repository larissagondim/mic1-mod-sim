/* Nome do arquivo: ALU.hpp
 * Autoria: Larissa Gondim, Laura Morais e Maria Luiza Uchoa
 * Data: 17/06/2026
 * Descrição: protótipo da ULA
 * Versão: 1.0
 */

#ifndef ALU_HPP
#define ALU_HPP

#include <cstdint>

/**
 * @brief Estrutura de retorno que encapsula todas as saídas físicas da ULA + Shifter.
 * * Modela sinais elétricos resultantes após o processamento aritmético/lógico e a passagem pelo circuito do deslocador (SLL8/SRA1).
 */
struct ALUResult {
    /**
     * @brief Saída deslocada final .
     * * Representa o barramento de dados resultante de 32 bits que será 
     * disponibilizado para escrita nos registradores selecionados pelo barramento C.
     */
    uint32_t result;

    /**
     * @brief indicador de vai-um (Carry-out)
     * * Só assume nível lógico 1 apenas se houver overflow de capacidade 
     * durante a operação de soma aritmética do somador completo (ULA control = 11).
     * Para operações basalmente lógicas (AND, OR, NOT), assume nível 0.
     */
    uint8_t carryOut;

    /**
     * @brief Flag de valor Negativo 
     * * Assume nível lógico 1 se o bit mais significativo (MSB, bit 31) 
     * do campo 'result' for igual a 1, indicando um valor negativo em complemento de dois.
     */
    uint8_t flagN;

    /**
     * @brief Flag de valor Zero.
     * * Assume nível lógico 1 se todos os 32 bits do campo 'result' 
     * forem iguais a 0, sinalizando que a operação resultou em zero absoluto.
     */
    uint8_t flagZ;
};

/**
 * @brief Classe responsável pela simulação do circuito combinacional da ULA e do Deslocador.
 * * Esta classe não possui estado interno (estritamente combinacional). Ela recebe os operandos
 * dos barramentos e os sinais de controle da microinstrução atual, processando-os instantaneamente
 * para gerar os barramentos de saída e as flags de estado correspondentes.
 */
class ALU {
public:
    /**
     * @brief Executa o ciclo combinacional completo da ULA e do Deslocador (Shifter).
     * * Modela a propagação dos sinais pelas portas lógicas de habilitação, inversão, 
     * multiplexação da ULA e, por fim, pelo circuito de deslocamento.
     * * @param A Valor numérico de 32 bits presente na entrada A da ULA (alimentado pelo registrador H).
     * @param B Valor numérico de 32 bits presente na entrada B da ULA (alimentado pelo registrador ativo no barramento B)
     * @param F0 Flag de controle da ULA. Junto com F1, determina a operação do decodificador interno.
     * @param F1 Flag de controle da ULA. Junto com F0, determina a operação do decodificador interno.
     * @param ENA Sinal de habilitação da entrada A. Se em nível lógico baixo (0), força a entrada A interna para zero.
     * @param ENB Sinal de habilitação da entrada B. Se em nível lógico baixo (0), força a entrada B interna para zero.
     * @param INVA Sinal de inversão da entrada A. Se em nível alto (1), aplica uma operação NOT bit a bit em A.
     * @param INC Sinal de incremento aritmético. Se em nível alto (1), força o bit de Vem-um (Carry-in) do somador para 1.
     * @param SLL8 Sinal de deslocamento lógico para a esquerda ($X_0$). Se ativo, desloca a saída da ULA em 8 bits à esquerda.
     * @param SRA1 Sinal de deslocamento aritmético para a direita ($X_1$). Se ativo, desloca a saída da ULA em 1 bit à direita preservando o sinal.
     * * @return ALUResult Estrutura contendo a palavra de dados resultante ($S_d$) e as flags de estado (Carry-out, N, Z).
     * * @note Os sinais SLL8 e SRA1 são mutuamente exclusivos e nunca devem assumir nível lógico alto simultaneamente.
     */
    static ALUResult execute(uint32_t A, uint32_t B, 
                             uint8_t F0, uint8_t F1, 
                             uint8_t ENA, uint8_t ENB, 
                             uint8_t INVA, uint8_t INC,
                             uint8_t SLL8, uint8_t SRA1);
};

#endif