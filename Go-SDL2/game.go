package main

import (
	"math"
	"math/rand"
	"strconv"
	"time"

	"github.com/veandco/go-sdl2/img"
	"github.com/veandco/go-sdl2/mix"
	"github.com/veandco/go-sdl2/sdl"
	"github.com/veandco/go-sdl2/ttf"
)

type game struct {
	window              *sdl.Window
	renderer            *sdl.Renderer
	backgroundImage     *sdl.Texture
	backgroundRect      sdl.Rect
	shipImage1          *sdl.Texture
	shipImage2          *sdl.Texture
	asteroidLargeImage  *sdl.Texture
	asteroidMediumImage *sdl.Texture
	asteroidSmallImage  *sdl.Texture
	iconImage           *sdl.Texture
	iconRect            sdl.Rect
	shotImage           *sdl.Texture
	fallingMusic        *mix.Music
	boomSound           *mix.Chunk
	collideSound        *mix.Chunk
	laserSound          *mix.Chunk
	thrusterSound       *mix.Chunk
	score               *score
	message             *message
	ship                *ship
	asteroids           []*asteroid
	shots               []*shot
	fps                 *fps
	random              *rand.Rand
	lives               int
	running             bool
	shotCount           int
	level               int
	showGame            bool
	showMessage         bool
	gameModeDuration    time.Duration
	gameMode            int
	deltaTime           float64
}

func newGame() (*game, error) {
	g := &game{}
	var err error

	if err = g.initialize(); err != nil {
		return g, err
	}

	if err = g.loadMedia(); err != nil {
		return g, err
	}

	if g.score, err = newScore(g.renderer); err != nil {
		return g, err
	}

	if g.message, err = newMessage(g.renderer); err != nil {
		return g, err
	}

	if g.ship, err = newShip(g.renderer, g.shipImage1, g.shipImage2); err != nil {
		return g, err
	}

	g.fps = newFps()

	g.iconRect.Y = 10
	g.running = true
	g.gameModeDuration = 5 * time.Second
	g.gameMode = 0

	return g, err
}

func (g *game) reset() error {
	var err error

	g.shotCount = 101

	g.ship.reset()

	g.shots = nil
	g.asteroids = nil

	for i := 0; i < g.level; i++ {
		asteroid, err := newAsteroid(g.renderer, g.random, g.asteroidLargeImage, g.asteroidMediumImage, g.asteroidSmallImage, 0, 0, 0, 0, 0, 0)
		if err != nil {
			return err
		}
		g.asteroids = append(g.asteroids, asteroid)
	}

	return err
}

func (g *game) setGameMode() error {
	var err error
	g.gameMode++
	if g.gameMode > 4 {
		g.gameMode = 1
	}
	switch g.gameMode {
	case 1:
		g.lives = 0
		g.level = 0
		if err = g.score.reset(); err != nil {
			return err
		}
		if err = g.message.message("Asteroids!"); err != nil {
			return err
		}
		g.showGame = false
		g.showMessage = true

	case 2:
		g.lives++
		g.level++
		if err = g.message.message("Level: " + strconv.Itoa(g.level)); err != nil {
			return err
		}
		g.showGame = false
		g.showMessage = true
	case 3:
		g.reset()
		g.showGame = true
		g.showMessage = false
	case 4:
		if err = g.message.message("Game Over"); err != nil {
			return err
		}
		g.showGame = true
		g.showMessage = true
	}

	if g.gameMode != 3 {
		time.AfterFunc(g.gameModeDuration, func() {
			event := sdl.UserEvent{
				Type: gameModeEvent,
			}
			sdl.PushEvent(&event)
		})
	}
	return err
}

func (g *game) initialize() error {
	var err error

	if err = sdl.Init(sdl.INIT_VIDEO | sdl.INIT_TIMER | sdl.INIT_AUDIO); err != nil {
		return err
	}

	if err = mix.OpenAudio(44100, mix.DEFAULT_FORMAT, 2, 1024); err != nil {
		return err
	}

	if err = ttf.Init(); err != nil {
		return err
	}

	if g.window, err = sdl.CreateWindow(windowTitle, sdl.WINDOWPOS_UNDEFINED, sdl.WINDOWPOS_UNDEFINED, windowWidth, windowHeight, sdl.WINDOW_SHOWN); err != nil {
		return err
	}

	if g.renderer, err = sdl.CreateRenderer(g.window, -1, sdl.RENDERER_ACCELERATED); err != nil {
		return err
	}

	iconSurf, err := img.Load("images/icon.png")
	defer iconSurf.Free()
	if err != nil {
		return err
	} else {
		g.window.SetIcon(iconSurf)
	}

	g.random = rand.New(rand.NewSource(time.Now().UnixNano()))

	return err
}

