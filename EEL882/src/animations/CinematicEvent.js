import * as THREE from 'three'

/**
 * ============================================================================
 * CINEMATIC EVENT — Câmera acompanha velocidade do zoetrópio
 * ============================================================================
 *
 * CONCEITO:
 *   A posição da câmera é DIRETAMENTE VINCULADA à velocidade do zoetrópio.
 *   Conforme o usuário acelera (↑), a câmera se aproxima suavemente da parede
 *   de projeção. Ao atingir velocidade suficiente, a câmera naturalmente
 *   atravessa a parede e entra na cena — sem trigger abrupto.
 *
 *   Vantagens:
 *   - Feedback visual contínuo (câmera responde imediatamente à velocidade)
 *   - Transição orgânica (sem "snap" de estado)
 *   - Efeito "rubber band": se o usuário para de acelerar, a câmera não avança
 *
 * CONCEITOS DE COMPUTAÇÃO GRÁFICA:
 *
 * 1. INTERPOLAÇÃO LINEAR (LERP):
 *    Dado dois pontos P₀ e P₁, a posição intermediária em t ∈ [0,1]:
 *      P(t) = P₀ × (1 - t) + P₁ × t
 *    Aqui, t é derivado da velocidade normalizada do zoetrópio.
 *
 * 2. SMOOTHSTEP (Easing Hermite):
 *    s(t) = 3t² - 2t³
 *    Aplicada ao parâmetro de velocidade para suavizar o mapeamento
 *    velocidade→posição. Sem smoothstep, a câmera teria velocidade constante
 *    por unidade de aceleração. Com smoothstep:
 *    - Primeiros 30% de velocidade: câmera mal se move (ease-in)
 *    - Velocidades altas: câmera acelera dramaticamente (ease-out)
 *    Resultado: início sutil, final cinematográfico.
 *
 * 3. FRUSTUM CULLING (após travessia):
 *    Câmera em (0,4,-12) olhando para (0,4,-15):
 *    Near plane em z = -12.1. Toda a sala (z > -10) fica excluída
 *    automaticamente pelo frustum culling.
 *
 * 4. VIEW MATRIX (lookAt dinâmico):
 *    camera.lookAt(target) reconstrói a View Matrix:
 *      forward = normalize(target - position)
 *      right = normalize(cross(forward, worldUp))
 *      up = cross(right, forward)
 *      V = [right | up | -forward | position]⁻¹
 *    Interpolando posição e target juntos: rotação suave.
 *
 * 5. MAPEAMENTO VELOCIDADE → POSIÇÃO:
 *    t_raw = velocidade / (0.95 × maxSpeed)
 *    t_clamped = clamp(t_raw, 0, 1)
 *    s = smoothstep(t_clamped)
 *    posição = lerp(START, END, s)
 *
 *    O divisor 0.95×max garante que a câmera chega ao destino antes da
 *    velocidade teórica máxima (que com LERP exponencial é assintótica).
 */

// ============================================================================
// CONSTANTES
// ============================================================================

/**
 * Fração da velocidade máxima em que a câmera completa a jornada.
 * Com LERP exponencial (fator 0.05) no ZoetropeAnimation, a velocidade
 * atinge 95% do max após ~58 frames ≈ 1s com targetSpeed = MAX.
 * Mapeamos [0, 0.95×MAX] → [0, 1] para a câmera.
 */
const SPEED_FULL_TRAVEL = 0.95

/**
 * Posições de LERP:
 *   START: posição lateral de observação (CameraManager default)
 *   END: 2 unidades atrás da parede → câmera em z=-12
 */
const START_POS = new THREE.Vector3(8, 5, 6)
const END_POS = new THREE.Vector3(0, 4, -12)

/**
 * Alvos de lookAt:
 *   LOOK_START: olhando entre zoetrópio e parede (vista geral)
 *   LOOK_END: olhando além da parede (dentro da cena)
 *
 * Distância mínima posição↔lookAt = 3.0 (em t=1) → seguro.
 */
const LOOK_START = new THREE.Vector3(0, 3, -2)
const LOOK_END = new THREE.Vector3(0, 4, -15)

/**
 * Z a partir do qual a parede é ocultada (evita z-fighting com near plane).
 * wall.z = -10.05, near = 0.1 → camera.z < -10.05 + 0.2 = -9.85
 */
const WALL_HIDE_Z = -9.85

// ============================================================================
// ESTADOS
// ============================================================================

