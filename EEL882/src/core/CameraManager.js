import * as THREE from 'three'

export class CameraManager {
  constructor(container) {
    const { clientWidth: w, clientHeight: h } = container
    this._camera = new THREE.PerspectiveCamera(60, w / h, 0.1, 1000)
    // Câmera elevada e lateral para enquadrar mesa + zoetrópio + parede
    this._camera.position.set(8, 5, 6)
    this._camera.lookAt(0, 3, -2)
  }

  get() {
    return this._camera
  }

  onResize(width, height) {
    this._camera.aspect = width / height
    this._camera.updateProjectionMatrix()
  }
}
