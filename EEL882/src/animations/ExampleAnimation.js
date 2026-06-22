import * as THREE from 'three'

// Demonstra o padrão base: construtor adiciona objetos à cena,
// update() recebe (time, delta) e anima a cada frame.
export class ExampleAnimation {
  constructor(scene) {
    const geo = new THREE.TorusKnotGeometry(1, 0.35, 128, 32)
    const mat = new THREE.MeshStandardMaterial({
      color: 0x6644ff,
      roughness: 0.3,
      metalness: 0.6,
    })
    this._mesh = new THREE.Mesh(geo, mat)
    scene.add(this._mesh)
  }

  update(time, delta) {
    this._mesh.rotation.x += delta * 0.5
    this._mesh.rotation.y += delta * 0.8
  }

  dispose(scene) {
    scene.remove(this._mesh)
    this._mesh.geometry.dispose()
    this._mesh.material.dispose()
  }
}
