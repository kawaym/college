import * as THREE from 'three'

/**
 * ============================================================================
 * ROOM ENVIRONMENT — Sala de projeção vintage (séc. XIX)
 * ============================================================================
 *
 * Cria o cenário ao redor do zoetrópio:
 *   - Mesa de madeira (primitivas: BoxGeometry para tampo + pernas)
 *   - Chão de madeira escura
 *   - Parede de fundo (onde a projeção acontece)
 *   - Iluminação ambiente quente (âmbar)
 *
 * CONCEITOS DE CG:
 *
 * 1. PRIMITIVAS COMO MODELAGEM:
 *    Em vez de importar modelos GLTF, construímos a mesa com BoxGeometry.
 *    Cada Box é definida por largura × altura × profundidade.
 *    A posição de cada peça é calculada relativamente ao tampo.
 *
 * 2. TEXTURA COM RepeatWrapping:
 *    Para simular piso de tábuas, repetimos uma textura N vezes.
 *    texture.wrapS = texture.wrapT = RepeatWrapping
 *    texture.repeat.set(nx, ny) → repete nx vezes em U, ny vezes em V.
 *
 * 3. receiveShadow / castShadow:
 *    O chão recebe sombras (receiveShadow) da mesa e do zoetrópio.
 *    A mesa projeta sombras (castShadow) no chão.
 */

// ============================================================================
// CONSTANTES DA SALA
// ============================================================================

/** Altura do tampo da mesa (em unidades do mundo) */
const TABLE_HEIGHT = 2.5

/** Dimensões do tampo */
const TABLE_WIDTH = 6.0
const TABLE_DEPTH = 4.0
const TABLE_TOP_THICKNESS = 0.12

/** Pernas da mesa */
const LEG_SIZE = 0.15
const LEG_HEIGHT = TABLE_HEIGHT - TABLE_TOP_THICKNESS

/** Chão */
const FLOOR_SIZE = 30

/** Parede */
const WALL_WIDTH = 10
const WALL_HEIGHT = 8
const WALL_Z = -10.0  // mesma posição Z da tela de projeção

// ============================================================================
// CLASSE
// ============================================================================

export class RoomEnvironment {
    constructor(scene) {
        this._scene = scene
        this._buildTable()
        this._buildFloor()
        this._buildWall()
        this._buildRoomLighting()
    }

    /**
     * Retorna a altura Y do topo do tampo da mesa.
     * Usado para posicionar o zoetrópio em cima.
     */
    getTableTopY() {
        return TABLE_HEIGHT
    }

    // ==========================================================================
    // MESA — Construída com primitivas (BoxGeometry)
    // ==========================================================================

    _buildTable() {
        /**
         * A mesa é composta por 5 BoxGeometry:
         *   1 tampo (plano horizontal)
         *   4 pernas (cilindros verticais aproximados por boxes)
         *
         * Material: madeira escura (marrom) com rugosidade alta.
         *
         * POSICIONAMENTO:
         *   O tampo é centrado em (0, TABLE_HEIGHT - thickness/2, 0).
         *   As pernas ficam nos 4 cantos, indo do chão ao tampo.
         *
         *   Perna (i,j) com i,j ∈ {-1, +1}:
         *     x = i × (TABLE_WIDTH/2 - LEG_SIZE/2 - margem)
         *     z = j × (TABLE_DEPTH/2 - LEG_SIZE/2 - margem)
         *     y = LEG_HEIGHT / 2  (centro da perna)
         */

        // Material da madeira escura
        const woodMat = new THREE.MeshStandardMaterial({
            color: 0x3b2314,       // marrom escuro (mogno)
            roughness: 0.75,
            metalness: 0.05,
        })

        // --- TAMPO ---
        const topGeo = new THREE.BoxGeometry(TABLE_WIDTH, TABLE_TOP_THICKNESS, TABLE_DEPTH)
        const top = new THREE.Mesh(topGeo, woodMat)
        top.position.y = TABLE_HEIGHT - TABLE_TOP_THICKNESS / 2
        top.castShadow = true
        top.receiveShadow = true
        this._scene.add(top)

        // --- PERNAS ---
        const legGeo = new THREE.BoxGeometry(LEG_SIZE, LEG_HEIGHT, LEG_SIZE)
        const margin = 0.15  // recuo das pernas em relação à borda

        for (const ix of [-1, 1]) {
            for (const iz of [-1, 1]) {
                const leg = new THREE.Mesh(legGeo, woodMat)
                leg.position.set(
                    ix * (TABLE_WIDTH / 2 - LEG_SIZE / 2 - margin),
                    LEG_HEIGHT / 2,
                    iz * (TABLE_DEPTH / 2 - LEG_SIZE / 2 - margin),
                )
                leg.castShadow = true
                leg.receiveShadow = true
                this._scene.add(leg)
            }
        }

        // --- TRAVESSAS (reforço entre as pernas) ---
        const stretcherMat = woodMat.clone()
        const stretcherThickness = 0.08
        const stretcherHeight = 0.3

        // Travessa frontal e traseira (ao longo de X)
        const stretcherXGeo = new THREE.BoxGeometry(
            TABLE_WIDTH - 2 * (LEG_SIZE + margin),
            stretcherHeight,
            stretcherThickness
        )
        for (const iz of [-1, 1]) {
            const s = new THREE.Mesh(stretcherXGeo, stretcherMat)
            s.position.set(
                0,
                LEG_HEIGHT * 0.3,
                iz * (TABLE_DEPTH / 2 - LEG_SIZE / 2 - margin),
            )
            s.castShadow = true
            this._scene.add(s)
        }

        // Travessas laterais (ao longo de Z)
        const stretcherZGeo = new THREE.BoxGeometry(
            stretcherThickness,
            stretcherHeight,
            TABLE_DEPTH - 2 * (LEG_SIZE + margin)
        )
        for (const ix of [-1, 1]) {
            const s = new THREE.Mesh(stretcherZGeo, stretcherMat)
            s.position.set(
                ix * (TABLE_WIDTH / 2 - LEG_SIZE / 2 - margin),
                LEG_HEIGHT * 0.3,
                0,
            )
            s.castShadow = true
            this._scene.add(s)
        }
    }

