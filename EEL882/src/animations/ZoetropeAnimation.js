import * as THREE from 'three'

/**
 * ============================================================================
 * ZOETROPE ANIMATION — Projetor Cilíndrico (Luz interna projeta para fora)
 * ============================================================================
 *
 * CONCEITO FÍSICO (Lanterna Mágica / Projetor de Slides Rotativo):
 *
 *   1. Uma lâmpada (SpotLight) fica NO CENTRO do cilindro.
 *   2. O cilindro tem as imagens do Muybridge como transparências na superfície.
 *      Visualmente, você VÊ as imagens coladas no cilindro (CylinderGeometry + textura).
 *   3. A luz de dentro projeta o frame atual na parede/tela à frente.
 *      Implementado via SpotLight.map com CanvasTexture que mostra 1 frame por vez.
 *   4. A própria imagem do Muybridge já possui separadores pretos entre frames,
 *      que agem como obturador natural (bloqueiam a projeção entre quadros).
 *   5. Conforme o cilindro gira, o frame projetado muda (próximo frame da sequência).
 *
 * CONCEITOS DE COMPUTAÇÃO GRÁFICA:
 *
 * 1. SPOTLIGHT.MAP (Projeção de Textura):
 *    SpotLight.map modula a intensidade da luz com uma textura projetada:
 *      lightCoord = shadowMatrix × fragWorldPos
 *      uv = lightCoord.xy / lightCoord.w  (divisão perspectiva)
 *      cor_projetada = texture2D(map, uv)
 *    Usamos CanvasTexture para controlar QUAL frame é projetado a cada instante.
 *
 * 2. CANVAS TEXTURE (Textura Dinâmica):
 *    Uma textura criada a partir de um <canvas> HTML. Podemos redesenhar o canvas
 *    a cada frame usando ctx.drawImage() com source crop para extrair 1 frame
 *    da strip. Ao setar texture.needsUpdate = true, a GPU re-carrega a textura.
 *
 * 3. UV MAPPING CILÍNDRICO (Filme no cilindro):
 *    CylinderGeometry gera UVs: u = θ/(2π), v = y/h.
 *    A strip horizontal do Muybridge é "enrolada" no cilindro automaticamente.
 *    Cada frame ocupa 1/N da circunferência.
 *
 * 4. GRAFO DE CENA:
 *    _group contém cilindro visual → rotaciona junto.
 *    SpotLight e tela ficam fixas.
 *
 * 5. DELTA TIME:
 *    θ_new = θ_old + ω × Δt → rotação independente do frame rate.
 */

// ============================================================================
// CONSTANTES
// ============================================================================

const CYLINDER_RADIUS = 2.0
const HEIGHT = 2.5
const NUM_FRAMES = 12       // Frames na sequência do Muybridge (clássica)
const MAX_SPEED = 15.0      // rad/s
const ACCELERATION = 3.0    // rad/s²
const WALL_DISTANCE = 8.0   // Distância da tela

// ============================================================================
// CLASSE PRINCIPAL
// ============================================================================

export class ZoetropeAnimation {
    constructor(scene) {
        this._scene = scene

        /**
         * GRAFO DE CENA:
         *   scene
         *     ├─ _group (ROTACIONA)
         *     │    ├─ cilindro visual (mostra o filme)
         *     │    └─ base / topo
         *     ├─ _spotLight (FIXO no centro) ← projeta CanvasTexture
         *     └─ _screen (FIXO) ← tela branca
         */
        this._group = new THREE.Group()
        scene.add(this._group)

        this._speed = 0
        this._targetSpeed = 0
        this._controlsEnabled = true

        // Carrega a imagem e depois constrói tudo
        this._loadImageAndBuild()
        this._setupControls()
    }

    // ==========================================================================
    // CARREGAMENTO DA IMAGEM + INICIALIZAÇÃO
    // ==========================================================================

