# 05 — Sprint 3: Cena do Cavalo 3D (HorseScene)

Este documento explica a revelação da cena 3D — cavalo galopando com jóquei montado, iluminação dramática e controles orbitais.

---

## 1. Conceito Narrativo

Após a câmera atravessar a parede (Sprint 2), a animação 2D do Muybridge "ganha vida" como um modelo 3D:

```
ANTES da travessia:           DEPOIS da travessia:
┌──────────────┐              ┌──────────────────────┐
│   Projeção   │              │  Cavalo 3D galopando │
│   na parede  │   ═══►      │  + Jóquei montado    │
│   (2D flat)  │              │  + Iluminação cinema │
└──────────────┘              │  + OrbitControls     │
                              └──────────────────────┘
```

---

## 2. Carregamento de Modelos (GLTF/GLB)

### 2.1 Horse.glb

| Propriedade | Valor |
|-------------|-------|
| Formato | GLB (binário) |
| Tamanho | 177 KB |
| Animação | Morph Targets (blend shapes) |
| Escala interna | ~100 unidades |
| Escala aplicada | 0.015 → ~1.5 unidades |

```javascript
const loader = new GLTFLoader()
loader.load('/models/Horse.glb', (gltf) => {
    this._model = gltf.scene
    // gltf.animations[0] → clip de galope (morph targets)
})
```

### 2.2 Xbot.glb (Jóquei)

| Propriedade | Valor |
|-------------|-------|
| Formato | GLB (binário) |
| Tamanho | 2.8 MB |
| Animação | Skeletal (bones + skinning) |
| Clips disponíveis | dance, headShake, idle, run, sad_pose, sneak_pose, walk |
| Escala aplicada | 1.6 → ~1.6 unidades |

---

## 3. Morph Targets (Cavalo)

### 3.1 Como Funciona

O Horse.glb armazena posições alternativas dos vértices para cada frame do galope:

$$\mathbf{v}_{final} = \mathbf{v}_{base} + \sum_{i=0}^{N-1} w_i \times (\mathbf{v}_{target_i} - \mathbf{v}_{base})$$

A cada frame, o `AnimationMixer` interpola os pesos $w_i$ entre keyframes:

```
Tempo: ──────────────────────────────►
Frame:  0    1    2    3    4    5    ...
w[0]:   1.0  0.5  0.0  0.0  0.0  0.0
w[1]:   0.0  0.5  1.0  0.5  0.0  0.0
w[2]:   0.0  0.0  0.0  0.5  1.0  0.5
...
```

Apenas 1-2 targets têm peso >0 por vez (transição suave entre poses).

### 3.2 GPU Morphing

Three.js computa morph targets no **vertex shader** (GPU):

```glsl
// Simplificado
vec3 transformed = position;  // base
transformed += morphTarget0 * morphTargetInfluences[0];
transformed += morphTarget1 * morphTargetInfluences[1];
// ...
```

**Consequência**: `geometry.computeBoundingBox()` NÃO reflete os morph targets (calcula apenas com posições base). Por isso usamos bbox estática + oscilação senoidal para posicionar o jóquei.

### 3.3 AnimationMixer (Cavalo)

```javascript
this._mixer = new THREE.AnimationMixer(this._model)
const clip = gltf.animations[0]  // Clip de galope
const action = this._mixer.clipAction(clip)
action.play()

// No update loop:
this._mixer.update(delta)  // Avança animação por Δt segundos
```

O mixer:
1. Incrementa clock interno por `delta`
2. Calcula tempo local no clip (com wrapping se loop)
3. Interpola keyframes → valores de `morphTargetInfluences`
4. Three.js atualiza uniforms → GPU recalcula vértices

---

## 4. Skeletal Animation (Jóquei)

### 4.1 Conceito de Skinning

O Xbot tem um **esqueleto** (hierarquia de bones). Cada vértice do mesh tem até 4 bones de influência:

$$\mathbf{v}_{final} = \sum_{i=0}^{3} w_i \times (B_i \times B_{i,bind}^{-1} \times \mathbf{v}_{local})$$

Onde:
- $B_i$ = matriz world do bone $i$ no frame atual
- $B_{i,bind}^{-1}$ = inversa da bind matrix (pose de referência "T-pose")
- $w_i$ = skinning weight (soma = 1.0)

### 4.2 Animação Aditiva

O Xbot usa **blending aditivo** para compor múltiplas animações:

```
Resultado = Base + Σ(weight × Additive)
```

#### Base: idle (index 2)
Pose parada de referência. Weight = 1.0.

#### Additive 1: sneak_pose (index 5)
Converte para aditivo com `makeClipAdditive()`:
$$key_{add}[j] = key_{orig}[j] - key_{orig}[0]$$

Depois extrai um único frame com `subclip(clip, name, 2, 3, 30)`:
- Frame 2-3 a 30fps = pose estática agachada

