package main

import (
	"math"

	"github.com/veandco/go-sdl2/sdl"
)

type shot struct {
	_renderer    *sdl.Renderer
	_image       *sdl.Texture
	_rect        sdl.Rect
	_radius      float64
	_xPos        float64
	_yPos        float64
	_xVel        float64
	_yVel        float64
	_angle       float64
	_speed       float64
	_lifeTime    float64
	_lifeCounter float64
}

func newShot(renderer *sdl.Renderer, image *sdl.Texture, xPos float64, yPos float64, angle float64) (*shot, error) {
	s := &shot{}
	var err error

	s._renderer = renderer
	s._image = image
	s._angle = angle
	s._speed = 600
	s._xPos = xPos
	s._yPos = yPos
	s._lifeTime = 40
	s._lifeCounter = 0

	if _, _, s._rect.W, s._rect.H, err = s._image.Query(); err != nil {
		return s, err
	}

	s._radius = float64(s._rect.W / 2)

	radian := float64(s._angle) * math.Pi / 180
	s._xVel += math.Sin(radian) * s._speed
	s._yVel -= math.Cos(radian) * s._speed

	s._xPos -= float64(s._rect.W) / 2
	s._yPos -= float64(s._rect.H) / 2

	s._xPos += s._xVel * 0.05
	s._yPos += s._yVel * 0.05

	s._rect.X = int32(s._xPos + 0.5)
	s._rect.Y = int32(s._yPos + 0.5)

	return s, err
}

func (s *shot) left() int32 {
	return int32(s._xPos)
}

func (s *shot) right() int32 {
	return int32(s._xPos) + s._rect.W
}

func (s *shot) top() int32 {
	return int32(s._yPos)
}

func (s *shot) bottom() int32 {
	return int32(s._yPos) + s._rect.H
}

func (s *shot) center() (float64, float64) {
	x := s._xPos + float64(s._rect.W)/2
	y := s._yPos + float64(s._rect.H)/2
	return x, y
}

func (s *shot) radius() float64 {
	return s._radius
}

func (s *shot) angle() float64 {
	return s._angle
}

func (s *shot) alive() bool {
	return s._lifeCounter < s._lifeTime
}

func (s *shot) update(dt float64) {
	s._xPos += s._xVel * dt
	s._yPos += s._yVel * dt

	if s.right() < 0 {
		s._xPos = windowWidth
	} else if s.left() > windowWidth {
		s._xPos = -float64(s._rect.W)
	}

	if s.bottom() < 0 {
		s._yPos = windowHeight
	} else if s.top() > windowHeight {
		s._yPos = -float64(s._rect.H)
	}

	s._rect.X = int32(s._xPos + 0.5)
	s._rect.Y = int32(s._yPos + 0.5)

	s._lifeCounter += 60 * dt
}

func (s *shot) draw() error {
	err := s._renderer.CopyEx(s._image, nil, &s._rect, s._angle, nil, sdl.FLIP_NONE)

	return err
}
