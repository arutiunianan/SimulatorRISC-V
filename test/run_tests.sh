#!/bin/bash

PROGRAM="$1"
TEST_DIR="${2:-.}"
FILE_EXT="o"
EXPECTED_EXT="txt"

# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Счетчики результатов
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
SKIPPED_TESTS=0

# Функция для запуска одного теста
run_test() {
    local o_file="$1"
    local expected_file="$2"
    local test_name
    
    # Извлекаем имя теста (без расширения)
    test_name="$(basename "$o_file" ".$FILE_EXT")"
    
    echo -e "\n${BLUE}=== Тестирование: $test_name ===${NC}"
    echo "Запускаем: $PROGRAM $o_file"
    echo "Ожидаемый вывод: $expected_file"
    
    # Проверяем наличие файла с ожидаемым выводом
    if [ ! -f "$expected_file" ]; then
        echo -e "${YELLOW}Пропущено: файл $expected_file не найден${NC}"
        ((SKIPPED_TESTS++))
        return 1
    fi
    
    # Проверяем наличие тестируемого .o файла
    if [ ! -f "$o_file" ]; then
        echo -e "${YELLOW}Пропущено: файл $o_file не найден${NC}"
        ((SKIPPED_TESTS++))
        return 1
    fi
    
    # Временные файлы
    local tmp_output
    tmp_output=$(mktemp)
    local tmp_diff
    tmp_diff=$(mktemp)
    
    # Запускаем вашу программу с .o файлом как аргументом
    echo -e "Выполняется: $PROGRAM $o_file"
    "$PROGRAM" "$o_file" 2> "$tmp_output"
    local program_exit_code=$?
    
    # Проверяем код завершения вашей программы
    if [ $program_exit_code -ne 0 ] && [ $program_exit_code -ne 1 ]; then
        echo -e "${YELLOW}Предупреждение: $PROGRAM завершилась с кодом $program_exit_code${NC}"
    fi
    
    # Сравниваем вывод с ожидаемым
    if diff -u "$expected_file" "$tmp_output" > "$tmp_diff"; then
        echo -e "${GREEN}✓ ТЕСТ ПРОЙДЕН: $test_name${NC}"
        ((PASSED_TESTS++))
        TEST_RESULT=0
    else
        echo -e "${RED}✗ ТЕСТ ПРОВАЛЕН: $test_name${NC}"
        echo "Обнаружены различия:"
        echo "------------------------"
        cat "$tmp_diff"
        echo "------------------------"
        ((FAILED_TESTS++))
        TEST_RESULT=1
        
        # Дополнительная информация для отладки
        echo -e "\n${YELLOW}Информация для отладки:${NC}"
        echo "Размер вывода $PROGRAM: $(wc -l < "$tmp_output") строк"
        echo "Размер ожидаемого вывода: $(wc -l < "$expected_file") строк"
        
        if [ -s "$tmp_output" ]; then
            echo -e "\nПервые 10 строк вывода $PROGRAM:"
            head -10 "$tmp_output" | sed 's/^/  /'
        else
            echo -e "\nВывод $PROGRAM пуст!"
        fi
    fi
    
    # Очистка временных файлов
    rm -f "$tmp_output" "$tmp_diff"
    
    ((TOTAL_TESTS++))
    return $TEST_RESULT
}

