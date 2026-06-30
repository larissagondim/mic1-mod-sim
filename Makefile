# Nome do compilador
CXX = g++

# Flags de compilação: 
# -Wall: ativa avisos de erros em potencial
# -std=c++17: define o padrão moderno do C++
# -O3: ativa otimizações máximas de performance de hardware
CXXFLAGS = -Wall -std=c++17 -O3

# Nome do executável final
TARGET = mic1_simulator

# Diretórios de origem
SRC_DIR = src

# Captura automaticamente todos os arquivos .cpp dentro da pasta src/
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Regra padrão (executada ao digitar apenas 'make' no terminal)
all: $(TARGET)

# Regra de compilação do executável principal
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# Regra para limpar os arquivos gerados de build e logs antigos
clean:
	rm -f $(TARGET)
	rm -f dados/etapa1/saida_etapa1.txt
	rm -f dados/etapa2/saida_etapa2_tarefa1.txt
	rm -f dados/etapa2/saida_etapa2_tarefa2.txt
	rm -f dados/etapa3/saida_etapa3_tarefa1.txt

# Regra para compilar e rodar o simulador logo em sequência
run: all
	./$(TARGET)