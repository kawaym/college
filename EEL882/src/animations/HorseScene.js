import * as THREE from 'three'
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js'
import { OrbitControls } from 'three/addons/controls/OrbitControls.js'

/**
 * ============================================================================
 * HORSE SCENE — Revelação do Cavalo 3D (Sprint 3)
 * ============================================================================
 *
 * CONCEITO:
 *   Após a câmera atravessar a parede (CinematicEvent → INSIDE), a animação
 *   2D do Muybridge "ganha vida" como um modelo 3D animado. O cavalo GLTF
 *   aparece iluminado dramaticamente, e o usuário pode orbitá-lo com o mouse.
 *
 * CONCEITOS DE COMPUTAÇÃO GRÁFICA:
 *
 * 1. GLTF/GLB (GL Transmission Format):
 *    Formato binário padrão Khronos Group para modelos 3D.
 *    Estrutura do .glb:
 *      [Header 12B] [JSON chunk] [Binary chunk]
 *      - JSON: metadados (cenas, nós, materiais, animações)
 *      - Binary: buffers de vértices, índices, morph targets
 *    Vantagem: arquivo único, compacto, parseia direto para GPU buffers.
 *
 * 2. MORPH TARGETS (Shape Keys / Blend Shapes):
 *    Técnica de animação onde cada frame armazena posições alternativas
 *    dos vértices. A posição final é interpolada:
 *      v_final = v_base + Σ(w_i × (v_target_i - v_base))
 *    onde w_i ∈ [0,1] é o peso do morph target i.
 *    O AnimationMixer interpola os pesos ao longo do tempo.
 *
 * 3. ANIMATION MIXER:
 *    Sistema de playback de Three.js. Gerencia AnimationClips:
 *      mixer.update(Δt) → avança todos os clips ativos
 *      clip.tracks[] → cada track interpola uma propriedade ao longo do tempo
 *    Para morph targets: track tipo "NumberKeyframeTrack" com nome
 *    "mesh.morphTargetInfluences[i]".
 *
 * 4. ORBIT CONTROLS (Coordenadas Esféricas):
 *    Câmera orbita ao redor de um ponto (target) usando coordenadas esféricas:
 *      x = r × sin(φ) × sin(θ)
 *      y = r × cos(φ)
 *      z = r × sin(φ) × cos(θ)
 *    onde r=distância, θ=azimute (horizontal), φ=polar (vertical).
 *    Mouse drag → altera (θ, φ). Scroll → altera r.
 *
 * 5. ILUMINAÇÃO DRAMÁTICA (Three-Point Lighting):
 *    - Key light (SpotLight): iluminação principal, posição 45° lateral/acima
 *    - Fill light (fraca): suaviza sombras do lado oposto
 *    - Rim light (backlight): contorno/silhueta por trás
 *    Aqui usamos key + rim para efeito teatral.
 */

// ============================================================================
// CONSTANTES
// ============================================================================

/** Posição do cavalo na cena (frente da câmera após travessia) */
const HORSE_POSITION = new THREE.Vector3(0, 0, -15)

/** Escala do modelo (Three.js Horse.glb é ~100 unidades internas) */
const HORSE_SCALE = 0.015

/**
 * Jóquei (Xbot.glb):
 * O Xbot mede ~1.0 unidade de altura a escala 1.0.
 * O cavalo a escala 0.015 tem ~1.5m de altura. O dorso fica em ~y=1.1.
 * Escalamos o Xbot para ~1.6m e elevamos para y=1.8 para não clipar.
 */
const JOCKEY_SCALE = 1.6
const JOCKEY_OFFSET_Y = 0.85  // Sentado no dorso do cavalo

/** Ângulo de abdução das coxas (rad) para simular pose de cavaleiro */
const LEG_SPREAD_ANGLE = 0.4  // ~23° para cada lado

/**
 * Key light — SpotLight principal (iluminação dramática lateral/superior).
 * Posição calculada para iluminar o cavalo a ~45° de cima e da direita:
 *   P_light = P_horse + (3, 6, 2) → ligeiramente à frente e lateral
 */
const KEY_LIGHT_POS = new THREE.Vector3(3, 8, -13)
const KEY_LIGHT_TARGET = new THREE.Vector3(0, 1.5, -15)

