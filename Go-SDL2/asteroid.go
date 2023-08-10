package main

import (
	"math"
	"math/rand"

	"github.com/veandco/go-sdl2/sdl"
)

type asteroid struct {
	_renderer  *sdl.Renderer
	_images    []*sdl.Texture
	_rects     []sdl.Rect
	_radii     []float64
	_xPos      float64
	_yPos      float64
	_xVel      float64
	_yVel      float64
	_angle     float64
	_angleRate float64
	_size      int
	_random    *rand.Rand
}

func newAsteroid(
	renderer *sdl.Renderer,
	random *rand.Rand,
	largeImage *sdl.Texture,
	mediumImage *sdl.Texture,
	smallImage *sdl.Texture,
	angle float64,
	xPos float64,
	yPos float64,
	xVel float64,
	yVel float64,
	size int,
) (*asteroid, error) {
	a := &asteroid{}
	var err error

	a._renderer = renderer
	a._random = random
	a._images = append(a._images, largeImage)
	a._images = append(a._images, mediumImage)
	a._images = append(a._images, smallImage)
	a._angle = angle
	a._xPos = xPos
	a._yPos = yPos
	a._xVel = xVel
	a._yVel = yVel
	a._size = size

	for size := range a._images {
		var rect sdl.Rect
		if _, _, rect.W, rect.H, err = a._images[size].Query(); err != nil {
			return a, err
		}
		a._rects = append(a._rects, rect)
		a._radii = append(a._radii, float64(rect.W/2))
	}

	if xPos == 0 || yPos == 0 {
		var centerX float64 = (windowWidth - float64(a._rects[a._size].W)) / 2
		var centerY float64 = (windowHeight - float64(a._rects[a._size].H)) / 2
		for {
			a._xPos = a._random.Float64() * windowWidth
			a._yPos = a._random.Float64() * windowHeight
			if math.Hypot(centerX-a._xPos, centerY-a._yPos) > 150 {
				break
			}
		}
	}

	a._rects[a._size].X = int32(a._xPos + 0.5)
	a._rects[a._size].Y = int32(a._yPos + 0.5)

	if angle == 0 {
		a._angle = a._random.Float64() * 360
	}

	a.reset()

	return a, err
}

func (a *asteroid) reset() {
	a._angleRate = (a._random.Float64()*2 - 1) * 60
	radian := float64(a._angle) * math.Pi / 180
	speed := a._random.Float64() * 60
	a._xVel += math.Sin(radian) * speed
	a._yVel -= math.Cos(radian) * speed
}

func (a *asteroid) split(angle float64) (*asteroid, error) {
	var err error
	xPos, yPos := a.center()
	a._size += 1
	if a._size > len(a._images)-1 {
		return nil, err
	}
	a._xPos = xPos - float64(a._rects[a._size].W)/2
	a._yPos = yPos - float64(a._rects[a._size].H)/2
	a._rects[a._size].X = int32(a._xPos + 0.5)
	a._rects[a._size].Y = int32(a._yPos + 0.5)

	angle += 90
	if angle > 360 {
		angle -= 360
	}
	newA := &asteroid{}
	if newA, err = newAsteroid(a._renderer, a._random, a._images[0], a._images[1], a._images[2], angle, a._xPos, a._yPos, a._xVel, a._yVel, a.size()); err != nil {
		return newA, err
	}

	angle += 180
	if angle > 360 {
		angle -= 360
	}
	a._angle = angle
	a.reset()

	return newA, err
}

func (a *asteroid) left() int32 {
	return int32(a._xPos)
}

func (a *asteroid) right() int32 {
	return int32(a._xPos) + a._rects[a._size].W
}

func (a *asteroid) top() int32 {
	return int32(a._yPos)
}

func (a *asteroid) bottom() int32 {
	return int32(a._yPos) + a._rects[a._size].H
}

func (a *asteroid) center() (float64, float64) {
	x := a._xPos + float64(a._rects[a._size].W)/2
	y := a._yPos + float64(a._rects[a._size].H)/2
	return x, y
}

func (a *asteroid) radius() float64 {
	return a._radii[a._size]
}

func (a *asteroid) size() int {
	return a._size
}

func (a *asteroid) update(dt float64) {
	a._angle += a._angleRate * dt
	if a._angle > 360 {
		a._angle -= 360
	} else if a._angle < 0 {
		a._angle += 360
	}

	a._xPos += a._xVel * dt
	a._yPos += a._yVel * dt

	if a.right() < 0 {
		a._xPos = windowWidth
	} else if a.left() > windowWidth {
		a._xPos = -float64(a._rects[a._size].W)
	}

	if a.bottom() < 0 {
		a._yPos = windowHeight
	} else if a.top() > windowHeight {
		a._yPos = -float64(a._rects[a._size].H)
	}

	a._rects[a._size].X = int32(a._xPos + 0.5)
	a._rects[a._size].Y = int32(a._yPos + 0.5)
}

func (a *asteroid) draw() error {
	err := a._renderer.CopyEx(a._images[a._size], nil, &a._rects[a._size], a._angle, nil, sdl.FLIP_NONE)

	return err
}
