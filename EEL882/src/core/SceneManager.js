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
     * LUZ AMBIENTE — Tom quente (âmbar) para estética vintage séc. XIX.
     * Intensidade baixa (0.2) para não "lavar" a projeção do SpotLight.
     * Cor 0xffd4a0 = branco-alaranjado (simula iluminação a gás).
     */
    const ambient = new THREE.AmbientLight(0xffd4a0, 0.2)
    this._scene.add(ambient)
  }
}
