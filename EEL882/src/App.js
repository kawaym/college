import { SceneManager } from './core/SceneManager.js'
import { CameraManager } from './core/CameraManager.js'
import { RendererManager } from './core/RendererManager.js'
import { ResizeHandler } from './utils/ResizeHandler.js'
import { ZoetropeAnimation } from './animations/ZoetropeAnimation.js'
import { CinematicEvent } from './animations/CinematicEvent.js'
import { RoomEnvironment } from './scene/RoomEnvironment.js'

export class App {
  constructor(container) {
    this.container = container
    this.rafId = null

    this.scene = new SceneManager()
    this.camera = new CameraManager(container)
    this.renderer = new RendererManager(container)
    this.resize = new ResizeHandler(container, this.camera, this.renderer)

    // Cenário: mesa, chão, parede, iluminação vintage
    this.room = new RoomEnvironment(this.scene.get())

    // Zootrópio posicionado sobre a mesa
    this.zoetrope = new ZoetropeAnimation(this.scene.get())

    // Evento cinemático: monitora velocidade → LERP de câmera
    this.cinematic = new CinematicEvent(
      this.camera.get(),
      this.zoetrope,
      this.room
    )

    this.animations = [
      this.zoetrope,
      this.cinematic,
    ]
  }

  start() {
    this.resize.listen()
    this._loop()
  }

  stop() {
    cancelAnimationFrame(this.rafId)
    this.resize.destroy()
    this.renderer.dispose()
  }

  _loop(time = 0) {
    this.rafId = requestAnimationFrame((t) => this._loop(t))

    const delta = this._delta(time)
    this.animations.forEach((a) => a.update(time, delta))
    this.renderer.get().render(this.scene.get(), this.camera.get())
  }

  _delta(time) {
    const prev = this._prevTime ?? time
    this._prevTime = time
    return (time - prev) / 1000
  }
}