# Основная функция
main() {
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}   ТЕСТИРОВАНИЕ С ИСПОЛЬЗОВАНИЕМ .o ФАЙЛОВ   ${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo "Программа: $PROGRAM"
    echo "Директория: $TEST_DIR"
    
    # Проверяем, существует ли программа
    if [ ! -f "$PROGRAM" ] && ! command -v "$PROGRAM" >/dev/null 2>&1; then
        echo -e "${RED}Ошибка: программа '$PROGRAM' не найдена${NC}"
        exit 1
    fi
    
    # Делаем программу исполняемой (если она не в PATH)
    if [ -f "$PROGRAM" ] && [ ! -x "$PROGRAM" ]; then
        echo -e "${YELLOW}Делаем программу исполняемой...${NC}"
        chmod +x "$PROGRAM"
    fi
    
    # Находим все .o файлы
    local o_files
    local o_files=()
    
    # Совместимый способ для macOS и старых bash
    while IFS= read -r -d $'\0' file; do
        o_files+=("$file")
    done < <(find "$TEST_DIR" -maxdepth 1 -name "*.$FILE_EXT" -type f -print0 2>/dev/null | sort -z)
    
    if [ ${#o_files[@]} -eq 0 ]; then
        echo -e "${YELLOW}Предупреждение: не найдено файлов с расширением .$FILE_EXT${NC}"
        echo "Убедитесь, что:"
        echo "1. Вы находитесь в правильной директории"
        echo "2. Файлы .$FILE_EXT существуют"
        exit 1
    fi
    
    echo -e "Найдено .o файлов для тестирования: ${#o_files[@]}"
    
    # Запускаем все тесты
    for o_file in "${o_files[@]}"; do
        # Формируем имя файла с ожидаемым выводом
        local base_name
        base_name="$(basename "$o_file" ".$FILE_EXT")"
        local expected_file="$TEST_DIR/$base_name.$EXPECTED_EXT"
        
        run_test "$o_file" "$expected_file"
    done
    
    # Выводим итоговую статистику
    echo -e "\n${BLUE}========================================${NC}"
    echo -e "${BLUE}             ИТОГИ ТЕСТИРОВАНИЯ             ${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo -e "Всего тестов:    $TOTAL_TESTS"
    echo -e "${GREEN}Пройдено:        $PASSED_TESTS${NC}"
    
    if [ $FAILED_TESTS -gt 0 ]; then
        echo -e "${RED}Провалено:      $FAILED_TESTS${NC}"
    else
        echo -e "Провалено:      $FAILED_TESTS"
    fi
    
    if [ $SKIPPED_TESTS -gt 0 ]; then
        echo -e "${YELLOW}Пропущено:      $SKIPPED_TESTS${NC}"
    else
        echo -e "Пропущено:      $SKIPPED_TESTS"
    fi
    
    # Рассчитываем процент успешных тестов
    if [ $TOTAL_TESTS -gt 0 ]; then
        local success_percent
        success_percent=$((PASSED_TESTS * 100 / TOTAL_TESTS))
        echo -e "Успешность:     $success_percent%"
    fi
    
    # Возвращаем код завершения
    if [ $FAILED_TESTS -gt 0 ]; then
        echo -e "\n${RED}Некоторые тесты провалились!${NC}"
        exit 1
    elif [ $PASSED_TESTS -eq $TOTAL_TESTS ] && [ $TOTAL_TESTS -gt 0 ]; then
        echo -e "\n${GREEN}Все тесты пройдены успешно!${NC}"
        exit 0
    else
        exit 0
    fi
}

# Функция для отображения справки
show_help() {
    echo "Использование: $0 [ПРОГРАММА] [ДИРЕКТОРИЯ С ТЕСТАМИ]"
    echo ""
    echo "Запускает программу $PROGRAM для каждого .o файла в указанной директории."
    echo "Для каждого файла file.o запускает:"
    echo "    ./program file.o"
    echo "И сравнивает рузельтат с файлом file.txt"
    echo ""
    echo "Примеры:"
    echo "  $0                    # показывает эту справку"
    echo "  $0 ./program          # тестирует в текущей директории"
    echo "  $0 ./program ./tests  # тестирует в указанной директории"
    echo "  $0 --help             # показывает эту справку"
    echo ""
    echo "Требования:"
    echo "  1. Программа должна существовать"
    echo "  2. Для каждого .o файла должен существовать соответствующий .txt файл"
    echo ""
    echo "Пример структуры файлов:"
    echo "  test1.o     # файл для тестирования"
    echo "  test1.txt   # ожидаемый результат для test1.o"
    echo "  test2.o"
    echo "  test2.txt"
    echo ""
}

# Обработка аргументов командной строки
case "$1" in
    --help|-h|"")
        show_help
        exit 0
        ;;
    *)
        main
        ;;
esac