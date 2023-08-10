package main

import (
	"fmt"

	"github.com/veandco/go-sdl2/sdl"
)

const (
	windowWidth     = 800
	windowHeight    = 600
	windowTitle     = "Asteroids!"
	scoreFontSize   = 20
	messageFontSize = 100
	immunityEvent   = sdl.USEREVENT
	gameModeEvent   = sdl.USEREVENT + 1
	targetFPS       = 60
)

func main() {
	game, err := newGame()
	defer game.close()
	if err != nil {
		fmt.Printf("Error: %s\n", err)
		return
	}

	if err = game.run(); err != nil {
		fmt.Printf("Error: %s\n", err)
	}
}