Resultado: o jóquei fica **agachado** (pose de cavaleiro).

#### Additive 2: headShake (index 1)
Animação completa (não subclip) tornada aditiva. O jóquei **balança a cabeça** continuamente enquanto está montado.

```javascript
// Base
const idleAction = mixer.clipAction(idleClip)
idleAction.play()

// Additive 1: sneak (pose estática)
THREE.AnimationUtils.makeClipAdditive(sneakClip)
sneakClip = THREE.AnimationUtils.subclip(sneakClip, name, 2, 3, 30)
const sneakAction = mixer.clipAction(sneakClip)
sneakAction.setEffectiveWeight(1)
sneakAction.play()

// Additive 2: headShake (animação contínua)
THREE.AnimationUtils.makeClipAdditive(headShakeClip)
const headShakeAction = mixer.clipAction(headShakeClip)
headShakeAction.setEffectiveWeight(1)
headShakeAction.play()
```

### 4.3 Manipulação Direta de Bones (Abdução das Pernas)

Após o mixer aplicar a pose, rotacionamos manualmente os bones das coxas para **abrir as pernas** (pose de cavaleiro montado):

```javascript
// A cada frame, DEPOIS de jockeyMixer.update(delta):
if (this._leftUpLeg) {
    this._leftUpLeg.rotation.z += LEG_SPREAD_ANGLE   // +0.4 rad ≈ +23°
}
if (this._rightUpLeg) {
    this._rightUpLeg.rotation.z -= LEG_SPREAD_ANGLE  // -0.4 rad ≈ -23°
}
```

**Por que funciona**: o mixer define a rotação dos bones, depois nós ADICIONAMOS a rotação de abertura. Isso é feito a cada frame porque o mixer sobrescreve os valores.

```
Vista frontal (antes):     Vista frontal (depois):
       O                         O
      /|\                       /|\
      / \   ← pernas juntas    / | \   ← pernas abertas
     /   \                    /  |  \      (±23°)
```

---

## 5. Posicionamento do Jóquei

### 5.1 Desafio

O cavalo se move verticalmente durante o galope (morph targets mudam a forma). O jóquei precisa acompanhar esse movimento para parecer "sentado" no dorso.

### 5.2 Solução: BBox Estática + Senoide

Como `computeBoundingBox()` não reflete morph targets (calculados na GPU), usamos:

1. **Bbox estática** → determina a posição BASE do dorso:
   $$y_{base} = y_{min} + (y_{max} - y_{min}) \times 0.4$$
   (40% da altura total ≈ dorso do cavalo)

2. **Oscilação senoidal** sincronizada ao mixer → simula o sobe-e-desce:
   $$y_{bounce} = A \times \sin\left(\frac{2\pi \times (t_{mixer} \mod T)}{T}\right)$$

3. **Posição final**:
   $$y_{jóquei} = y_{base} + y_{bounce}$$

```javascript
// Usa o tempo do mixer do cavalo para sincronizar
const T = this._horseClipDuration
const mixerTime = this._mixer.time % T
const cycle = (2 * Math.PI * mixerTime) / T
const bounceY = 0.12 * Math.sin(cycle)
this._jockey.position.y = backY + bounceY
```

### 5.3 Por que o Mixer Time?

Usando `this._mixer.time` (não o clock global `time`), a oscilação é derivada do **mesmo relógio** que anima o cavalo → sincronia perfeita.

---

## 6. Iluminação Dramática (Three-Point Lighting)

### 6.1 Key Light (SpotLight)

Iluminação principal — posicionada a ~45° lateral/acima:

```
        Key Light (3, 8, -13)
             ↘
              \
               ●━━━━━ Cavalo (0, 0, -15)
              /
             ↗
       Câmera
```

```javascript
new THREE.SpotLight(0xffffff, 200)
    .position.set(3, 8, -13)
    .target.position.set(0, 1.5, -15)
    .angle = Math.PI / 5
    .penumbra = 0.4
    .castShadow = true
```

### 6.2 Rim Light (PointLight)

Contra-luz por trás — cria silhueta/contorno luminoso:

```
                   Rim Light (-2, 6, -18)
                        ↙
Câmera ←──── ●━━━━ Cavalo ━━━━━ ◄ luz
                                (backlight)
```

```javascript
new THREE.PointLight(0x4488ff, 80)  // Azulado para contraste
    .position.set(-2, 6, -18)
```

### 6.3 Ground Plane (Receptor de Sombra)

```javascript
new THREE.PlaneGeometry(12, 12)  // Chão escuro
    .rotation.x = -Math.PI / 2   // Horizontal
    .position.set(0, 0, -15)     // Sob o cavalo
    .receiveShadow = true
```

