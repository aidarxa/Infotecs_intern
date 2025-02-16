BUILD_DIR = build
OUT_DIR = out
INCLUDE = lib/include
LIB_SOURCES = lib/src
APPLICATION_SOURCES = app

# Компиляция и сборка приложения
app: $(OUT_DIR)/app
$(OUT_DIR)/app: $(APPLICATION_SOURCES)/Application.cpp | lib
	g++ -std=c++17 -o $@ $< -I$(INCLUDE) -L$(OUT_DIR) -llogger -Wl,-rpath=$(PWD)/$(OUT_DIR)

# Отдельно библиотеки
lib: $(OUT_DIR)/liblogger.so
$(OUT_DIR)/liblogger.so: $(BUILD_DIR)/logger.o | $(OUT_DIR)
	g++ -shared -o $@ $^

$(BUILD_DIR)/logger.o: $(LIB_SOURCES)/Logger.cpp $(INCLUDE)/Logger.h | $(BUILD_DIR)
	g++ -fPIC -O2 -std=c++17 -I$(INCLUDE) -c $< -o $@

$(BUILD_DIR) $(OUT_DIR):
	mkdir -p $@

# Очистка
clean:
	rm -rf $(BUILD_DIR)/*
clean_all:
	rm -rf $(BUILD_DIR)/* $(OUT_DIR)/*