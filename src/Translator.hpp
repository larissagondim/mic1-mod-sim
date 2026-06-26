/* Nome do arquivo: Translator.hpp
 * Autoria: Larissa Gondim, Laura Morais, Maria Luiza Uchoa e Sérgio Gabriel
 * Data: 18/06/2026
 * Descrição: protótipo do Translator
 * Versão: 1.0
 */
 
 #ifndef TRANSLATOR_HPP
#define TRANSLATOR_HPP

#include <string>
#include <vector>
#include <sstream>

/**
 * @brief Classe responsável pela tradução (compilação) de instruções em alto nível da ISA da IJVM.
 * * Atua como a unidade de controle de macroinstruções, decodificando mnemônicos textuais 
 * em sequências dinâmicas de microinstruções de 23 bits compatíveis com o hardware da Mic-1.
 */
class Translator {
public:
    /**
     * @brief Converte uma linha textual da ISA da IJVM em uma sequência equivalente de microinstruções binárias.
     * * Mapeia dinamicamente os argumentos de comandos como BIPUSH e ILOAD para gerar os vetores de bits corretos.
     * * @param ijvmLine String contendo a instrução em alto nível da IJVM (ex: "BIPUSH 00110011", "DUP", "ILOAD 1").
     * @return std::vector<std::string> Vetor contendo a sequência ordenada de microinstruções em strings binárias de 23 bits.
     */
    static std::vector<std::string> translateToMicroinstructions(const std::string& ijvmLine) {
        std::vector<std::string> microinsts;
        std::stringstream ss(ijvmLine);
        std::string opcode;
        ss >> opcode;

        if (opcode == "DUP") {
            // Atualização dos ponteiros de topo da pilha: MAR = SP = SP + 1
            microinsts.push_back("00110100000010000000100"); 
            // Cópia do valor do topo antigo e disparo da escrita física: MDR = TOS; wr
            microinsts.push_back("00110100000001000100111");
        } 
        else if (opcode == "BIPUSH") {
            std::string byteStr;
            ss >> byteStr; // Extrai o argumento binário de 8 bits (o literal constante)

            // Atualização do espaço de pilha: SP = MAR = SP + 1
            microinsts.push_back("00110100000010010000100");
            // Caso especial: Injeta o byte nos 8 bits iniciais do IR e ativa READ e WRITE simultaneamente (11)
            microinsts.push_back(byteStr + "000000000110000"); 
            // Persistência do registrador de retenção no topo da pilha: MDR = TOS = H; wr
            microinsts.push_back("00110100000001100101000");
        } 
        else if (opcode == "ILOAD") {
            int x;
            ss >> x; // Extrai o índice numérico correspondente à variável local requisitada

            // Passo de inicialização da base: H = LV
            microinsts.push_back("00110100000000000001101");

            // Geração dinâmica de ciclos de incremento (H = H + 1) indexados pelo valor do argumento x
            for (int i = 0; i < x; ++i) {
                microinsts.push_back("00110101000000000001000"); 
            }

            // Sequência padrão de finalização para carregamento e empilhamento do dado lido
            microinsts.push_back("00110100000000010011000"); // Ciclo combinacional: MAR = H; rd
            microinsts.push_back("00110101000010010100100"); // Ciclo combinacional: MAR = SP = SP + 1; wr
            microinsts.push_back("00110100000001000000000"); // Atualização do cache local: TOS = MDR
        }

        return microinsts;
    }
};

#endif