    _loadImageAndBuild() {
        /**
         * Carregamos a strip do Muybridge como um HTMLImageElement.
         * Depois de carregada, extraímos frames individuais via Canvas.
         * Isso nos dá controle total sobre qual frame é projetado.
         */
        const img = new Image()
        img.src = '/textures/muybridge_classic.jpg'
        img.onload = () => {
            this._stripImage = img
            this._frameWidth = Math.floor(img.width / NUM_FRAMES)
            this._frameHeight = img.height

            this._buildFilmCylinder()
            this._buildBase()
            this._buildProjectorLight()
            this._buildScreen()
        }
    }

    // ==========================================================================
    // CILINDRO VISUAL — Mostra o filme (imagens) na superfície
    // ==========================================================================

    _buildFilmCylinder() {
        /**
         * CylinderGeometry com a textura do Muybridge visível na superfície.
         * Isso cria a aparência de um cilindro com "slides" colados nele.
         *
         * UV MAPPING CILÍNDRICO (automático do Three.js):
         *   u = θ / (2π) → percorre a textura horizontalmente ao redor
         *   v = y / height → percorre verticalmente
         *
         * A strip tem 12 frames lado a lado. Ao enrolar no cilindro,
         * cada frame ocupa 1/12 (30°) da circunferência.
         *
         * side: FrontSide → visível de fora (a câmera vê o filme no cilindro)
         * Usamos emissive para que o filme pareça "iluminado por dentro"
         * (como transparências com luz atrás).
         */
        const geometry = new THREE.CylinderGeometry(
            CYLINDER_RADIUS,  // radiusTop
            CYLINDER_RADIUS,  // radiusBottom
            HEIGHT,           // height
            64,               // radialSegments (suavidade)
            1,                // heightSegments
            true              // openEnded (sem tampas)
        )

        const texture = new THREE.TextureLoader().load('/textures/muybridge_classic.jpg')
        texture.colorSpace = THREE.SRGBColorSpace
        texture.wrapS = THREE.RepeatWrapping

        /**
         * Material com emissiveMap:
         * O filme parece "retroiluminado" — como transparências num projetor.
         * emissiveIntensity controla o brilho (simula luz passando por trás).
         * O material standard com emissive ignora sombras no emissive channel,
         * criando o efeito de slides luminosos.
         */
        const material = new THREE.MeshStandardMaterial({
            map: texture,
            emissive: 0xffffff,
            emissiveMap: texture,
            emissiveIntensity: 0.4,
            side: THREE.FrontSide,
            transparent: true,
            opacity: 0.85,
            roughness: 0.3,
            metalness: 0.0,
        })

        const cylinder = new THREE.Mesh(geometry, material)
        this._filmCylinder = cylinder
        this._group.add(cylinder)
    }

    // ==========================================================================
    // BASE
    // ==========================================================================

    _buildBase() {
        const baseMat = new THREE.MeshStandardMaterial({
            color: 0x1a1a1a, roughness: 0.9, metalness: 0.3,
        })

        const base = new THREE.Mesh(
            new THREE.CircleGeometry(CYLINDER_RADIUS + 0.3, 32), baseMat
        )
        base.rotation.x = -Math.PI / 2
        base.position.y = -HEIGHT / 2
        this._group.add(base)

        const top = new THREE.Mesh(
            new THREE.RingGeometry(CYLINDER_RADIUS * 0.2, CYLINDER_RADIUS + 0.3, 32),
            baseMat.clone()
        )
        top.rotation.x = -Math.PI / 2
        top.position.y = HEIGHT / 2
        top.castShadow = true
        this._group.add(top)
    }

    // ==========================================================================
    // SPOTLIGHT + CANVAS TEXTURE — Projetor com frame dinâmico
    // ==========================================================================

