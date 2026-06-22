# 01 — Fundamentos de Computação Gráfica

Este documento cobre **todos os conceitos matemáticos e de CG** utilizados no projeto, desde o mais básico até os mais avançados. Ele serve como referência para a defesa oral.

---

## 1. Sistemas de Coordenadas

### 1.1 Espaço 3D (Cartesiano)

O Three.js usa o sistema de coordenadas **right-handed** (mão direita):

```
       Y (up)
       │
       │
       │_______ X (right)
      /
     /
    Z (toward viewer)
```

- **X** → direita
- **Y** → cima
- **Z** → em direção ao observador (para fora da tela)

Um ponto no espaço 3D é um vetor:

$$\mathbf{p} = \begin{pmatrix} x \\ y \\ z \end{pmatrix}$$

### 1.2 Coordenadas Homogêneas

Para unificar translação, rotação e escala em uma única multiplicação de matrizes, usamos **coordenadas homogêneas** (4 componentes):

$$\mathbf{p}_{homogêneo} = \begin{pmatrix} x \\ y \\ z \\ w \end{pmatrix}$$

- **Pontos**: $w = 1$ → $\begin{pmatrix} x & y & z & 1 \end{pmatrix}^T$
- **Vetores/Direções**: $w = 0$ → $\begin{pmatrix} x & y & z & 0 \end{pmatrix}^T$ (imune a translação)

Para voltar ao 3D (divisão perspectiva):

$$\mathbf{p}_{3D} = \begin{pmatrix} x/w \\ y/w \\ z/w \end{pmatrix}$$

---

## 2. Transformações Geométricas (Matrizes 4×4)

Toda transformação 3D é representada por uma **matriz 4×4** aplicada a coordenadas homogêneas.

### 2.1 Translação

Move um objeto pela distância $(t_x, t_y, t_z)$:

$$T = \begin{pmatrix} 1 & 0 & 0 & t_x \\ 0 & 1 & 0 & t_y \\ 0 & 0 & 1 & t_z \\ 0 & 0 & 0 & 1 \end{pmatrix}$$

**No projeto**: `this._model.position.set(0, 0, -15)` aplica $T(0, 0, -15)$ ao cavalo.

### 2.2 Escala

Modifica o tamanho do objeto:

$$S = \begin{pmatrix} s_x & 0 & 0 & 0 \\ 0 & s_y & 0 & 0 \\ 0 & 0 & s_z & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix}$$

Escala **uniforme** (mesma em todos eixos): $s_x = s_y = s_z = s$

**No projeto**: `this._model.scale.setScalar(0.015)` → $S(0.015, 0.015, 0.015)$. O Horse.glb tem ~100 unidades internas, fica com ~1.5 unidades do mundo.

### 2.3 Rotação

#### Em torno do eixo Y (mais usada no projeto):

$$R_y(\theta) = \begin{pmatrix} \cos\theta & 0 & \sin\theta & 0 \\ 0 & 1 & 0 & 0 \\ -\sin\theta & 0 & \cos\theta & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix}$$

**No projeto**: `this._group.rotation.y += ω × Δt` → o cilindro gira continuamente.

#### Em torno do eixo X:

$$R_x(\theta) = \begin{pmatrix} 1 & 0 & 0 & 0 \\ 0 & \cos\theta & -\sin\theta & 0 \\ 0 & \sin\theta & \cos\theta & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix}$$

**No projeto**: `floor.rotation.x = -π/2` → transforma o plano XY em plano XZ (chão horizontal).

### 2.4 Composição de Transformações (Model Matrix)

A **Model Matrix** combina todas as transformações de um objeto:

$$M = T \times R \times S$$

A ordem importa! Three.js aplica: **Escala → Rotação → Translação** (lido da direita para esquerda).

Para o cavalo:
$$M_{cavalo} = T(0, 0, -15) \times R_y(-\pi/2) \times S(0.015)$$

Resultado: o cavalo é escalado, girado para a esquerda, e posicionado em z=-15.

---

## 3. Pipeline de Renderização

O Three.js (via WebGL) processa cada vértice pelo **pipeline**:

```
Vértice Local → Model Matrix → World Space → View Matrix → Camera Space → Projection Matrix → Clip Space → Viewport → Tela
```

### 3.1 Model Matrix (Objeto → Mundo)

Já vista acima. Transforma coordenadas locais do objeto em coordenadas do mundo.

### 3.2 View Matrix (Mundo → Câmera)

Transforma o mundo para o referencial da câmera (câmera na origem, olhando para -Z):

$$V = \begin{pmatrix} r_x & r_y & r_z & -\mathbf{r} \cdot \mathbf{e} \\ u_x & u_y & u_z & -\mathbf{u} \cdot \mathbf{e} \\ -f_x & -f_y & -f_z & \mathbf{f} \cdot \mathbf{e} \\ 0 & 0 & 0 & 1 \end{pmatrix}$$

Onde:
- $\mathbf{f}$ = `normalize(target - position)` (forward)
- $\mathbf{r}$ = `normalize(cross(f, worldUp))` (right)
- $\mathbf{u}$ = `cross(r, f)` (up)
- $\mathbf{e}$ = posição da câmera

**No projeto**: `camera.lookAt(target)` reconstrói a View Matrix.

### 3.3 Projection Matrix (Câmera → Clip Space)

Câmera **perspectiva** (simula visão humana com objetos distantes menores):

$$P = \begin{pmatrix} \frac{1}{a \cdot \tan(fov/2)} & 0 & 0 & 0 \\ 0 & \frac{1}{\tan(fov/2)} & 0 & 0 \\ 0 & 0 & \frac{-(f+n)}{f-n} & \frac{-2fn}{f-n} \\ 0 & 0 & -1 & 0 \end{pmatrix}$$

Onde:
- $fov$ = field of view (60° no projeto)
- $a$ = aspect ratio (largura/altura)
- $n$ = near plane (0.1)
- $f$ = far plane (1000)

**Divisão perspectiva**: Após multiplicação por $P$, divide-se por $w$:
$$\mathbf{p}_{ndc} = \frac{1}{w}\begin{pmatrix} x \\ y \\ z \end{pmatrix}$$

Isso é o que faz objetos distantes parecerem menores.

---

## 4. Iluminação (Modelo de Phong / PBR)

### 4.1 Tipos de Luz no Projeto

| Tipo | Equação de Atenuação | Uso no Projeto |
|------|---------------------|----------------|
| **AmbientLight** | $I_{amb} = k_a \times I$ (constante) | Iluminação base (âmbar vintage) |
| **PointLight** | $I = \frac{I_0}{4\pi d^2}$ (decay=2) | Lâmpada da sala, rim light do cavalo |
| **SpotLight** | $I = \frac{I_0}{d^2} \times \text{cone}(\theta)$ | Projetor, key light do cavalo |

### 4.2 SpotLight — Cone de Luz

O SpotLight ilumina apenas dentro de um cone:

```
        * (posição)
       /|\
      / | \   ← angle (meia-abertura)
     /  |  \
    /   |   \
   /    ↓    \
  /  target   \
 /─────────────\  ← penumbra (suavização da borda)
```

- `angle` = $\alpha$ = meia-abertura do cone (π/8 no projetor = 22.5°)
- `penumbra` = fator de suavização na borda [0,1]
- `target` = ponto para onde o cone aponta

### 4.3 Shadow Mapping (Mapa de Sombras)

Técnica em dois passos:

1. **Shadow Pass**: Renderiza a cena do ponto de vista da LUZ → gera depth map (quem a luz "vê")
2. **Main Pass**: Para cada fragmento, projeta no espaço da luz e compara profundidade:
   - Se `depth_fragmento > depth_mapa` → está em SOMBRA
   - Caso contrário → está iluminado

**No projeto**: `castShadow = true` (mesa, cavalo) + `receiveShadow = true` (chão, parede) + `shadowMap.type = PCFSoftShadowMap` (filtragem para sombras suaves).

---

## 5. Texturas e UV Mapping

### 5.1 O que é Textura?

Uma imagem 2D "colada" na superfície 3D. Cada vértice do mesh tem coordenadas UV:

$$UV \in [0,1]^2$$

- $u$ = coordenada horizontal na textura
- $v$ = coordenada vertical na textura