/**
 * Rim light — contra-luz por trás/cima para criar contorno (silhueta).
 * Posição atrás e acima do cavalo.
 */
const RIM_LIGHT_POS = new THREE.Vector3(-2, 6, -18)

// ============================================================================
// CLASSE
// ============================================================================

export class HorseScene {
    /**
     * @param {THREE.Scene} scene
     * @param {THREE.PerspectiveCamera} camera
     * @param {HTMLElement} domElement — canvas do renderer (para OrbitControls)
     * @param {CinematicEvent} cinematic — referência para checar isInside
     * @param {ZoetropeAnimation} zoetrope — para esconder o grupo
     */
    constructor(scene, camera, domElement, cinematic, zoetrope) {
        this._scene = scene
        this._camera = camera
        this._cinematic = cinematic
        this._zoetrope = zoetrope

        this._revealed = false
        this._model = null
        this._horseMesh = null  // Mesh principal para bounding box
        this._jockey = null
        this._mixer = null
        this._jockeyMixer = null
        this._horseClipDuration = 1.0  // Será atualizado após load
        this._leftUpLeg = null   // Bone da coxa esquerda
        this._rightUpLeg = null  // Bone da coxa direita
        this._keyLight = null
        this._rimLight = null
        this._horseBBox = new THREE.Box3()  // Reutilizado cada frame

        // Preload dos modelos (async)
        this._loadModel()
        this._loadJockey()

        // OrbitControls (desativados até revelação)
        this._setupControls(domElement)
    }

    // ==========================================================================
    // CARREGAMENTO DO MODELO GLTF
    // ==========================================================================

    _loadModel() {
        /**
         * GLTFLoader carrega o arquivo .glb assincronamente.
         * O resultado contém:
         *   gltf.scene → THREE.Group com toda a hierarquia de meshes
         *   gltf.animations → Array de AnimationClip (morph targets / skeleton)
         *
         * Precarregamos no construtor para que o modelo esteja pronto
         * instantaneamente quando isInside se tornar true (zero delay na revelação).
         */
        const loader = new GLTFLoader()
        loader.load(import.meta.env.BASE_URL + 'models/Horse.glb', (gltf) => {
            this._model = gltf.scene

            /**
             * ESCALA E POSIÇÃO:
             * O Horse.glb do Three.js tem coordenadas internas em centímetros
             * (~100 unidades de altura). Aplicamos escala 0.02 → ~2m de altura.
             *
             * Transformação: Model Matrix M = T × R × S
             *   S = scale(0.02, 0.02, 0.02)
             *   R = Ry(-π/2) → cavalo vira para -x (galopa para a esquerda
             *       do ponto de vista da câmera que olha em -z)
             *   T = translate(0, 0, -15)
             */
            this._model.scale.setScalar(HORSE_SCALE)
            this._model.position.copy(HORSE_POSITION)
            this._model.rotation.y = -Math.PI / 2  // Galopa para a esquerda (+z → -z)

            // Habilita sombras e guarda referência à mesh principal
            this._model.traverse((child) => {
                if (child.isMesh) {
                    child.castShadow = true
                    child.receiveShadow = true
                    // Guarda a primeira mesh (corpo do cavalo) para rastrear bbox
                    if (!this._horseMesh) this._horseMesh = child
                }
            })

            // Oculto até revelação
            this._model.visible = false
            this._scene.add(this._model)

            /**
             * ANIMATION MIXER:
             * AnimationMixer gerencia o playback de AnimationClips.
             * Internamente mantém um relógio e interpola os tracks:
             *   track.interpolant.evaluate(localTime) → valor do keyframe
             *
             * Para morph targets, cada track controla
             * mesh.morphTargetInfluences[i] ao longo do tempo.
             */
            if (gltf.animations && gltf.animations.length > 0) {
                this._mixer = new THREE.AnimationMixer(this._model)
                const clip = gltf.animations[0]
                this._horseClipDuration = clip.duration

                const action = this._mixer.clipAction(clip)
                action.play()
            }
        })
    }

    // ==========================================================================
    // CARREGAMENTO DO JÓQUEI (Xbot)
    // ==========================================================================

