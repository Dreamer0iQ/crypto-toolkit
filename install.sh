#!/bin/bash
#
# Скрипт для скачивания и подготовки к использованию последнего релиза
# crypto-toolkit с GitHub. Автоматически определяет ОС и архитектуру.
#

# Выход при любой ошибке
set -e

# --- Конфигурация ---
REPO="Dreamer0iQ/crypto-toolkit"
API_URL="https://api.github.com/repos/$REPO/releases/latest"
# --- Конец конфигурации ---

echo "Поиск последнего релиза для crypto-toolkit..."

# 1. Определяем операционную систему и расширение файла
OS=""
FILE_EXT=""
case "$(uname -s)" in
    Linux*)
        OS='linux'
        FILE_EXT='.deb'
        ;;
    Darwin*)
        OS='macos'
        FILE_EXT='.tar.gz'
        ;;
    *)
        echo "Ошибка: Ваша операционная система не поддерживается." >&2
        exit 1
        ;;
esac

# 2. Определяем архитектуру процессора
ARCH=""
case "$(uname -m)" in
    x86_64)     ARCH='amd64';;
    arm64)      ARCH='arm64';;
    aarch64)    ARCH='arm64';;
    *)
        echo "Ошибка: Ваша архитектура процессора не поддерживается." >&2
        exit 1
        ;;
esac

echo "Ваша система: $OS ($ARCH)"

# 3. Ищем URL для скачивания нужного файла
# Ищем файл, который содержит ОС, архитектуру и нужное расширение
DOWNLOAD_URL=$(curl -s "$API_URL" | \
    grep "browser_download_url" | \
    grep -i "$OS" | \
    grep -i "$ARCH" | \
    grep "$FILE_EXT" | \
    sed -E 's/.*"browser_download_url": "([^"]+)".*/\1/' | \
    head -n 1) # Берем первый найденный на всякий случай

# 4. Проверяем, найден ли URL
if [ -z "$DOWNLOAD_URL" ]; then
    echo "Ошибка: Не удалось найти готовый файл для скачивания для вашей системы ($OS-$ARCH$FILE_EXT)." >&2
    echo "Пожалуйста, проверьте страницу релизов вручную: https://github.com/$REPO/releases" >&2
    echo ""
    echo "---"
    echo "В качестве альтернативы, вы можете собрать проект из исходного кода:"
    echo "1. Убедитесь, что у вас установлены git, make и компилятор C++ (g++ или clang++)."
    echo "2. Выполните следующие команды:"
    echo "   git clone https://github.com/$REPO.git"
    echo "   cd crypto-toolkit"
    echo "   make all"
    echo "   # После этого вы можете запустить приложение командой 'make run'"
    exit 1
fi

# 5. Скачиваем файл
FILENAME=$(basename "$DOWNLOAD_URL")
echo "Найден подходящий файл: $FILENAME"
echo "Начинаю скачивание..."

curl -L -o "$FILENAME" "$DOWNLOAD_URL"

echo ""
echo "Скачивание завершено!"
echo ""

# 6. Выводим инструкцию по установке в зависимости от типа файла
if [ "$OS" = "linux" ]; then
    echo "Чтобы установить пакет, выполните команду:"
    echo "sudo dpkg -i $FILENAME"
elif [ "$OS" = "macos" ]; then
    echo "Распаковываю архив..."
    tar -xzf "$FILENAME"
    
    echo ""
    echo "Готово! Приложение и его библиотеки находятся в папке 'crypto-toolkit'."
    echo "Чтобы запустить, перейдите в эту папку и выполните команду:"
    echo "cd crypto-toolkit && ./crypto_app"
fi

exit 0