    // ==========================================================================
    // CHÃO
    // ==========================================================================

    _buildFloor() {
        /**
         * Plano horizontal no Y=0 (nível do chão).
         *
         * PlaneGeometry cria um plano no plano XY por padrão.
         * Rotacionamos -90° em X para torná-lo horizontal (plano XZ).
         *
         * Rotação: aplicamos Rx(-π/2):
         *   Rx(-90°) × (0,0,1) = (0,1,0)  → normal aponta para cima ✓
         *
         * receiveShadow = true para que a mesa e o zoetrópio projetem sombras.
         */
        const floorMat = new THREE.MeshStandardMaterial({
            color: 0x2a1f14,      // marrom muito escuro (assoalho antigo)
            roughness: 0.85,
            metalness: 0.05,
        })

        const floor = new THREE.Mesh(
            new THREE.PlaneGeometry(FLOOR_SIZE, FLOOR_SIZE),
            floorMat
        )
        floor.rotation.x = -Math.PI / 2
        floor.position.y = 0
        floor.receiveShadow = true
        this._scene.add(floor)
    }

    // ==========================================================================
    // PAREDE DE FUNDO
    // ==========================================================================

    _buildWall() {
        /**
         * Parede vertical atrás da tela de projeção.
         * Maior que a tela (16×10 vs 10×7) para dar contexto visual.
         *
         * A tela de projeção (branca, no ZoetropeAnimation) fica ligeiramente
         * à frente desta parede (z = WALL_Z + 0.01), criando o efeito de uma
         * tela presa na parede.
         *
         * Cor: bege/estuque, evocando paredes de salões do séc. XIX.
         */
        const wallMat = new THREE.MeshStandardMaterial({
            color: 0x8b7d6b,     // bege/estuque envelhecido
            roughness: 0.9,
            metalness: 0.0,
        })

        const wall = new THREE.Mesh(
            new THREE.PlaneGeometry(WALL_WIDTH, WALL_HEIGHT),
            wallMat
        )
        // Parede centrada verticalmente a partir do chão
        wall.position.set(0, WALL_HEIGHT / 2, WALL_Z - 0.05)
        wall.receiveShadow = true
        this._scene.add(wall)
        this._wall = wall
    }

    /**
     * Retorna a mesh da parede de fundo.
     * Usado pelo CinematicEvent para ocultar quando a câmera a atravessa.
     */
    getWall() { return this._wall }

    // ==========================================================================
    // ILUMINAÇÃO DA SALA
    // ==========================================================================

    _buildRoomLighting() {
        /**
         * PointLight quente (âmbar) — simula uma lâmpada a gás/óleo
         * pendurada acima da mesa. Dá profundidade e ambiência vintage.
         *
         * Posição elevada e deslocada para não competir com o SpotLight
         * do projetor (que é a luz principal da projeção).
         *
         * PointLight emite em todas as direções (esfera):
         *   I(d) = intensity / (4π × d²)   quando decay = 2
         *
         * castShadow = true para que a mesa projete sombra no chão.
         */
        const warmLight = new THREE.PointLight(0xffa54f, 30, 25, 2)
        warmLight.position.set(2, 6, 1)
        warmLight.castShadow = true
        warmLight.shadow.mapSize.width = 1024
        warmLight.shadow.mapSize.height = 1024
        this._scene.add(warmLight)
    }
}
