package main

import (
	"math"
	"time"

	"github.com/veandco/go-sdl2/sdl"
)

type ship struct {
	_renderer         *sdl.Renderer
	_image1           *sdl.Texture
	_image2           *sdl.Texture
	_rect             sdl.Rect
	_xPos             float64
	_yPos             float64
	_xVel             float64
	_yVel             float64
	_offset           float64
	_size             float64
	_angle            float64
	_angleRate        float64
	_thrust           float64
	_engine           bool
	_keystate         []uint8
	_radius           float64
	_immunity         bool
	_immunityDuration time.Duration
	_immunityCounter  float64
}

func newShip(renderer *sdl.Renderer, image1 *sdl.Texture, image2 *sdl.Texture) (*ship, error) {
	s := &ship{}
	var err error

	s._renderer = renderer
	s._image1 = image1
	s._image2 = image2
	s._immunityDuration = 3 * time.Second

	if _, _, s._rect.W, s._rect.H, err = s._image1.Query(); err != nil {
		return s, err
	}

	s._keystate = sdl.GetKeyboardState()

	s._thrust = 360
	s._angleRate = 300
	s._size = 60
	s._offset = 32
	s._radius = 28

	return s, err
}

func (s *ship) reset() {
	s._rect.X = (windowWidth - s._rect.W) / 2
	s._rect.Y = (windowHeight - s._rect.H) / 2
	s._xPos = float64(s._rect.X)
	s._yPos = float64(s._rect.Y)
	s._xVel = 0
	s._yVel = 0
	s._angle = 0
	s.setImmunity()
}

func (s *ship) left() int32 {
	return int32(s._xPos + s._offset)
}

func (s *ship) right() int32 {
	return int32(s._xPos + s._size + s._offset)
}

func (s *ship) top() int32 {
	return int32(s._yPos + s._offset)
}

func (s *ship) bottom() int32 {
	return int32(s._yPos + s._size + s._offset)
}

func (s *ship) center() (float64, float64) {
	x := s._xPos + s._offset + s._size/2
	y := s._yPos + s._offset + s._size/2
	return x, y
}

func (s *ship) radius() float64 {
	return s._radius
}

func (s *ship) angle() float64 {
	return s._angle
}

func (s *ship) setImmunity() {
	s._immunity = true
	s._immunityCounter = 0
	time.AfterFunc(s._immunityDuration, func() {
		event := sdl.UserEvent{
			Type: immunityEvent,
		}
		sdl.PushEvent(&event)
	})
}

func (s *ship) unsetImmunity() {
	s._immunity = false
}

func (s *ship) immunity() bool {
	return s._immunity
}

func (s *ship) update(dt float64) {
	if s._keystate[sdl.SCANCODE_LEFT] == 1 {
		s._angle -= s._angleRate * dt
		if s._angle < 0 {
			s._angle += 360
		}
	}
	if s._keystate[sdl.SCANCODE_RIGHT] == 1 {
		s._angle += s._angleRate * dt
		if s._angle > 360 {
			s._angle -= 360
		}
	}
	if s._keystate[sdl.SCANCODE_UP] == 1 {
		radian := s._angle * math.Pi / 180
		s._xVel += math.Sin(radian) * s._thrust * dt
		s._yVel -= math.Cos(radian) * s._thrust * dt
		s._engine = true
	} else {
		s._engine = false
	}

	s._xPos += s._xVel * dt
	s._yPos += s._yVel * dt

	if s.right() < 0 {
		s._xPos = windowWidth - s._offset
	} else if s.left() > windowWidth {
		s._xPos = -s._offset - s._size
	}

	if s.bottom() < 0 {
		s._yPos = windowHeight - s._offset
	} else if s.top() > windowHeight {
		s._yPos = -s._offset - s._size
	}

	s._rect.X = int32(s._xPos + 0.5)
	s._rect.Y = int32(s._yPos + 0.5)

	if s._immunity {
		s._immunityCounter += 60 * dt
		if s._immunityCounter > 10 {
			s._immunityCounter -= 10
		}
	}
}

func (s *ship) draw() error {
	var err error
	if !s._immunity || s._immunityCounter < 5 {
		if s._engine {
			err = s._renderer.CopyEx(s._image2, nil, &s._rect, s._angle, nil, sdl.FLIP_NONE)
		} else {
			err = s._renderer.CopyEx(s._image1, nil, &s._rect, s._angle, nil, sdl.FLIP_NONE)
		}
	}

	return err
}