    _buildProjectorLight() {
        /**
         * CANVAS TEXTURE — Textura dinâmica que muda a cada frame
         *
         * Problema: SpotLight.map no WebGL NÃO respeita texture.offset.
         * Os UVs de projeção são computados via shadowMatrix, ignorando transform UV.
         *
         * Solução: Usamos um <canvas> como textura. A cada frame da animação,
         * desenhamos APENAS o frame atual do Muybridge no canvas via drawImage()
         * com source crop (recorte de região):
         *   ctx.drawImage(strip, sx, sy, sw, sh, dx, dy, dw, dh)
         *
         * Depois setamos texture.needsUpdate = true para a GPU re-upload.
         * Resultado: a projeção mostra frame por frame conforme o cilindro gira.
         *
         * CONCEITO: CanvasTexture é uma forma de "render-to-texture" simplificada.
         * Em vez de shaders, usamos a API Canvas 2D para manipular pixels.
         */
        const canvasSize = 512
        this._projCanvas = document.createElement('canvas')
        this._projCanvas.width = canvasSize
        this._projCanvas.height = canvasSize
        this._projCtx = this._projCanvas.getContext('2d')

        // Desenha a projeção inicial (ângulo 0)
        this._drawProjection()

        const canvasTexture = new THREE.CanvasTexture(this._projCanvas)
        canvasTexture.colorSpace = THREE.SRGBColorSpace
        this._canvasTexture = canvasTexture

        /**
         * SpotLight — Lâmpada do projetor, posicionada no CENTRO do cilindro.
         *
         * - position (0, 0, 0): dentro do cilindro
         * - target (0, 0, -WALL_DISTANCE): aponta para a tela
         * - angle: π/5 (~36°): cone que cobre a tela
         * - map = canvasTexture: projeta o frame atual
         * - castShadow = true: obrigatório para .map funcionar + frestas projetarem sombra
         */
        const spotLight = new THREE.SpotLight(0xffffff, 200)
        spotLight.position.set(0, 0, 0)
        spotLight.angle = Math.PI / 6
        spotLight.penumbra = 0.1
        spotLight.decay = 2
        spotLight.distance = 0

        spotLight.map = canvasTexture
        spotLight.castShadow = true
        spotLight.shadow.mapSize.width = 2048
        spotLight.shadow.mapSize.height = 2048
        spotLight.shadow.camera.near = 0.5
        spotLight.shadow.camera.far = WALL_DISTANCE + 2
        spotLight.shadow.bias = -0.003

        spotLight.target.position.set(0, 0, -WALL_DISTANCE)
        this._scene.add(spotLight.target)
        this._scene.add(spotLight)
        this._spotLight = spotLight
    }

    // ==========================================================================
    // TELA DE PROJEÇÃO
    // ==========================================================================

    _buildScreen() {
        /**
         * Parede branca recebe a projeção. receiveShadow = true para
         * que as sombras das frestas (cortes entre frames) apareçam.
         */
        const screen = new THREE.Mesh(
            new THREE.PlaneGeometry(10, 7),
            new THREE.MeshStandardMaterial({
                color: 0xffffff, roughness: 0.4, metalness: 0.0,
            })
        )
        screen.position.set(0, 0, -WALL_DISTANCE)
        screen.receiveShadow = true
        this._scene.add(screen)
        this._screen = screen
    }

    // ==========================================================================
    // PROJEÇÃO CONTÍNUA — Strip scrolls com a rotação do cilindro
    // ==========================================================================

    /**
     * Desenha no canvas de projeção a porção da strip que está DIANTE da luz.
     *
     * CONCEITO — Scroll Contínuo (acompanha rotação do cilindro):
     *
     *   O filme está "enrolado" no cilindro. A strip completa representa 2π rad.
     *   Quando o cilindro gira θ rad, a porção do filme diante da luz desloca-se:
     *
     *     offset_pixels = (θ / 2π) × stripWidth
     *
     *   Desenhamos no canvas uma "janela" de largura = 1 frame (= 1/N da strip),
     *   centrada nesse offset. Isso faz a projeção na tela acompanhar o movimento
     *   das frestas, pois ambos (frestas + film) estão solidários no cilindro.
     *
     *   Wrapping: a strip é circular (começo cola no fim). Se a janela cruza a
     *   borda da imagem, desenhamos em duas partes (drawImage × 2).
     *
     * POR QUE FUNCIONA COMO ZOETRÓPIO:
     *   - A imagem projetada desloca-se junto com o cilindro (mesmo referencial).
     *   - As frestas (castShadow) bloqueiam a projeção entre frames.
     *   - Resultado: cada fresta "enquadra" exatamente o frame entre ela e a próxima.
     *   - O observador vê frames discretos alternados (persistência retiniana).
     */
    _drawProjection() {
        if (!this._stripImage) return

        const ctx = this._projCtx
        const canvas = this._projCanvas
        const rotation = this._group.rotation.y

        // Limpa (preto = silêncio de projeção)
        ctx.fillStyle = '#000000'
        ctx.fillRect(0, 0, canvas.width, canvas.height)

        const stripWidth = this._stripImage.width

        /**
         * CORREÇÃO DE FASE E DIREÇÃO:
         *
         * O UV mapping do CylinderGeometry do Three.js gera vértices assim:
         *   x = R × sin(φ),  z = R × cos(φ),  φ ∈ [0, 2π)
         *   u = φ / (2π)
         *
         * Portanto:
         *   φ=0   → (+z)  → u=0.0  (frame 0)
         *   φ=π/2 → (+x)  → u=0.25 (frame 3)
         *   φ=π   → (-z)  → u=0.5  (frame 6)   ← FACE VOLTADA PARA A TELA
         *   φ=3π/2→ (-x)  → u=0.75 (frame 9)
         *
         * A tela está em z = -WALL_DISTANCE. A face do cilindro voltada para
         * a tela (direção -z) tem φ = π → u = 0.5 → METADE da strip.
         *
         * Quando group.rotation.y aumenta (giro CCW visto de cima), o vértice
         * local em φ fica em φ + rotation no mundo. Para a face -z continuar
         * alinhada, o φ local que aponta para -z é:
         *   φ_face = π - rotation
         *
         * Logo o offset correto na strip é:
         *   θ_corrigido = π + rotation
         *   offset = (θ_corrigido / 2π) × stripWidth
         *
         * O sinal positivo mantém a direção do scroll coerente com a rotação
         * visual do cilindro: ambos avançam no mesmo sentido.
         */
        const twoPi = 2 * Math.PI
        const correctedAngle = Math.PI + rotation
        const normalizedAngle = ((correctedAngle % twoPi) + twoPi) % twoPi

        /**
         * Offset em pixels na strip:
         *   A strip completa (stripWidth px) mapeia 2π rad.
         *   offset = (θ_norm / 2π) × stripWidth
         */
        const offset = (normalizedAngle / twoPi) * stripWidth

        /**
         * Janela visível: centramos a janela de 1 frameWidth no offset.
         * Isso corresponde ao arco do cilindro iluminado pelo cone do SpotLight.
         *
         *   sx_start = offset - frameWidth/2
         *
         * Aplicamos módulo circular para manter sx em [0, stripWidth).
         */
        const visibleWidth = this._frameWidth
        let sx = offset - visibleWidth / 2
        sx = ((sx % stripWidth) + stripWidth) % stripWidth

        // Pixels disponíveis antes do fim da strip (para detecção de wrap)
        const remaining = stripWidth - sx

        if (remaining >= visibleWidth) {
            /**
             * Caso simples (sem wrap): toda a janela cabe numa leitura.
             * drawImage(src, sx, sy, sw, sh, dx, dy, dw, dh)
             */
            ctx.drawImage(
                this._stripImage,
                sx, 0, visibleWidth, this._frameHeight,
                0, 0, canvas.width, canvas.height
            )
        } else {
            /**
             * Caso de wrapping: a janela cruza a borda da strip.
             *   Parte 1: do offset até o fim da strip (remaining px)
             *   Parte 2: do início da strip (visibleWidth - remaining px)
             *
             * No canvas, dividimos proporcionalmente:
             *   canvasFirst = (remaining / visibleWidth) × canvasWidth
             */
            const firstPartWidth = remaining
            const secondPartWidth = visibleWidth - firstPartWidth
            const canvasFirst = (firstPartWidth / visibleWidth) * canvas.width

            // Parte 1: fim da strip
            ctx.drawImage(
                this._stripImage,
                sx, 0, firstPartWidth, this._frameHeight,
                0, 0, canvasFirst, canvas.height
            )
            // Parte 2: início da strip (wrap circular)
            ctx.drawImage(
                this._stripImage,
                0, 0, secondPartWidth, this._frameHeight,
                canvasFirst, 0, canvas.width - canvasFirst, canvas.height
            )
        }
    }

    // ==========================================================================
    // CONTROLES
    // ==========================================================================

    _setupControls() {
        this._keys = { up: false, down: false }

        this._onKeyDown = (e) => {
            if (!this._controlsEnabled) return
            if (e.code === 'ArrowUp') this._keys.up = true
            if (e.code === 'ArrowDown') this._keys.down = true
        }
        this._onKeyUp = (e) => {
            if (e.code === 'ArrowUp') this._keys.up = false
            if (e.code === 'ArrowDown') this._keys.down = false
        }

        window.addEventListener('keydown', this._onKeyDown)
        window.addEventListener('keyup', this._onKeyUp)
    }

    // ==========================================================================
    // UPDATE — Rotação + avanço de frame projetado
    // ==========================================================================

    update(time, delta) {
        // Aceleração: v = v₀ + a·Δt
        if (this._keys.up) {
            this._targetSpeed = Math.min(this._targetSpeed + ACCELERATION * delta, MAX_SPEED)
        }
        if (this._keys.down) {
            this._targetSpeed = Math.max(this._targetSpeed - ACCELERATION * delta, 0)
        }

        // LERP exponencial (inércia): speed lerps toward target
        this._speed += (this._targetSpeed - this._speed) * 0.05

        /**
         * ROTAÇÃO: θ_new = θ_old + ω × Δt
         * Aplica Ry(θ) a todo o grupo (cilindro visual + frestas giram juntos).
         */
        this._group.rotation.y += this._speed * delta

        /**
         * PROJEÇÃO CONTÍNUA (scroll acompanha cilindro):
         *
         * A cada frame de renderização, redesenhamos o canvas com a porção
         * da strip correspondente à rotação atual. Isso faz a imagem projetada
         * deslizar na tela à mesma velocidade que o cilindro gira.
         *
         * As frestas (solidárias ao cilindro) projetam sombras que se movem
         * na mesma velocidade → cada fresta enquadra exatamente seu frame.
         *
         * Otimização: só atualiza quando há velocidade (evita re-upload a 0 rad/s).
         */
        if (this._stripImage && this._canvasTexture && Math.abs(this._speed) > 0.001) {
            this._drawProjection()
            this._canvasTexture.needsUpdate = true
        }
    }

    // ==========================================================================
    // GETTERS (Sprint 2)
    // ==========================================================================

    getGroup() { return this._group }
    getSpeedNormalized() { return this._speed / MAX_SPEED }
    getSpeed() { return this._speed }
    getMaxSpeed() { return MAX_SPEED }

    setControlsEnabled(enabled) {
        if (!enabled) {
            this._keys.up = false
            this._keys.down = false
        }
        this._controlsEnabled = enabled
    }

    // ==========================================================================
    // CLEANUP
    // ==========================================================================

    dispose(scene) {
        window.removeEventListener('keydown', this._onKeyDown)
        window.removeEventListener('keyup', this._onKeyUp)

        this._group.traverse((child) => {
            if (child.isMesh) {
                child.geometry.dispose()
                if (child.material.map) child.material.map.dispose()
                child.material.dispose()
            }
        })
        scene.remove(this._group)

        if (this._spotLight) {
            this._canvasTexture.dispose()
            scene.remove(this._spotLight)
            scene.remove(this._spotLight.target)
        }
        if (this._screen) {
            this._screen.geometry.dispose()
            this._screen.material.dispose()
            scene.remove(this._screen)
        }
    }
}
