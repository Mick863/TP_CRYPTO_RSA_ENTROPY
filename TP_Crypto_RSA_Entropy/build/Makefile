# === Variables ===
CC = gcc
CFLAGS = -Wall -Iinclude -std=c11
SRCDIR = src
BUILDDIR = build
TARGET = $(BUILDDIR)/rsa_sim

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

# === Règles ===
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

run: all
	./$(TARGET)

.PHONY: all clean run