### 5.2 UV Mapping Cilíndrico

Para `CylinderGeometry`, Three.js gera UVs automaticamente:

$$u = \frac{\theta}{2\pi}, \quad v = \frac{y}{h}$$

Onde $\theta$ é o ângulo ao redor do cilindro. A strip do Muybridge (12 frames lado a lado) é "enrolada" — cada frame ocupa $\frac{1}{12}$ da circunferência (30°).

### 5.3 CanvasTexture (Textura Dinâmica)

Uma textura criada a partir de um `<canvas>` HTML:

```javascript
const canvas = document.createElement('canvas')
const ctx = canvas.getContext('2d')
// Desenha no canvas...
const texture = new THREE.CanvasTexture(canvas)
texture.needsUpdate = true  // Força re-upload para GPU
```

**No projeto**: Usado para controlar qual frame do Muybridge é projetado pelo SpotLight a cada instante.

### 5.4 SpotLight.map (Projeção de Textura)

`SpotLight.map` modula a cor da luz com uma textura projetada:

$$\text{cor}_{projetada} = I_{spot} \times \text{texture2D}(map, uv_{proj})$$

As UVs de projeção são calculadas por:
$$uv_{proj} = \frac{(M_{shadow} \times \mathbf{p}_{world}).xy}{(M_{shadow} \times \mathbf{p}_{world}).w}$$

Funciona exatamente como um **projetor de slides** (pinhole invertido).

---

## 6. Interpolação Linear (LERP)

### 6.1 Definição

Dados dois valores $A$ e $B$, o LERP com parâmetro $t \in [0,1]$:

$$\text{lerp}(A, B, t) = A \times (1 - t) + B \times t$$

- $t = 0$ → resultado = $A$
- $t = 1$ → resultado = $B$
- $t = 0.5$ → resultado = ponto médio

### 6.2 LERP Exponencial (Suavização)

Para transições suaves com "inércia":

$$v_{new} = v_{current} + (v_{target} - v_{current}) \times factor$$

Com $factor = 0.05$ (ZoetropeAnimation):
- Convergência: $v$ alcança 95% do target em $\frac{\ln(20)}{1} \approx 60$ frames
- Nunca ultrapassa o target (sem overshoot)

**No projeto**: Velocidade do zoetrópio usa LERP exponencial para suavizar aceleração/desaceleração.

### 6.3 Smoothstep (Easing Hermite)

Suaviza um parâmetro $t$ para evitar transições lineares bruscas:

$$s(t) = 3t^2 - 2t^3$$

Propriedades:
- $s(0) = 0$, $s(1) = 1$
- $s'(0) = 0$ (começa devagar — ease-in)
- $s'(1) = 0$ (termina devagar — ease-out)
- $s'(0.5) = 1.5$ (máxima velocidade no meio)

**No projeto** (CinematicEvent): mapeia velocidade→posição da câmera com smoothstep. Resultado: início sutil, final dramático.

---

## 7. Animação

### 7.1 Delta Time (Independência de Frame Rate)

A cada frame, o tempo decorrido desde o último frame é $\Delta t$:

$$\theta_{new} = \theta_{old} + \omega \times \Delta t$$

Garante que a velocidade visual é constante independente de FPS:
- 60 FPS → $\Delta t \approx 16.6ms$ → passos pequenos
- 30 FPS → $\Delta t \approx 33.3ms$ → passos maiores, mesma velocidade visual

### 7.2 Morph Targets (Blend Shapes)

Técnica onde cada frame de animação armazena posições alternativas de TODOS os vértices:

$$\mathbf{v}_{final} = \mathbf{v}_{base} + \sum_{i} w_i \times (\mathbf{v}_{target_i} - \mathbf{v}_{base})$$

- $\mathbf{v}_{base}$ = geometria de referência
- $\mathbf{v}_{target_i}$ = posição do vértice no frame $i$
- $w_i \in [0,1]$ = peso (influence) do morph target $i$

**No projeto**: Horse.glb usa morph targets para o galope. O AnimationMixer interpola os pesos ao longo do tempo.

### 7.3 Skeletal Animation (Animação por Esqueleto)

