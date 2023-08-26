CC				?= gcc
CFLAGS_BASE		= `pkg-config --cflags sdl2 SDL2_image SDL2_ttf SDL2_mixer` -std=gnu99
CFLAGS_RELEASE	= -O2
CFLAGS_DEV		= -O0 -ggdb3 -Wall -Wextra -Wwrite-strings -Werror -Wunused -Wuninitialized \
                 -Wconversion -Wshadow -Wformat -Wmissing-prototypes -Wmissing-declarations \
				 -Wfloat-equal -Wsign-compare -fsanitize=address
CFLAGS			= $(CFLAGS_BASE) $(CFLAGS_DEV)
LDLIBS_BASE		= `pkg-config --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer` -lm
LDLIBS_DEV      = -fsanitize=address
LDLIBS          = $(LDLIBS_BASE) $(LDLIBS_DEV)
TARGET			= Asteroids

SRCS			= $(wildcard *.c)
BUILD_DIR 		= .build
OBJS    		= $(addprefix $(BUILD_DIR)/, $(SRCS:.c=.o))

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c %.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDLIBS) $^ -o $@

.PHONY: all rebuild release run clean

all: $(TARGET)

release: CFLAGS = $(CFLAGS_BASE) $(CFLAGS_RELEASE)
release: LDLIBS = $(LDLIBS_BASE)
release: all

run: $(TARGET)
	./$<

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET)