/* Nome do arquivo: ALU.cpp
 * Autoria: Larissa Gondim, Laura Morais e Maria Luiza Uchoa
 * Data: 17/06/2026
 * Descrição: implementação das funções da ULA
 * Versão: 1.0
 */

#include "ALU.hpp"

ALUResult ALU::execute(uint32_t A, uint32_t B, 
                       uint8_t F0, uint8_t F1, 
                       uint8_t ENA, uint8_t ENB, 
                       uint8_t INVA, uint8_t INC,
                       uint8_t SLL8, uint8_t SRA1) {
    
    ALUResult out = {0, 0, 0, 0};

    // modificadores das entradas pelas linhas Enable
    uint32_t A_in = (ENA == 1) ? A : 0;
    uint32_t B_in = (ENB == 1) ? B : 0;

    // modificador de inversão de A
    if (INVA == 1) {
        A_in = ~A_in; 
    }

    // decodificador interno da ULA (multiplexador)
    uint64_t alu_out = 0; // Usamos 64 bits temporários para capturar o Carry-out de 32 bits facilmente
    uint8_t op = (F0 << 1) | F1;

    switch (op) {
        case 0: alu_out = A_in & B_in; break; // AB
        case 1: alu_out = A_in | B_in; break; // A + B lógico
        case 2: alu_out = ~B_in;       break; // NOT B
        case 3: // somador completo
            alu_out = static_cast<uint64_t>(A_in) + static_cast<uint64_t>(B_in) + INC;
            out.carryOut = ((alu_out >> 32) & 1); // Captura o bit 32 (Vai-um)
            break;
    }

    uint32_t final_ula = static_cast<uint32_t>(alu_out);

    // o bloco do deslocador (ocorre após o cálculo da ULA)
    if (SLL8 == 1) {
        out.result = final_ula << 8; // deslocamento lógico para a esquerda
    } else if (SRA1 == 1) {
        // o cast para int32_t força o compilador a realizar um shift aritmético (preserva sinal)
        out.result = static_cast<uint32_t>(static_cast<int32_t>(final_ula) >> 1);
    } else {
        out.result = final_ula;
    }

    // atualização das flags de estado
    out.flagZ = (out.result == 0) ? 1 : 0;
    out.flagN = ((out.result & 0x80000000) != 0) ? 1 : 0; // Verifica se o bit de sinal (MSB) é 1

    return out;
}