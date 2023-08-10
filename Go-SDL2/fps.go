package main

import (
	"fmt"
	"time"
)

type fps struct {
	targetFrameDuration time.Duration
	lastTime            time.Time
	carryDelay          time.Duration
	lastFpsTime         time.Time
	fpsCounter          int32
	showFps             bool
}

func newFps() *fps {
	f := &fps{}
	f.targetFrameDuration = time.Second / targetFPS
	f.lastTime = time.Now()
	f.lastFpsTime = f.lastTime
	return f
}

func (f *fps) setShowFps() {
	if f.showFps {
		f.showFps = false
	} else {
		f.lastFpsTime = time.Now()
		f.fpsCounter = 0
		f.showFps = true
	}
}

func (f *fps) update() float64 {
	elapsedTime := time.Since(f.lastTime)

	sleepDuration := f.targetFrameDuration + f.carryDelay - elapsedTime
	if sleepDuration > 0 {
		time.Sleep(sleepDuration)
		elapsedTime = time.Since(f.lastTime)
	}

	f.lastTime = time.Now()

	f.carryDelay = f.targetFrameDuration + f.carryDelay - elapsedTime
	if f.carryDelay > f.targetFrameDuration {
		f.carryDelay = f.targetFrameDuration
	} else if f.carryDelay < -f.targetFrameDuration {
		f.carryDelay = -f.targetFrameDuration
	}

	if f.showFps {
		f.fpsCounter += 1
		if time.Since(f.lastFpsTime) > time.Second {
			fmt.Println(f.fpsCounter)
			f.fpsCounter = 0
			f.lastFpsTime = f.lastFpsTime.Add(time.Second)
		}
	}

	return elapsedTime.Seconds()
}
