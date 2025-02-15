BUILD_DIR = build
OUT_DIR = out
INCLUDE = lib/include
LIB_SOURCES = lib/src
APPLICATION_SOURCES = app
# Исполняемый файл и библиотеку наверно стоит вывести в отдельную папку, отделив от временных файлов


# Создание директорий
$(BUILD_DIR) $(OUT_DIR):
	mkdir -p $@

# Очистка
clean:
	rm -rf $(BUILD_DIR)/*
clean_all:
	rm -rf $(BUILD_DIR)/* $(OUT_DIR)/*

# Компиляция logger
$(BUILD_DIR)/logger.o: $(LIB_SOURCES)/Logger.cpp $(INCLUDE)/Logger.h | $(BUILD_DIR)
	g++ -fPIC -O2 -std=c++17 -I$(INCLUDE) -c $< -o $@

lib: $(OUT_DIR)/liblogger.so
$(OUT_DIR)/liblogger.so: $(BUILD_DIR)/logger.o | $(OUT_DIR)
	g++ -shared -o $@ $^

# Компиляция и сборка тестового приложения
test: $(OUT_DIR)/test
$(OUT_DIR)/test: $(APPLICATION_SOURCES)/Application.cpp | lib
	g++ -std=c++17 -o $@ $< -I$(INCLUDE) -L$(OUT_DIR) -llogger -Wl,-rpath=$(PWD)/$(OUT_DIR)