const STATE = {
    /** Câmera rastreia velocidade do zoetrópio (bidirecional) */
    TRACKING: 'TRACKING',
    /** Câmera passou pela parede — posição final fixa */
    INSIDE: 'INSIDE',
}

// ============================================================================
// CLASSE
// ============================================================================

export class CinematicEvent {
    /**
     * @param {THREE.PerspectiveCamera} camera
     * @param {ZoetropeAnimation} zoetrope
     * @param {RoomEnvironment} room
     */
    constructor(camera, zoetrope, room) {
        this._camera = camera
        this._zoetrope = zoetrope
        this._room = room

        this._state = STATE.TRACKING

        /**
         * Vetores auxiliares reutilizados a cada frame.
         * Evita alocação no render loop (previne GC spikes).
         */
        this._currentPos = new THREE.Vector3()
        this._currentLook = new THREE.Vector3()

        /** Flag pública — Sprint 3 lê para revelar cena 3D */
        this.isInside = false
    }

    // ==========================================================================
    // UPDATE
    // ==========================================================================

    update(time, delta) {
        switch (this._state) {
            case STATE.TRACKING:
                this._updateTracking()
                break
            case STATE.INSIDE:
                // Câmera fixa na posição final
                break
        }
    }

    // ==========================================================================
    // TRACKING — Câmera acompanha velocidade
    // ==========================================================================

    _updateTracking() {
        /**
         * MAPEAMENTO VELOCIDADE → PARÂMETRO t:
         *
         *   speedNorm = velocidade_atual / velocidade_máxima ∈ [0, 1]
         *   t_raw = speedNorm / SPEED_FULL_TRAVEL
         *
         * Dividindo por 0.95, a câmera atinge t=1.0 quando speed = 95% max.
         * Isso compensa a convergência assintótica do LERP exponencial
         * (speed nunca chega exatamente a MAX com fator 0.05).
         *
         * clamp(t, 0, 1) garante que não extrapolamos além do destino.
         */
        const speedNorm = this._zoetrope.getSpeedNormalized()
        const tRaw = speedNorm / SPEED_FULL_TRAVEL
        const tClamped = Math.min(Math.max(tRaw, 0), 1)

        /**
         * SMOOTHSTEP — Hermite easing:
         *   s(t) = 3t² - 2t³
         *
         * Efeito no mapeamento velocidade→câmera:
         *   - Velocidades baixas (0-30% max): câmera quase não se move
         *     (usuário vê o zoetrópio normalmente, sem distração)
         *   - Velocidades médias (30-70%): câmera começa a se aproximar
         *   - Velocidades altas (70-95%): câmera avança rápido → cinema!
         *
         * Derivada s'(t) = 6t(1-t):
         *   s'(0) = 0 → início imperceptível
         *   s'(0.5) = 1.5 → máxima velocidade de câmera no meio
         *   s'(1) = 0 → chegada suave
         */
        const s = tClamped * tClamped * (3 - 2 * tClamped)

        /**
         * INTERPOLAÇÃO LINEAR da posição:
         *   P(s) = START_POS × (1 - s) + END_POS × s
         */
        this._currentPos.lerpVectors(START_POS, END_POS, s)
        this._camera.position.copy(this._currentPos)

        /**
         * INTERPOLAÇÃO LINEAR do lookAt:
         *   L(s) = LOOK_START × (1 - s) + LOOK_END × s
         *
         * camera.lookAt() atualiza a View Matrix (rotation da câmera).
         */
        this._currentLook.lerpVectors(LOOK_START, LOOK_END, s)
        this._camera.lookAt(this._currentLook)

        /**
         * OCULTAÇÃO DA PAREDE + TRANSIÇÃO:
         * Quando a câmera cruza z = -9.85, a parede (z=-10.05) está a
         * menos de 0.2 unidades — risco de z-fighting com near plane (0.1).
         * Escondemos a parede e travamos o estado como INSIDE.
         *
         * Ao entrar em INSIDE:
         * - Controles do zoetrópio são desativados (sem retorno)
         * - Câmera para na posição final (END_POS)
         * - Sprint 3 pode ler this.isInside para revelar a cena 3D
         */
        if (this._camera.position.z < WALL_HIDE_Z) {
            const wall = this._room.getWall()
            if (wall) wall.visible = false

            this._zoetrope.setControlsEnabled(false)
            this._camera.position.copy(END_POS)
            this._camera.lookAt(LOOK_END)

            this._state = STATE.INSIDE
            this.isInside = true
        }
    }
}
