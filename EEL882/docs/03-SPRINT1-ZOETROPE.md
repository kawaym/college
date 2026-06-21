# 03 — Sprint 1: Zoetrópio (Projetor Cilíndrico)

Este documento explica em detalhe o funcionamento do `ZoetropeAnimation.js` — o coração do projeto.

---

## 1. Conceito Histórico

O **zoetrópio** (1834, William Horner) é um dos primeiros dispositivos de animação pré-cinema:

```
Vista superior:

         ___________
       /   frestas   \
      /    |  |  |    \      ← Frestas no topo (obturador)
     |  ┌──────────┐  |
     |  │  frames  │  |      ← Strip de imagens colada dentro
     |  │  1..12   │  |
     |  └──────────┘  |
      \               /
       \_____________/
            ↕
        (gira no eixo)
```

O observador olha pelas frestas enquanto o cilindro gira. Cada fresta "congela" um frame por um instante (obturador mecânico). A **persistência retiniana** funde os frames em movimento contínuo.

### Nossa Variação: Projetor

Em vez de olhar *para dentro* pelas frestas, colocamos uma **luz dentro** que projeta *para fora*:

```
         Luz (SpotLight)
              │
              ▼
    ┌─── cilindro ───┐         ┌── parede ──┐
    │  frame │ frame  │  ───►   │  projeção  │
    │  (transparência)│         │  (sombra)  │
    └─────────────────┘         └────────────┘
```

A luz atravessa o frame "transparente" e projeta na parede. Os separadores pretos entre frames agem como obturador natural.

---

## 2. Estrutura do Código

```
ZoetropeAnimation
├── constructor(scene)
│   ├── _group (THREE.Group) — rotaciona tudo junto
│   ├── _loadImageAndBuild() — carrega strip, depois constrói
│   └── _setupControls() — keydown/keyup handlers
│
├── _buildFilmCylinder() — mesh visual com textura
├── _buildBase() — discos superior/inferior
├── _buildProjectorLight() — SpotLight + CanvasTexture
├── _buildScreen() — (vazia — projeta direto na parede)
├── _drawProjection() — redesenha canvas com frame atual
│
├── update(time, delta) — chamado a cada frame
│   ├── Aceleração: targetSpeed += ACCEL × Δt (se ↑)
│   ├── Inércia: speed += (target - speed) × 0.05
│   ├── Rotação: group.rotation.y += speed × Δt
│   └── Projeção: _drawProjection() + needsUpdate
│
└── Getters: getGroup(), getSpeed(), getSpeedNormalized(), setControlsEnabled()
```

---

## 3. O Cilindro Visual (Filme)

### 3.1 Geometria

```javascript
new THREE.CylinderGeometry(
    CYLINDER_RADIUS,  // raio topo = 1.0
    CYLINDER_RADIUS,  // raio base = 1.0
    HEIGHT,           // altura = 1.5
    64,               // segmentos radiais (suavidade)
    1,                // segmentos de altura
    true              // openEnded (sem tampas — pode ver dentro)
)
```

O `CylinderGeometry` gera vértices na superfície:

$$\begin{cases}
x = R \times \sin(\phi), \quad \phi \in [0, 2\pi) \\
y \in [-H/2, H/2] \\
z = R \times \cos(\phi)
\end{cases}$$

### 3.2 UV Mapping

UVs automáticos do Three.js para cilindro:

$$u = \frac{\phi}{2\pi}, \quad v = \frac{y + H/2}{H}$$

A strip do Muybridge (3750×200 px, 12 frames) é mapeada assim:
- Horizontalmente: cada frame ocupa $u \in [\frac{i}{12}, \frac{i+1}{12}]$
- Verticalmente: esticada de $v=0$ a $v=1$

Resultado: 12 frames "colados" ao redor do cilindro.

### 3.3 Material (Retroiluminado)

```javascript
new THREE.MeshStandardMaterial({
    map: texture,           // cor difusa (imagem visível)
    emissive: 0xffffff,     // cor emissiva = branco
    emissiveMap: texture,   // mesma textura como emissiva
    emissiveIntensity: 0.4, // 40% de brilho próprio
    transparent: true,
    opacity: 0.85,
})
```

O `emissiveMap` faz o filme parecer **retroiluminado** (como transparências com luz por trás) — independente da iluminação da cena.

---

## 4. O Projetor (SpotLight + CanvasTexture)

### 4.1 CanvasTexture — Textura Dinâmica

```javascript
// Cria um canvas 512×512 como "slide" do projetor
const canvas = document.createElement('canvas')
canvas.width = canvas.height = 512
const ctx = canvas.getContext('2d')

// A cada frame de animação:
ctx.drawImage(strip, sx, 0, sw, sh, 0, 0, 512, 512)
canvasTexture.needsUpdate = true  // Upload para GPU
```

O canvas funciona como o "slide" dentro do projetor. A cada frame de renderização, redesenhamos com a porção correta da strip.

### 4.2 SpotLight como Projetor

```javascript
const spotLight = new THREE.SpotLight(0xffffff, 300)
spotLight.position.set(0, 3.25, 0)      // Centro do cilindro
spotLight.target.position.set(0, 4, -10) // Aponta para parede
spotLight.angle = Math.PI / 8           // Cone de 22.5°
spotLight.map = canvasTexture           // "Slide" projetado
```

O `SpotLight.map` funciona como um **projetor de slides**:
1. A luz parte de um ponto (posição do SpotLight)
2. Passa pela "transparência" (CanvasTexture = map)
3. Projeta na superfície onde atinge

### 4.3 Geometria do Cone Tiltado

O projetor NÃO aponta reto para frente — é **tiltado para cima**:

```
       SpotLight (0, 3.25, 0)
            \  ↗ 4.3° de elevação
             \/
              \
               \_______________
               |              |  ← Parede (z = -10)
               |  PROJEÇÃO    |     Centro em y ≈ 4
               |______________|
```

- **Fonte**: $(0, 3.25, 0)$
- **Target**: $(0, 4, -10)$
- **Elevação**: $\arctan\left(\frac{4 - 3.25}{10}\right) \approx 4.3°$

Isso centra a projeção circular na parede (10×8) sem vazar no chão.

### 4.4 Cobertura na Parede

O raio do cone na parede (distância = 10):

$$r = \tan(\alpha) \times d = \tan(\pi/8) \times 10 \approx 4.14$$

Diâmetro ≈ 8.3 unidades → cobre quase toda a parede (largura 10).

---

## 5. O Scroll Contínuo (Projeção)

### 5.1 Problema

SpotLight.map no WebGL **não respeita** `texture.offset/repeat`. Os UVs de projeção são calculados via a shadow matrix do spot, não pelos atributos da textura.

### 5.2 Solução: Redesenhar o Canvas

A cada frame, recortamos da strip a porção que está "diante da luz" e desenhamos no canvas 512×512.

### 5.3 Cálculo do Offset

A strip completa (3750 px) mapeia $2\pi$ rad. Quando o cilindro gira $\theta$ rad:

$$\text{offset}_{px} = \frac{\theta}{2\pi} \times \text{stripWidth}$$

Porém, a face do cilindro voltada para a parede (z negativo) está em $\phi = \pi$. Com o cilindro girando:

$$\theta_{corrigido} = \pi - \text{rotation}$$

O sinal negativo compensa o espelhamento horizontal do canvas.

### 5.4 Espelhamento Horizontal (Correção Pinhole)

O SpotLight projeta como uma câmera pinhole — **inverte horizontalmente**:

```
   Canvas (desenhado)     Projeção na parede
   ┌──────────┐           ┌──────────┐
   │ A  B  C  │    →      │ C  B  A  │  (invertido!)
   └──────────┘           └──────────┘
```

Solução: desenhamos o canvas **espelhado**:

```javascript
ctx.scale(-1, 1)           // Inverte eixo X
ctx.translate(-width, 0)   // Reposiciona origem
// Desenha normalmente...
ctx.restore()
```

A inversão do canvas × inversão do projetor = imagem correta na parede.

### 5.5 Wrapping Circular

Quando a janela visível cruza o fim da strip, dividimos em duas partes:

```
Strip: [frame 10 | frame 11 | frame 0 | frame 1]
                            ↑ borda (wrap)

Parte 1: do offset até o fim da strip
Parte 2: do início da strip até completar a janela
```

```javascript
if (remaining >= visibleWidth) {
    // Caso simples: uma chamada
    ctx.drawImage(strip, sx, 0, sw, sh, 0, 0, dw, dh)
} else {
    // Wrap: duas chamadas
    ctx.drawImage(strip, sx, 0, remaining, sh, 0, 0, canvasFirst, dh)
    ctx.drawImage(strip, 0, 0, rest, sh, canvasFirst, 0, canvasRest, dh)
}
```

---

## 6. Controles e Física

### 6.1 Entrada do Usuário

```
↑ (ArrowUp)   → acelera (targetSpeed += ACCELERATION × Δt)
↓ (ArrowDown) → desacelera (targetSpeed -= ACCELERATION × Δt)
```

### 6.2 Modelo de Velocidade

```
targetSpeed: velocidade desejada (responde imediatamente ao input)
speed: velocidade real (segue o target com inércia)
```

**LERP exponencial** para suavização:

$$\text{speed} \mathrel{+}= (\text{target} - \text{speed}) \times 0.05$$

Propriedades:
- **Sem overshoot** (speed nunca ultrapassa target)
- **Convergência suave** (95% em ~60 frames)
- **Responsivo**: começa a reagir imediatamente

### 6.3 Rotação

$$\theta_{new} = \theta_{old} + \omega \times \Delta t$$

Onde $\omega$ = speed (rad/s). Com `MAX_SPEED = 15 rad/s`:
- Rotação máxima: $\frac{15}{2\pi} \approx 2.4$ voltas/segundo

---

## 7. Constantes do Sistema

| Constante | Valor | Significado |
|-----------|-------|-------------|
| `CYLINDER_RADIUS` | 1.0 | Raio do cilindro |
| `HEIGHT` | 1.5 | Altura do cilindro |
| `NUM_FRAMES` | 12 | Frames na strip do Muybridge |
| `MAX_SPEED` | 15.0 | Velocidade angular máxima (rad/s) |
| `ACCELERATION` | 3.0 | Aceleração (rad/s²) |
| `WALL_DISTANCE` | 10.0 | Distância da parede (para far do shadow) |

---

## 8. Resumo Visual

```
Frame N:
  1. Usuário segura ↑ → targetSpeed sobe
  2. speed = lerp(speed, targetSpeed, 0.05)
  3. group.rotation.y += speed × Δt
  4. _drawProjection():
     a. Calcula θ_corrigido = π - rotation
     b. offset = (θ/2π) × stripWidth
     c. Espelha canvas (ctx.scale(-1,1))
     d. Recorta janela da strip com wrapping
     e. Desenha no canvas 512×512
  5. canvasTexture.needsUpdate = true → GPU re-upload
  6. Three.js renderiza:
     - SpotLight projeta canvasTexture na parede
     - Cilindro visual mostra o filme girando (emissive)
     - Resultado: animação sincronizada cilindro ↔ projeção
```