func (g *game) loadMedia() error {
	var err error

	if g.backgroundImage, err = img.LoadTexture(g.renderer, "images/background.png"); err != nil {
		return err
	}

	if _, _, g.backgroundRect.W, g.backgroundRect.H, err = g.backgroundImage.Query(); err != nil {
		return err
	}

	if g.shipImage1, err = img.LoadTexture(g.renderer, "images/ship1.png"); err != nil {
		return err
	}

	if g.shipImage2, err = img.LoadTexture(g.renderer, "images/ship2.png"); err != nil {
		return err
	}

	if g.asteroidLargeImage, err = img.LoadTexture(g.renderer, "images/asteroidlarge.png"); err != nil {
		return err
	}

	if g.asteroidMediumImage, err = img.LoadTexture(g.renderer, "images/asteroidmedium.png"); err != nil {
		return err
	}

	if g.asteroidSmallImage, err = img.LoadTexture(g.renderer, "images/asteroidsmall.png"); err != nil {
		return err
	}

	if g.iconImage, err = img.LoadTexture(g.renderer, "images/icon.png"); err != nil {
		return err
	}

	if _, _, g.iconRect.W, g.iconRect.H, err = g.iconImage.Query(); err != nil {
		return err
	}

	if g.shotImage, err = img.LoadTexture(g.renderer, "images/shot.png"); err != nil {
		return err
	}

	if g.fallingMusic, err = mix.LoadMUS("music/falling.ogg"); err != nil {
		return err
	}

	if g.boomSound, err = mix.LoadWAV("sounds/boom.ogg"); err != nil {
		return err
	}

	if g.collideSound, err = mix.LoadWAV("sounds/collide.ogg"); err != nil {
		return err
	}

	if g.laserSound, err = mix.LoadWAV("sounds/laser.ogg"); err != nil {
		return err
	}

	if g.thrusterSound, err = mix.LoadWAV("sounds/thrusters.ogg"); err != nil {
		return err
	}

	return err
}

func (g *game) close() {
	if g.score != nil {
		g.score.close()
	}
	if g.message != nil {
		g.message.close()
	}
	g.thrusterSound.Free()
	g.laserSound.Free()
	g.collideSound.Free()
	g.boomSound.Free()
	g.fallingMusic.Free()
	g.shotImage.Destroy()
	g.iconImage.Destroy()
	g.asteroidSmallImage.Destroy()
	g.asteroidMediumImage.Destroy()
	g.asteroidLargeImage.Destroy()
	g.shipImage1.Destroy()
	g.shipImage2.Destroy()
	g.backgroundImage.Destroy()
	g.renderer.Destroy()
	g.window.Destroy()
	ttf.Quit()
	mix.Quit()
	sdl.Quit()
}

func (g *game) checkShipCollision() error {
	var err error
	for _, asteroid := range g.asteroids {
		if asteroid.right() > g.ship.left() && asteroid.left() < g.ship.right() {
			if asteroid.bottom() > g.ship.top() && asteroid.top() < g.ship.bottom() {
				x1, y1 := asteroid.center()
				r1 := asteroid.radius()
				x2, y2 := g.ship.center()
				r2 := g.ship.radius()
				if checkCircleCollision(x1, y1, r1, x2, y2, r2) {
					g.collideSound.Play(-1, 0)
					if g.lives == 0 {
						if err = g.setGameMode(); err != nil {
							return err
						}
					} else {
						g.ship.setImmunity()
						g.lives -= 1
					}
				}
			}
		}
	}
	return err
}