Hierarquia de **bones** (ossos) que deformam o mesh via **skinning weights**:

$$\mathbf{v}_{final} = \sum_{i} w_i \times (B_i \times B_i^{-1}_{bind} \times \mathbf{v}_{local})$$

Onde:
- $B_i$ = matriz world do bone $i$ no frame atual
- $B_i^{-1}_{bind}$ = inversa da matriz bind pose (pose de referência)
- $w_i$ = peso de influência (max 4 bones por vértice)

**No projeto**: Xbot.glb (jóquei) usa esqueleto com bones articulados.

### 7.4 Animação Aditiva (Additive Blending)

Permite combinar animações somando **diferenças** sobre uma base:

$$T_{final} = T_{base} + \sum_{i} w_i \times (T_{additive_i} - T_{reference_i})$$

`makeClipAdditive(clip)` converte:
$$key_{additive}[j] = key_{original}[j] - key_{original}[0]$$

**No projeto**: Jóquei = idle (base) + sneak_pose (agachado, weight=1) + headShake (movimento de cabeça, weight=1).

### 7.5 AnimationMixer

Sistema de playback do Three.js:

```
mixer.update(Δt) → avança clock → interpola keyframes → atualiza propriedades
```

Para cada `AnimationClip`:
- `tracks[]` = array de propriedades animadas
- Cada track tem `times[]` e `values[]` (keyframes)
- `interpolant.evaluate(t)` calcula o valor entre keyframes

---

## 8. Coordenadas Esféricas (OrbitControls)

Câmera orbita ao redor de um ponto (target):

$$\begin{cases}
x = r \times \sin(\varphi) \times \sin(\theta) \\
y = r \times \cos(\varphi) \\
z = r \times \sin(\varphi) \times \cos(\theta)
\end{cases}$$

Onde:
- $r$ = distância ao target (scroll do mouse)
- $\theta$ = azimute / ângulo horizontal (arrastar horizontal)
- $\varphi$ = polar / ângulo vertical (arrastar vertical)

**Damping** (amortecimento):
$$\theta_{new} = \text{lerp}(\theta_{current}, \theta_{target}, dampingFactor)$$

---

## 9. Grafo de Cena (Scene Graph)

Estrutura hierárquica em árvore:

```
Scene
├── AmbientLight
├── Group (zoetrópio — ROTACIONA)
│   ├── Cylinder (filme)
│   └── Base / Topo
├── SpotLight (projetor — FIXO)
├── Wall (parede)
├── Floor (chão)
├── Table (mesa)
├── Horse Model (com morph targets)
├── Jockey Model (com skeleton)
├── Key SpotLight
└── Rim PointLight
```

Transformações são **herdadas**: filho herda a transformação do pai.
- Rotacionar o Group → todos os filhos giram juntos
- Mover o Group → filhos se movem juntos

**World Matrix** de um nó = `parentWorldMatrix × localMatrix`

---

## 10. Formato GLTF/GLB

Padrão Khronos Group para modelos 3D:

```
[Header 12 bytes] [JSON chunk] [Binary chunk]
```

- **JSON chunk**: metadados (hierarquia, materiais, animações, referências a buffers)
- **Binary chunk**: dados brutos (vértices, índices, morph targets, texturas)

Vantagens:
- Arquivo único (.glb) — carregamento eficiente
- Buffers prontos para GPU (sem conversão)
- Suporte nativo a animações, morph targets, skinning

**No projeto**: `GLTFLoader` carrega Horse.glb e Xbot.glb assincronamente.

---

## 11. Bounding Box (AABB)

**Axis-Aligned Bounding Box** — caixa que envolve completamente um objeto, alinhada aos eixos:

$$AABB = \{min(x,y,z), max(x,y,z)\}$$

Usada para:
- Culling rápido (descartar objetos fora do frustum)
- Posicionamento relativo (ex: jóquei no dorso do cavalo)

**Limitação**: `computeBoundingBox()` calcula apenas com a geometria BASE. Morph targets (que rodam na GPU) não são refletidos.

**No projeto**: Usamos bbox estática do cavalo para determinar a altura base do dorso (~40% da altura total), e somamos uma oscilação senoidal para simular o sobe-e-desce do galope.
