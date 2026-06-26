/* Nome do arquivo: Decoder.hpp
 * Autoria: Larissa Gondim, Laura Morais, Maria Luiza Uchoa e Sérgio Gabriel
 * Data: 18/06/2026
 * Descrição: protótipo do Decodificador
 * Versão: 1.0
 */

#ifndef DECODER_HPP
#define DECODER_HPP

#include <string>
#include <cstdint>

/**
 * @brief Classe utilitária responsável pelo fatiamento e decodificação de strings binárias de controle.
 * * Modela o comportamento do Registrador de Instrução (IR) ao segmentar barramentos de controle 
 * estendidos em sinais específicos para os demais componentes da microarquitetura.
 */
class Decoder {
public:
    /**
     * @brief Desmembra uma string binária de 23 bits nos sinais individuais de controle da máquina.
     * * Fatia a instrução respeitando a organização dos blocos elétricos do caminho de dados 
     * estabelecido pelo layout padrão do arquivo de logs.
     * * @param irStr String contendo os 23 bits binários da microinstrução atual.
     * @param ula Referência para armazenar os 8 bits de controle destinados à ULA e ao Shifter.
     * @param busC Referência para armazenar os 9 bits de habilitação de escrita do Barramento C.
     * @param mem Referência para armazenar os 2 bits de controle de operações de memória (Read/Write).
     * @param busB Referência para armazenar os 4 bits de seleção de leitura do Barramento B.
     */
    static void split23Bits(const std::string& irStr, uint8_t& ula, uint16_t& busC, uint8_t& mem, uint8_t& busB) {
        // Fatiamento por substring convertendo a base binária pura (base 2) para tipos numéricos
        ula  = static_cast<uint8_t>(std::stoi(irStr.substr(0, 8), nullptr, 2));
        busC = static_cast<uint16_t>(std::stoi(irStr.substr(8, 9), nullptr, 2));
        mem  = static_cast<uint8_t>(std::stoi(irStr.substr(17, 2), nullptr, 2));
        busB = static_cast<uint8_t>(std::stoi(irStr.substr(19, 4), nullptr, 2));
    }
};

#endif