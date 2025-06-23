CXX := clang++
CXXFLAGS := -std=c++17 -Wall -fPIC -I. -Ilib -Icrypto
LDFLAGS := 

SRC_DIRS := crypto_app ui utils core crypto/mickey2Algo crypto/chacha20Algo crypto/permutationsAlgo lib
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')
OBJS := $(SRCS:.cpp=.o)

EXEC := bin/crypto_app
DYLIB := bin/librgrcrypto.dylib

.PHONY: all run clean lib libs dist

# Собирает всё (и исполняемый файл, и библиотеку, и отдельные dylib)
all: $(EXEC) $(DYLIB) bin/libmickey2.dylib bin/libchacha20.dylib bin/libpermutations.dylib

# Отдельно: только приложение
app: $(EXEC)

# Отдельно: только библиотека
lib: $(DYLIB)

# Запуск
run: $(EXEC)
	@echo "Запуск приложения..."
	@./$(EXEC)

# Исполняемый файл
$(EXEC): $(OBJS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Dylib
$(DYLIB): lib/rgrcrypto.o crypto/mickey2Algo/mickey2.o crypto/mickey2Algo/mickey2_api.o utils/base64.o
	@mkdir -p $(@D)
	$(CXX) -dynamiclib -o $@ $^

# Individual dylibs
bin/libmickey2.dylib: crypto/mickey2Algo/mickey2.o crypto/mickey2Algo/mickey2_api.o utils/base64.o
	@mkdir -p $(@D)
	$(CXX) -dynamiclib -o $@ $^

bin/libchacha20.dylib: crypto/chacha20Algo/chacha20.o crypto/chacha20Algo/chacha20_api.o utils/base64.o
	@mkdir -p $(@D)
	$(CXX) -dynamiclib -o $@ $^

bin/libpermutations.dylib: crypto/permutationsAlgo/permutation.o crypto/permutationsAlgo/permutation_api.o utils/base64.o
	@mkdir -p $(@D)
	$(CXX) -dynamiclib -o $@ $^

libs: bin/libmickey2.dylib bin/libchacha20.dylib bin/libpermutations.dylib

# Общая компиляция .cpp → .o
%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Очистка проекта..."
	@rm -rf $(OBJS) $(EXEC) $(DYLIB)

dist: all
	@echo "--- Создание дистрибутива для macOS ---"
	@# Создаем временную структуру
	@mkdir -p dist/crypto-toolkit/bin
	@# Копируем файлы
	@cp $(EXEC) dist/crypto-toolkit/
	@cp bin/*.dylib dist/crypto-toolkit/bin/
	@# Создаем архив
	@cd dist && tar -czf crypto-toolkit-macos-$(shell uname -m).tar.gz crypto-toolkit
	@# Очищаем временную папку
	@rm -rf dist/crypto-toolkit
	@echo "--- Архив готов: dist/crypto-toolkit-macos-$(shell uname -m).tar.gz ---"