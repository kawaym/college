import * as THREE from 'three'

export class RendererManager {
  constructor(container) {
    const { clientWidth: w, clientHeight: h } = container

    this._renderer = new THREE.WebGLRenderer({ antialias: true })
    this._renderer.setSize(w, h)
    this._renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2))
    this._renderer.shadowMap.enabled = true
    this._renderer.shadowMap.type = THREE.PCFSoftShadowMap

    container.appendChild(this._renderer.domElement)
  }

  get() {
    return this._renderer
  }

  onResize(width, height) {
    this._renderer.setSize(width, height)
    this._renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2))
  }

  dispose() {
    this._renderer.dispose()
  }
}
