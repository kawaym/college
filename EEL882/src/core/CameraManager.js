import * as THREE from 'three'

export class CameraManager {
  constructor(container) {
    const { clientWidth: w, clientHeight: h } = container
    this._camera = new THREE.PerspectiveCamera(60, w / h, 0.1, 1000)
    // Câmera lateral e elevada para ver o mecanismo completo:
    // - Cilindro/obturador no centro (z=0)
    // - Tela de projeção à frente (z=-5)
    // - Percepção de que a luz sai de dentro do cilindro
    this._camera.position.set(6, 3, 2)
    this._camera.lookAt(0, 0, -2)
  }

  get() {
    return this._camera
  }

  onResize(width, height) {
    this._camera.aspect = width / height
    this._camera.updateProjectionMatrix()
  }
}