func (g *game) checkShotCollision() error {
	var err error
	if len(g.shots) > 0 && len(g.asteroids) > 0 {
		for i := len(g.asteroids) - 1; i >= 0; i-- {
			asteroid := g.asteroids[i]
			for j := len(g.shots) - 1; j >= 0; j-- {
				shot := g.shots[j]
				if g.asteroids[i].right() > shot.left() && asteroid.left() < shot.right() {
					if asteroid.bottom() > shot.top() && asteroid.top() < shot.bottom() {
						x1, y1 := asteroid.center()
						x2, y2 := shot.center()
						if checkCircleCollision(x1, y1, asteroid.radius(), x2, y2, shot.radius()) {
							g.boomSound.Play(-1, 0)
							if err = g.score.increment(g.shotCount * g.level); err != nil {
								return err
							}
							newA, err := asteroid.split(shot.angle())
							if err != nil {
								return err
							}
							if newA == nil {
								g.asteroids = append(g.asteroids[:i], g.asteroids[i+1:]...)
								if len(g.asteroids) == 0 {
									g.gameMode = 1
									if err = g.setGameMode(); err != nil {
										return err
									}
									return nil
								}
							} else {
								g.asteroids = append(g.asteroids, newA)
							}
							g.shots = append(g.shots[:j], g.shots[j+1:]...)
							break
						}
					}
				}
			}
		}
	}
	return err
}

func checkCircleCollision(x1 float64, y1 float64, r1 float64, x2 float64, y2 float64, r2 float64) bool {
	return (r1 + r2) > math.Hypot(x1-x2, y1-y2)
}

func (g *game) shoot() error {
	if len(g.shots) < 5 {
		g.laserSound.Play(-1, 0)
		xPos, yPos := g.ship.center()
		if shot, err := newShot(g.renderer, g.shotImage, xPos, yPos, g.ship.angle()); err != nil {
			return err
		} else {
			g.shots = append(g.shots, shot)
		}
		if g.shotCount > 1 {
			g.shotCount -= 1
		}
	}
	return nil
}

func (g *game) run() error {
	var err error

	if err = g.setGameMode(); err != nil {
		return err
	}

	if err = g.fallingMusic.Play(-1); err != nil {
		return err
	}

	for g.running {
		for event := sdl.PollEvent(); event != nil; event = sdl.PollEvent() {
			switch t := event.(type) {
			case *sdl.QuitEvent:
				g.running = false
			case *sdl.UserEvent:
				if t.Type == immunityEvent {
					g.ship.unsetImmunity()
				}
				if t.Type == gameModeEvent {
					if err = g.setGameMode(); err != nil {
						return err
					}
				}
			case *sdl.KeyboardEvent:
				if t.Type == sdl.KEYDOWN {
					switch t.Keysym.Sym {
					case sdl.K_ESCAPE:
						g.running = false
					case sdl.K_f:
						g.fps.setShowFps()
					case sdl.K_SPACE:
						if g.showGame {
							if err = g.shoot(); err != nil {
								return err
							}
						}
					}
				}
			}
		}

		if !g.showMessage {
			for _, asteroid := range g.asteroids {
				asteroid.update(g.deltaTime)
			}

			for i := len(g.shots) - 1; i >= 0; i-- {
				if g.shots[i].alive() {
					g.shots[i].update(g.deltaTime)
				} else {
					g.shots = append(g.shots[:i], g.shots[i+1:]...)
				}
			}
			g.ship.update(g.deltaTime)

			if !g.ship.immunity() {
				if err = g.checkShipCollision(); err != nil {
					return err
				}
			}

			if err = g.checkShotCollision(); err != nil {
				return err
			}

		}

		g.renderer.Clear()

		g.renderer.Copy(g.backgroundImage, nil, &g.backgroundRect)

		if g.showGame {
			for _, asteroid := range g.asteroids {
				asteroid.draw()
			}
			for _, shot := range g.shots {
				shot.draw()
			}

			if g.lives > 0 {
				for i := 0; i < g.lives; i++ {
					g.iconRect.X = int32(30*i) + 10
					g.renderer.Copy(g.iconImage, nil, &g.iconRect)
				}
			}
			g.ship.draw()
			g.score.draw()
		}

		if g.showMessage {
			g.message.draw()
		}

		g.renderer.Present()

		g.deltaTime = g.fps.update()
	}
	return err
}