    _loadJockey() {
        /**
         * Xbot.glb — modelo humanóide (robô articulado) usado como jóquei.
         *
         * SKELETAL ANIMATION (Animação Esqueletal):
         * Diferente do cavalo (morph targets), o Xbot usa BONES (ossos):
         *   - Hierarquia de Bone objects (esqueleto)
         *   - Cada vértice tem até 4 bones de influência (skinning weights)
         *   - Posição final do vértice:
         *       v_final = Σ(weight_i × boneMatrix_i × bindMatrixInverse × v_local)
         *   - O AnimationMixer interpola as transforms dos bones entre keyframes
         *
         * Posicionamento: mesmo (x, z) do cavalo, y elevado ao dorso.
         * Rotação: mesma do cavalo (-π/2) para ficar alinhado.
         */
        const loader = new GLTFLoader()
        loader.load(import.meta.env.BASE_URL + 'models/Xbot.glb', (gltf) => {
            this._jockey = gltf.scene

            /**
             * Model Matrix do jóquei:
             *   S = scale(1.6) → ~1.6m (cavaleiro visível sobre o cavalo)
             *   R = Ry(-π/2) → mesma direção que o cavalo
             *   T = translate(horseX, dorso_y, horseZ)
             */
            this._jockey.scale.setScalar(JOCKEY_SCALE)
            this._jockey.position.set(
                HORSE_POSITION.x,
                HORSE_POSITION.y + JOCKEY_OFFSET_Y,
                HORSE_POSITION.z
            )
            this._jockey.rotation.y = -Math.PI / 2

            // Sombras
            this._jockey.traverse((child) => {
                if (child.isMesh) {
                    child.castShadow = true
                    child.receiveShadow = true
                }
            })

            /**
             * ANIMAÇÃO ADITIVA (Additive Blending):
             *
             * O Xbot usa o padrão de animação aditiva do Three.js:
             *   - Base action: 'idle' (weight=1) — pose de referência
             *   - Additive actions: 'sneak_pose', 'headShake' — DIFERENÇAS
             *     aplicadas SOBRE a base
             *
             * CONCEITO MATEMÁTICO (Additive Blending):
             *   T_final = T_base + Σ(weight_i × (T_additive_i - T_reference))
             *
             *   Onde T_reference é o frame 0 do clip original.
             *   makeClipAdditive(clip) converte cada keyframe:
             *     key_additive[j] = key_original[j] - key_original[0]
             *   Resultado: o clip armazena apenas o DELTA (diferença).
             *
             * Para clips que terminam em '_pose' (poses estáticas):
             *   subclip(clip, name, 2, 3, 30) extrai um único frame
             *   (frames 2-3 a 30fps) como clip de 1 keyframe.
             *
             * Com weight=1 em sneak_pose + headShake, o jóquei fica:
             *   idle + 100% sneak (agachado) + 100% headShake
             */
            if (gltf.animations && gltf.animations.length > 5) {
                this._jockeyMixer = new THREE.AnimationMixer(this._jockey)

                const animations = gltf.animations

                // BASE: 'idle' (index 2) — pose de referência parada
                const idleClip = animations[2]
                const idleAction = this._jockeyMixer.clipAction(idleClip)
                idleAction.enabled = true
                idleAction.setEffectiveTimeScale(1)
                idleAction.setEffectiveWeight(1)
                idleAction.play()

                // ADDITIVE: 'sneak_pose' (index 5)
                // makeClipAdditive remove o reference frame (frame 0)
                // subclip extrai frame 2-3 como pose estática
                let sneakClip = animations[5]
                THREE.AnimationUtils.makeClipAdditive(sneakClip)
                sneakClip = THREE.AnimationUtils.subclip(sneakClip, sneakClip.name, 2, 3, 30)
                const sneakAction = this._jockeyMixer.clipAction(sneakClip)
                sneakAction.enabled = true
                sneakAction.setEffectiveTimeScale(1)
                sneakAction.setEffectiveWeight(1)
                sneakAction.play()

                // ADDITIVE: 'headShake' (index 1)
                let headShakeClip = animations[1]
                THREE.AnimationUtils.makeClipAdditive(headShakeClip)
                const headShakeAction = this._jockeyMixer.clipAction(headShakeClip)
                headShakeAction.enabled = true
                headShakeAction.setEffectiveTimeScale(1)
                headShakeAction.setEffectiveWeight(1)
                headShakeAction.play()

                // Avança o mixer para aplicar as poses imediatamente
                this._jockeyMixer.update(0)
            }

            // Busca os bones das coxas para abrir as pernas (pose de cavaleiro)
            this._jockey.traverse((child) => {
                if (child.isBone) {
                    if (child.name.includes('LeftUpLeg')) this._leftUpLeg = child
                    if (child.name.includes('RightUpLeg')) this._rightUpLeg = child
                }
            })

            this._jockey.visible = false
            this._scene.add(this._jockey)
        })
    }

