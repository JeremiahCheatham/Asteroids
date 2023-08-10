package main

import (
	"github.com/veandco/go-sdl2/sdl"
	"github.com/veandco/go-sdl2/ttf"
)

type message struct {
	_renderer  *sdl.Renderer
	_font      *ttf.Font
	_fontColor sdl.Color
	_image     *sdl.Texture
	_rect      sdl.Rect
}

func newMessage(renderer *sdl.Renderer) (*message, error) {
	m := &message{}
	var err error

	m._renderer = renderer

	if m._font, err = ttf.OpenFont("fonts/freesansbold.ttf", messageFontSize); err != nil {
		return m, err
	}

	m._fontColor = sdl.Color{R: 255, G: 255, B: 255, A: 255}

	return m, err
}

func (m *message) close() {
	m._font.Close()
	m._image.Destroy()
}

func (m *message) message(text string) error {
	var messageSurf *sdl.Surface
	var err error

	if messageSurf, err = m._font.RenderUTF8Blended(text, m._fontColor); err != nil {
		return err
	}
	defer messageSurf.Free()

	m._image.Destroy()
	if m._image, err = m._renderer.CreateTextureFromSurface(messageSurf); err != nil {
		return err
	}

	if _, _, m._rect.W, m._rect.H, err = m._image.Query(); err != nil {
		return err
	}

	m._rect.X = (windowWidth - m._rect.W) / 2
	m._rect.Y = (windowHeight - m._rect.H) / 2

	return err
}

func (m *message) draw() {
	m._renderer.Copy(m._image, nil, &m._rect)
}
