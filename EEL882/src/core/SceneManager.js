import * as THREE from 'three'

export class SceneManager {
  constructor() {
    this._scene = new THREE.Scene()
    this._scene.background = new THREE.Color(0x050505)

    this._addLights()
  }

  get() {
    return this._scene
  }

  _addLights() {
    /**
     * LUZ AMBIENTE — Intensidade mínima (0.15).
     * Serve APENAS para que o cilindro/obturador seja visível (não fique invisível no escuro).
     * A projeção é feita 100% pela SpotLight (definida no ZoetropeAnimation).
     * Se a ambiente fosse forte, "lavaria" a projeção na tela.
     */
    const ambient = new THREE.AmbientLight(0xffffff, 0.15)
    this._scene.add(ambient)
  }
}