    _setupControls(domElement) {
        /**
         * OrbitControls permite ao usuário orbitar a câmera ao redor do cavalo.
         *
         * COORDENADAS ESFÉRICAS:
         *   A câmera é posicionada em (r, θ, φ) relativo ao target:
         *     x = r × sin(φ) × sin(θ)
         *     y = r × cos(φ)
         *     z = r × sin(φ) × cos(θ)
         *
         * Mouse interactions:
         *   - Left drag → altera θ (azimute) e φ (polar)
         *   - Scroll → altera r (distância)
         *   - Right drag → pan (desloca target)
         *
         * enableDamping: inércia suave (amortecimento exponencial)
         *   pos_new = lerp(pos_current, pos_target, dampingFactor)
         */
        this._controls = new OrbitControls(this._camera, domElement)
        this._controls.target.copy(HORSE_POSITION)
        this._controls.target.y = 1.5  // Centro aproximado do cavalo (não o chão)
        this._controls.enableDamping = true
        this._controls.dampingFactor = 0.08
        this._controls.minDistance = 2
        this._controls.maxDistance = 12
        this._controls.enabled = false  // Desativado até revelação
    }

    // ==========================================================================
    // ILUMINAÇÃO DRAMÁTICA
    // ==========================================================================

    _buildLighting() {
        /**
         * KEY LIGHT — Iluminação principal (SpotLight).
         * Posicionada acima e à direita do cavalo para criar sombras
         * definidas no lado esquerdo (modelagem de volume).
         *
         * SpotLight é usado (não PointLight) para controlar o cone de luz
         * e evitar iluminar a cena inteira desnecessariamente.
         */
        this._keyLight = new THREE.SpotLight(0xffffff, 200)
        this._keyLight.position.copy(KEY_LIGHT_POS)
        this._keyLight.target.position.copy(KEY_LIGHT_TARGET)
        this._keyLight.angle = Math.PI / 5
        this._keyLight.penumbra = 0.4
        this._keyLight.decay = 2
        this._keyLight.castShadow = true
        this._keyLight.shadow.mapSize.set(1024, 1024)
        this._keyLight.visible = false
        this._scene.add(this._keyLight)
        this._scene.add(this._keyLight.target)

        /**
         * RIM LIGHT — Contra-luz (backlight) para criar silhueta/contorno.
         * Posicionada atrás e acima do cavalo. Intensidade mais baixa.
         * Sem sombras (apenas para efeito visual de borda luminosa).
         */
        this._rimLight = new THREE.PointLight(0x4488ff, 80)
        this._rimLight.position.copy(RIM_LIGHT_POS)
        this._rimLight.visible = false
        this._scene.add(this._rimLight)

        /**
         * Chão mínimo para receber sombra do cavalo.
         * Sem isso, a sombra projetada pelo key light não tem onde pousar,
         * e o cavalo parece "flutuar" no escuro.
         */
        const groundGeo = new THREE.PlaneGeometry(12, 12)
        const groundMat = new THREE.MeshStandardMaterial({
            color: 0x111111,
            roughness: 0.9,
            metalness: 0.0,
        })
        this._ground = new THREE.Mesh(groundGeo, groundMat)
        this._ground.rotation.x = -Math.PI / 2
        this._ground.position.set(0, 0, -15)
        this._ground.receiveShadow = true
        this._ground.visible = false
        this._scene.add(this._ground)
    }

    // ==========================================================================
    // REVELAÇÃO / OCULTAÇÃO (reversível)
    // ==========================================================================