Sem o ground plane, a sombra do key light não teria onde pousar e o cavalo pareceria flutuar.

---

## 7. OrbitControls

### 7.1 Configuração

```javascript
const controls = new OrbitControls(camera, domElement)
controls.target.set(0, 1.5, -15)  // Centro do cavalo
controls.enableDamping = true      // Inércia suave
controls.dampingFactor = 0.08
controls.minDistance = 2           // Não pode chegar muito perto
controls.maxDistance = 12          // Não pode se afastar demais
controls.enabled = false           // Desativado até revelação
```

### 7.2 Coordenadas Esféricas

O usuário controla a câmera em coordenadas esféricas $(r, \theta, \varphi)$ ao redor do target:

$$\begin{cases}
x_{cam} = x_{target} + r \sin\varphi \sin\theta \\
y_{cam} = y_{target} + r \cos\varphi \\
z_{cam} = z_{target} + r \sin\varphi \cos\theta
\end{cases}$$

- **Arrastar horizontal** → altera $\theta$ (azimute)
- **Arrastar vertical** → altera $\varphi$ (polar)
- **Scroll** → altera $r$ (distância)

### 7.3 Damping (Amortecimento)

Com `enableDamping = true`:

$$\theta_{new} = \theta_{current} + (\theta_{target} - \theta_{current}) \times dampingFactor$$

Isso cria **inércia** — ao soltar o mouse, a câmera continua girando suavemente e para gradualmente. Requer `controls.update()` a cada frame.

---

## 8. Revelação (_reveal)

Quando `cinematic.isInside` se torna `true` pela primeira vez:

```javascript
_reveal() {
    // 1. Esconde zoetrópio (não é mais visível pelo frustum, mas por segurança)
    zoetrope.getGroup().visible = false
    
    // 2. Constrói iluminação (lazy — só quando necessário)
    this._buildLighting()
    
    // 3. Revela tudo
    model.visible = true
    jockey.visible = true
    keyLight.visible = true
    rimLight.visible = true
    ground.visible = true
    
    // 4. Ativa controles orbitais
    controls.enabled = true
}
```

A construção é **lazy** (iluminação criada apenas quando necessária) para não desperdiçar GPU antes da revelação.

---

## 9. Update Loop Completo

```
HorseScene.update(time, delta):
│
├── if (!cinematic.isInside) → return (nada acontece)
│
├── if (!revealed) → _reveal() (uma vez)
│
├── mixer.update(delta)              ← Avança morph targets do cavalo
│
├── jockeyMixer.update(delta)        ← Avança headShake + idle + sneak
│
├── leftUpLeg.rotation.z += 0.4      ← Abre perna esquerda
├── rightUpLeg.rotation.z -= 0.4     ← Abre perna direita
│
├── Bounding box tracking:
│   ├── computeBoundingBox()         ← BBox estática (base geometry)
│   ├── applyMatrix4(worldMatrix)    ← Transforma para world space
│   ├── backY = min + (max-min)×0.4  ← 40% da altura = dorso
│   ├── bounceY = 0.12 × sin(cycle) ← Senoide sincronizada
│   └── jockey.y = backY + bounceY  ← Posição final
│
└── controls.update()                ← Damping dos OrbitControls
```

---

## 10. Model Matrices

### Cavalo

$$M_{horse} = T(0, 0, -15) \times R_y(-\pi/2) \times S(0.015)$$

- Escala 0.015: modelo de ~100 unidades → 1.5 unidades
- Rotação $-\pi/2$: vira para a esquerda (galopa no eixo -X)
- Translação $(0, 0, -15)$: frente da câmera após travessia

### Jóquei

$$M_{jockey} = T(0, y_{dinâmico}, -15) \times R_y(-\pi/2) \times S(1.6)$$

- Escala 1.6: Xbot (~1m) → 1.6m (proporção humana sobre cavalo)
- Rotação $-\pi/2$: mesma direção do cavalo
- Translação: mesmo (x, z), y calculado dinamicamente

---

## 11. Constantes

| Constante | Valor | Significado |
|-----------|-------|-------------|
| `HORSE_POSITION` | (0, 0, -15) | Centro do cavalo no mundo |
| `HORSE_SCALE` | 0.015 | Fator de escala do modelo |
| `JOCKEY_SCALE` | 1.6 | Escala do Xbot |
| `JOCKEY_OFFSET_Y` | 0.85 | Offset base (ajustado via bbox) |
| `LEG_SPREAD_ANGLE` | 0.4 rad | Abertura das pernas (~23°) |
| `KEY_LIGHT_POS` | (3, 8, -13) | Posição da key light |
| `RIM_LIGHT_POS` | (-2, 6, -18) | Posição da rim light |
| Bounce amplitude | 0.12 | Amplitude da oscilação vertical |
| BBox height ratio | 0.4 | Fração da bbox = dorso |
