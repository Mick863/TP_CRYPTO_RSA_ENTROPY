CC = gcc
CFLAGS = -Wall -Iinclude -std=c11 -O1
SRCDIR = src
BUILDDIR = build
TARGET = $(BUILDDIR)/rsa_sim

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Nettoyage du build..."
	@rm -rf $(BUILDDIR)

run: all
	./$(TARGET)

debug: CFLAGS += -g
debug: clean all

.PHONY: all clean run debug

