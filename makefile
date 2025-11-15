# --- Variáveis de Compilação ---

# Compilador a ser utilizado
CC = gcc

# Nome do arquivo executável final
EXEC = run_n_gun

# --- Diretórios ---

# Diretório onde estão os arquivos de código-fonte (.c)
SRCDIR = src

# Diretório onde serão guardados os arquivos objeto (.o)
OBJDIR = obj

# --- Flags de Compilação e Linkagem ---

# Flags para o compilador C.
# -I$(SRCDIR) -> Adiciona o diretório src/ para que os #include "arquivo.h" funcionem
# -Wall -> Mostra todos os warnings (avisos)
# -g    -> Inclui informações de debug no executável
# -O2   -> Nível de otimização do código
CFLAGS = -I$(SRCDIR) -Wall -g -O2

# Flags para o linker. Aqui você precisa adicionar todas as bibliotecas Allegro que usar.
# Estas são as mais comuns. Adicione outras se precisar (ex: -lallegro_audio).
LFLAGS = -lallegro -lallegro_main -lallegro_primitives -lallegro_image -lallegro_font -lallegro_ttf -lm

# --- Descoberta Automática de Arquivos ---

# Pega todos os arquivos .c do diretório de fontes (SRCDIR)
SRCS = $(wildcard $(SRCDIR)/*.c)

# Transforma a lista de arquivos .c em uma lista de arquivos .o no diretório de objetos (OBJDIR)
# Ex: src/player.c se torna obj/player.o
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

# --- Regras do Makefile ---

# A primeira regra é a padrão, executada quando você digita apenas "make"
all: $(EXEC)

# Regra para linkar todos os arquivos objeto (.o) e criar o executável final
$(EXEC): $(OBJS)
	@echo "Ligando os arquivos objeto para criar o executável..."
	$(CC) -o $@ $^ $(LFLAGS)
	@echo "Executável '$(EXEC)' criado com sucesso!"

# Regra para compilar cada arquivo .c em seu respectivo arquivo .o
# Ela é uma regra de padrão que ensina o make a criar um .o a partir de um .c
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Regra "clean": remove todos os arquivos gerados pela compilação
# Exigida pelo enunciado do trabalho [cite: 35]
clean:
	@echo "Limpando arquivos de compilação..."
	rm -f $(EXEC)
	rm -rf $(OBJDIR)
	@echo "Limpeza concluída."

# Regra "re": força a limpeza e a recompilação de todo o projeto
re: clean all

# Phony targets são regras que não representam arquivos
.PHONY: all clean re