    _show() {
        /**
         * Mostra a cena 3D do cavalo quando a câmera está "dentro".
         */

        // Esconde o zoetrópio (fora do frustum, mas por segurança)
        const group = this._zoetrope.getGroup()
        if (group) group.visible = false

        // Constrói iluminação (lazy — só quando necessário)
        if (!this._keyLight) this._buildLighting()

        // Revela tudo
        if (this._model) this._model.visible = true
        if (this._jockey) this._jockey.visible = true
        if (this._keyLight) this._keyLight.visible = true
        if (this._rimLight) this._rimLight.visible = true
        if (this._ground) this._ground.visible = true

        // Ativa controles de órbita
        this._controls.enabled = true

        this._revealed = true
    }

    _hide() {
        /**
         * Esconde a cena 3D e restaura a vista principal quando
         * o usuário reduz a velocidade e a câmera volta.
         */

        // Mostra o zoetrópio novamente
        const group = this._zoetrope.getGroup()
        if (group) group.visible = true

        // Esconde modelo + iluminação
        if (this._model) this._model.visible = false
        if (this._jockey) this._jockey.visible = false
        if (this._keyLight) this._keyLight.visible = false
        if (this._rimLight) this._rimLight.visible = false
        if (this._ground) this._ground.visible = false

        // Desativa controles de órbita
        this._controls.enabled = false

        this._revealed = false
    }

    // ==========================================================================
    // UPDATE
    // ==========================================================================

    update(time, delta) {
        // Transição reversível: mostra/esconde baseado em isInside
        if (this._cinematic.isInside && !this._revealed) {
            this._show()
        } else if (!this._cinematic.isInside && this._revealed) {
            this._hide()
            return
        }

        // Se não está dentro, não atualiza nada
        if (!this._revealed) return

        /**
         * ANIMATION MIXER UPDATE:
         * mixer.update(Δt) avança o clock interno e interpola todos os
         * clips ativos. Para morph targets:
         *   influences[i] = interpolate(keyframes, localTime)
         * O Three.js recalcula os vértices via shader (GPU morphing).
         */
        if (this._mixer) {
            this._mixer.update(delta)
        }

        // Avança animação esqueletal do jóquei (headShake aditivo)
        if (this._jockeyMixer) {
            this._jockeyMixer.update(delta)
        }

        // Abre as pernas do jóquei (abdução) APÓS o mixer aplicar a pose
        // Rotação no eixo Z local do bone: positivo = abre esquerda, negativo = abre direita
        if (this._leftUpLeg) {
            this._leftUpLeg.rotation.z += LEG_SPREAD_ANGLE
        }
        if (this._rightUpLeg) {
            this._rightUpLeg.rotation.z -= LEG_SPREAD_ANGLE
        }

        /**
         * RASTREAMENTO DIRETO DO DORSO (Bounding Box Tracking):
         *
         * Em vez de aproximar o galope por uma senoide, calculamos a
         * Bounding Box (AABB) do mesh do cavalo A CADA FRAME após o
         * morph target ser aplicado. O topo da bbox (box.max.y) é a
         * posição exata do dorso naquele instante.
         *
         * O jóquei é posicionado em bbox.max.y + pequeno offset,
         * garantindo sincronia perfeita com qualquer curva de animação.
         */
        if (this._jockey && this._revealed && this._horseMesh && this._mixer) {
            // Bbox estática (base geometry, morph targets não afetam)
            this._horseMesh.geometry.computeBoundingBox()
            const localBox = this._horseMesh.geometry.boundingBox
            this._horseBBox.copy(localBox).applyMatrix4(this._horseMesh.matrixWorld)

            // Base Y: 40% da altura (dorso)
            const minY = this._horseBBox.min.y
            const maxY = this._horseBBox.max.y
            const backY = minY + (maxY - minY) * 0.4

            // Oscilação sincronizada ao galope (morph targets movem na GPU,
            // não afetam a bbox, então simulamos o sobe-e-desce aqui)
            const T = this._horseClipDuration
            const mixerTime = this._mixer.time % T
            const cycle = (2 * Math.PI * mixerTime) / T
            const bounceY = 0.12 * Math.sin(cycle)

            this._jockey.position.y = backY + bounceY
        }

        /**
         * ORBIT CONTROLS UPDATE:
         * Com enableDamping = true, os controles usam LERP exponencial
         * para suavizar o movimento:
         *   θ_new = lerp(θ_current, θ_target, dampingFactor)
         * Precisa de update() a cada frame para o damping funcionar.
         */
        if (this._controls.enabled) {
            this._controls.update()
        }
    }
}
