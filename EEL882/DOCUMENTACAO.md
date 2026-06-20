# Documentação Técnica — Zoetrópio 3D (EEL882)

## Visão Geral do Projeto

Este projeto implementa um **zoetrópio digital** usando Three.js: um cilindro rotativo com as fotografias históricas de Eadweard Muybridge ("The Horse in Motion", 1878) que projeta a animação numa tela plana, simulando o funcionamento de uma **lanterna mágica rotativa**.

---

## 1. Arquitetura do Sistema

```
src/
├── App.js                    → Loop principal (requestAnimationFrame)
├── main.js                   → Ponto de entrada (instancia App)
├── core/
│   ├── SceneManager.js       → Cria a Scene + iluminação ambiente
│   ├── CameraManager.js      → Câmera perspectiva
│   └── RendererManager.js    → WebGLRenderer + shadow maps
├── utils/
│   └── ResizeHandler.js      → Responsividade (resize)
└── animations/
    └── ZoetropeAnimation.js  → Toda a lógica do zoetrópio
```

### Padrão Manager
Cada subsistema (cena, câmera, renderer) é encapsulado numa classe com `get()` para acesso ao objeto Three.js interno. Isso separa configuração de uso.

---

## 2. Grafo de Cena (Scene Graph)

### O que é um Grafo de Cena?

Um grafo de cena é uma **árvore** (estrutura hierárquica) onde cada nó representa um objeto 3D. A ideia fundamental é:

> **A transformação de um nó filho é relativa ao pai.**

Se o pai rotaciona, todos os filhos rotacionam junto — sem precisar atualizar cada um manualmente. É como um braço robótico: se o ombro gira, o cotovelo, pulso e dedos giram junto.

### Nosso grafo:

```
Scene
 ├─ AmbientLight (0.15)         ← visibilidade mínima
 ├─ Group (_group) [ROTACIONA]
 │    ├─ Mesh (CylinderGeometry) ← filme visível
 │    ├─ Mesh (CircleGeometry)   ← base inferior
 │    └─ Mesh (RingGeometry)     ← anel superior
 ├─ SpotLight [FIXO em (0,0,0)] ← projetor com CanvasTexture
 │    └─ target → (0, 0, -8)
 └─ Mesh (PlaneGeometry)        ← tela de projeção branca
```

### Por que essa hierarquia?
- O **Group** gira → o cilindro e suas partes visuais giram juntas (mesmo referencial).
- A **SpotLight** NÃO está no Group → fica fixa no centro → simula lâmpada estacionária dentro do cilindro.
- A **tela** NÃO está no Group → fica fixa → é a "parede" onde vemos a projeção.

### Transformações hierárquicas (a matemática)

Quando rotacionamos o Group, a **model matrix** de todos os filhos é multiplicada pela matriz do pai:

$$M_{world,filho} = M_{group} \times M_{local}$$

Onde $M_{group} = R_y(\theta)$ (rotação em torno de Y).

Isso significa que se o cilindro tem posição local $(0, 0, 0)$, sua posição no mundo é:

$$\mathbf{p}_{world} = R_y(\theta) \times (0, 0, 0, 1)^T = (0, 0, 0, 1)^T$$

O centro não muda (está no eixo de rotação). Mas os vértices na superfície ($r = 2$) giram ao redor do eixo Y.

Se adicionássemos a SpotLight ao Group, ela giraria junto → destruiria o efeito de projetor fixo. Por isso ela está na Scene (nível acima).

---

## 3. Conceitos de Computação Gráfica

### Pré-requisito: O Pipeline de Renderização 3D

Antes de entrar nos conceitos específicos, é importante entender **como o GPU transforma dados 3D em pixels na tela**. O processo tem etapas fixas:

```
Vértices 3D → [Vertex Shader] → Coordenadas de tela → [Rasterização] → Fragmentos → [Fragment Shader] → Pixels
```

1. **Vertex Shader**: recebe cada vértice, aplica transformações (model × view × projection):
   $$\mathbf{p}_{clip} = P \times V \times M \times \mathbf{p}_{local}$$
   - $M$ = model matrix (posição/rotação/escala do objeto no mundo)
   - $V$ = view matrix (posição/orientação da câmera)
   - $P$ = projection matrix (perspectiva)

2. **Rasterização**: determina quais pixels da tela são cobertos por cada triângulo. Interpola atributos (UVs, normais, cores) entre os vértices.

3. **Fragment Shader**: para cada pixel coberto, calcula a cor final. Aqui acontecem:
   - Leitura de texturas (textura do filme, shadow map, light map)
   - Cálculos de iluminação (PBR, sombras, emissive)
   - Projeção do SpotLight.map

**Three.js gera esses shaders automaticamente** baseado no material escolhido (MeshStandardMaterial → shader PBR completo). Não escrevemos GLSL manualmente, mas entendemos o que acontece por baixo.

### 3.0 Fundamentos — Por que matrizes 4×4? (Coordenadas Homogêneas)

Em computação gráfica, usamos **matrizes 4×4** em vez de 3×3. Isso pode parecer estranho para transformar pontos 3D, mas existe uma razão fundamental.

#### O problema das matrizes 3×3

Uma matriz 3×3 pode representar **rotação** e **escala**:

$$\begin{pmatrix} x' \\ y' \\ z' \end{pmatrix} = \begin{pmatrix} a & b & c \\ d & e & f \\ g & h & i \end{pmatrix} \begin{pmatrix} x \\ y \\ z \end{pmatrix}$$

Mas ela **NÃO pode representar translação** (mover um objeto de lugar). A translação é uma soma:

$$\mathbf{p}' = \mathbf{p} + \mathbf{t}$$

Isso significa que, com 3×3, precisaríamos de operações separadas (multiplicação para rotação + soma para translação), impossibilitando compor tudo numa única multiplicação.

#### A solução: coordenadas homogêneas

Adicionamos uma **4ª coordenada** $w$ a cada ponto:

$$\mathbf{p} = \begin{pmatrix} x \\ y \\ z \\ 1 \end{pmatrix}$$

O ponto 3D real é obtido dividindo por $w$: $(x/w,\ y/w,\ z/w)$. Com $w = 1$, é simplesmente $(x, y, z)$.

Agora, com uma **matriz 4×4**, conseguimos representar rotação, escala E translação **numa única multiplicação**:

$$\begin{pmatrix} x' \\ y' \\ z' \\ 1 \end{pmatrix} = \begin{pmatrix} R_{3\times3} & \mathbf{t} \\ 0\;0\;0 & 1 \end{pmatrix} \begin{pmatrix} x \\ y \\ z \\ 1 \end{pmatrix}$$

Onde:
- $R_{3\times3}$ = sub-matriz 3×3 de rotação/escala (canto superior esquerdo)
- $\mathbf{t} = (t_x, t_y, t_z)^T$ = vetor de translação (última coluna)
- Última linha $(0, 0, 0, 1)$ = preserva a coordenada homogênea

#### Por que isso importa?

**Composição de transformações**: podemos combinar várias transformações multiplicando matrizes:

$$M_{total} = T \cdot R \cdot S$$

Uma única multiplicação `M × p` aplica escala, depois rotação, depois translação. Sem coordenadas homogêneas, teríamos que aplicar cada operação separadamente.

**No Three.js**: todo `Object3D` possui uma `matrix` (4×4) que combina `position` (T), `rotation` (R) e `scale` (S):

$$M_{object} = T(position) \cdot R(rotation) \cdot S(scale)$$

#### Vetores vs. Pontos

- **Ponto**: $w = 1$ → afetado por translação
- **Vetor/direção**: $w = 0$ → NÃO afetado por translação (faz sentido: mover um vetor de direção não muda sua direção)

$$\begin{pmatrix} R & \mathbf{t} \\ 0 & 1 \end{pmatrix} \begin{pmatrix} v_x \\ v_y \\ v_z \\ 0 \end{pmatrix} = \begin{pmatrix} R \cdot \mathbf{v} \\ 0 \end{pmatrix}$$

A translação é anulada automaticamente pelo $w = 0$.

---

### 3.1 Matriz de Rotação (Eixo Y)

A rotação do cilindro aplica a matriz:

$$R_y(\theta) = \begin{pmatrix} \cos\theta & 0 & \sin\theta & 0 \\ 0 & 1 & 0 & 0 \\ -\sin\theta & 0 & \cos\theta & 0 \\ 0 & 0 & 0 & 1 \end{pmatrix}$$

#### De onde vem essa matriz?

Rotação em torno do eixo Y significa que a coordenada $y$ **não muda** (o eixo de rotação é fixo). As coordenadas $x$ e $z$ giram num plano:

$$x' = x\cos\theta + z\sin\theta$$
$$z' = -x\sin\theta + z\cos\theta$$
$$y' = y$$

Isso vem da fórmula de rotação 2D aplicada ao plano XZ:

$$\begin{pmatrix} x' \\ z' \end{pmatrix} = \begin{pmatrix} \cos\theta & \sin\theta \\ -\sin\theta & \cos\theta \end{pmatrix} \begin{pmatrix} x \\ z \end{pmatrix}$$

A 4ª linha e coluna existem porque estamos em coordenadas homogêneas (seção 3.0). A rotação pura não translada, então a última coluna é $(0, 0, 0, 1)^T$.

#### Propriedades da matriz de rotação:
- **Ortogonal**: $R^T = R^{-1}$ (inverter = transpor, muito eficiente)
- **Determinante = 1**: preserva volume (não distorce)
- **Preserva comprimentos**: $|\mathbf{p}'| = |\mathbf{p}|$ (isometria)

Para cada vértice do cilindro com posição $\mathbf{p}$, a posição no mundo é:

$$\mathbf{p}' = R_y(\theta) \cdot \mathbf{p}$$

No código: `this._group.rotation.y += ω × Δt`

Three.js converte internamente `rotation.y` nessa matriz 4×4 e a aplica a todos os filhos do Group.

---

### 3.2 UV Mapping Cilíndrico

A `CylinderGeometry` do Three.js gera coordenadas de textura (UVs) automaticamente usando o mapeamento cilíndrico:

$$u = \frac{\theta}{2\pi}, \quad v = \frac{y + h/2}{h}$$

Onde:
- $\theta \in [0, 2\pi)$ é o ângulo ao redor do eixo Y
- $y \in [-h/2, h/2]$ é a altura
- $u \in [0, 1]$ percorre a textura horizontalmente
- $v \in [0, 1]$ percorre verticalmente

**O que são UVs?** São coordenadas 2D que dizem ao GPU "para este vértice 3D, leia a textura nesta posição (u, v)". É como um mapa de instruções para "colar" uma imagem 2D numa superfície 3D.

**Por que "U" e "V"?** Para não confundir com x, y, z (coordenadas espaciais). U = horizontal na textura, V = vertical.

**Resultado**: a strip horizontal (12 frames lado a lado) "enrola" ao redor do cilindro. Cada frame ocupa $1/12$ da circunferência = $30°$ de arco.

---

### 3.3 SpotLight.map — Projeção de Textura

O `SpotLight.map` projeta uma textura na cena como um projetor de slides real.

#### O que é um SpotLight?

Um SpotLight é uma fonte de luz **cônica** — emite luz de um ponto, numa direção, formando um cone:

```
        * (posição da luz)
       /|\
      / | \
     /  |  \    ← cone de luz
    /   |   \
   /    |    \
  ------+------  ← superfície iluminada
```

Parâmetros do cone:
- **angle** ($\alpha$): meia-abertura do cone. No nosso caso, $\pi/6$ (30°) → cone total de 60°.
- **penumbra** (0–1): suavidade na borda do cone (0 = borda dura, 1 = gradiente total).
- **decay**: quão rápido a intensidade diminui com a distância (lei do inverso do quadrado: $I \propto 1/d^2$ quando decay=2).

#### O que SpotLight.map faz?

Quando atribuímos uma textura a `spotLight.map`, o Three.js **projeta essa textura** na cena como se fosse uma imagem num projetor de slides. A textura modula a intensidade/cor da luz.

**Pipeline matemática** (no fragment shader do Three.js):

1. **Coordenada no espaço da luz** — transforma o ponto do mundo para o "olhar" da luz:
$$\mathbf{p}_{light} = M_{shadow} \times \mathbf{p}_{world}$$

   Onde $M_{shadow} = P_{luz} \times V_{luz}$ (projeção × view da câmera interna da luz).

   Isso é exatamente o mesmo conceito da câmera perspectiva (seção 3.6), mas aplicado à LUZ.

2. **Divisão perspectiva** (projeção cônica) — converte para coordenadas 2D normalizadas:
$$u = \frac{p_{light}.x}{p_{light}.w}, \quad v = \frac{p_{light}.y}{p_{light}.w}$$

   Isso cria a projeção cônica: objetos mais distantes da luz recebem uma porção maior da textura (como numa tela de cinema — mais longe = imagem maior).

3. **Amostragem da textura** — lê a cor da textura naquela posição:
$$\text{cor\_projetada} = \text{texture2D}(\text{map}, \text{uv})$$

4. **Modulação** — a cor final da luz naquele ponto é:
$$L_{final} = L_{luz} \times \text{cor\_projetada}$$

   Pixels pretos na textura → sem luz. Pixels brancos → luz plena. É assim que a imagem do cavalo aparece na tela.

**Por que NÃO usamos texture.offset**: os UVs são computados a partir da `shadowMatrix`, não dos UVs da geometria. Qualquer alteração de `offset`/`repeat` na textura é ignorada pelo shader de projeção no WebGL.

**Solução**: CanvasTexture — redesenhamos o conteúdo do canvas a cada frame.

---

### 3.4 CanvasTexture — Textura Dinâmica (Render-to-Texture simplificado)

#### O que é uma textura na GPU?

Uma textura é uma imagem armazenada na memória da GPU (VRAM). O GPU a consulta bilhões de vezes por frame para colorir pixels. Normalmente, uma textura é carregada UMA VEZ e nunca muda.

#### O problema: precisamos mudar a textura a cada frame

No nosso zoetrópio, a porção do filme projetada muda conforme o cilindro gira. Precisamos de uma textura que se atualize dinamicamente.

#### A solução: CanvasTexture

Uma `CanvasTexture` usa um elemento `<canvas>` HTML como fonte de pixels (texels):

1. Criamos um `<canvas>` de 512×512 (invisível — não adicionado ao DOM)
2. Usamos a API Canvas 2D (`ctx.drawImage`) para "pintar" a porção desejada da strip
3. Ao setar `texture.needsUpdate = true`, Three.js chama `gl.texImage2D()` — que faz **upload** dos pixels do canvas para a VRAM

**Processo a cada frame de renderização**:
```
CPU: ctx.drawImage(strip, sx, 0, sw, sh, 0, 0, 512, 512)  → desenha no canvas
CPU→GPU: gl.texImage2D(canvas)                              → upload para VRAM
GPU: fragment shader lê a textura atualizada                → projeta na tela
```

**Custo**: upload de 512×512×4 bytes (RGBA) = 1 MB por frame. Em 60 FPS → 60 MB/s de bandwidth CPU→GPU. Tolerável para texturas pequenas.

**Conceito**: equivale a um **render-to-texture** feito em CPU. Em produção usaríamos FBO (framebuffer object) + shaders, mas CanvasTexture é uma solução elegante e didática.

---

### 3.5 Scroll Contínuo — Projeção Solidária ao Cilindro

**O problema resolvido**: se a projeção fosse estática (um frame fixo na tela), o efeito visual seria de "sombras varrendo uma imagem parada" — incorreto fisicamente.

**Analogia**: imagine um projetor de slides real com o carrossel girando. O slide na tela NÃO fica parado enquanto o carrossel gira — ele se desloca e é substituído pelo próximo.

**Solução**: a porção da strip projetada desloca-se proporcionalmente à rotação do cilindro:

$$\text{offset}_{pixels} = \frac{\theta_{norm}}{2\pi} \times W_{strip}$$

Onde:
- $\theta_{norm} = ((\theta \mod 2\pi) + 2\pi) \mod 2\pi \in [0, 2\pi)$
- $W_{strip}$ = largura total da strip em pixels (3750 px)

#### Por que o módulo duplo?

O operador `%` em JavaScript pode retornar valores negativos (ex: `-5 % 3 = -2`). Para garantir um valor em $[0, 2\pi)$:

$$\theta_{norm} = ((\theta \mod 2\pi) + 2\pi) \mod 2\pi$$

O primeiro `mod` traz para $(-2\pi, 2\pi)$. Somar $2\pi$ garante positivo. O segundo `mod` traz de volta para $[0, 2\pi)$.

#### A "janela" de projeção

A "janela" visível no canvas tem largura = $W_{frame} = W_{strip} / N$:

$$sx = \text{offset} - \frac{W_{frame}}{2}$$

Centramos a janela no ponto da strip que está diretamente na frente da luz. Isso corresponde a ~30° de arco do cilindro (1 frame).

#### Wrapping circular

A strip é circular (frame 12 é vizinho do frame 1, como num cilindro). Se a janela de leitura cruzar a borda da imagem, desenhamos em duas partes:

```
Strip: [===Frame11===|===Frame12===][===Frame1===|===Frame2===]
                              ↑ janela ↑
                         parte1  |  parte2
```

**Resultado**: a imagem projetada se move na mesma velocidade que o filme no cilindro → fisicamente correto. Os separadores pretos na imagem original do Muybridge servem como obturador natural.

---

### 3.6 Câmera Perspectiva

#### O que é uma câmera em 3D?

A câmera define **o que** você vê e **como** o mundo 3D é projetado numa tela 2D. São duas transformações:

1. **View matrix** — move o mundo para que a câmera fique na origem olhando para -Z
2. **Projection matrix** — "achata" o 3D em 2D (cria perspectiva)

```js
PerspectiveCamera(fov=60°, aspect=w/h, near=0.1, far=1000)
position: (6, 3, 2)
lookAt:   (0, 0, -2)
```

#### FOV (Field of View)

Ângulo vertical de visão. 60° é similar ao olho humano. Menor = mais zoom. Maior = mais fish-eye.

#### Near e Far (Planos de corte)

Objetos mais perto que `near` ou mais longe que `far` são **descartados** (clipped). Isso define o **frustum** — a pirâmide truncada que delimita o volume visível:

```
       near
       /  \
      / vis \    ← frustum (volume visível)
     / ível  \
    /________\
        far
```

#### Matriz de projeção perspectiva

$$P = \begin{pmatrix} \frac{1}{a \cdot \tan(fov/2)} & 0 & 0 & 0 \\ 0 & \frac{1}{\tan(fov/2)} & 0 & 0 \\ 0 & 0 & \frac{-(f+n)}{f-n} & \frac{-2fn}{f-n} \\ 0 & 0 & -1 & 0 \end{pmatrix}$$

Onde $a$ = aspect ratio, $n$ = near, $f$ = far.

**Nota**: a última linha é $(0, 0, -1, 0)$ — isso faz $w' = -z$. Após a divisão perspectiva ($x'/w', y'/w'$), objetos mais distantes (z maior) ficam menores na tela. É isso que cria a **perspectiva** (trilhos de trem convergindo no horizonte).

#### Matriz de view (lookAt)

Transforma coordenadas do mundo para o espaço da câmera. Construída a partir de 3 vetores ortonormais:

- $\hat{f} = \text{normalize}(\text{target} - \text{eye})$ — para onde a câmera olha (forward)
- $\hat{r} = \text{normalize}(\hat{f} \times \text{up})$ — eixo horizontal da câmera (right)
- $\hat{u} = \hat{r} \times \hat{f}$ — eixo vertical real da câmera (up)

$$V = \begin{pmatrix} r_x & r_y & r_z & -\mathbf{r} \cdot \mathbf{eye} \\ u_x & u_y & u_z & -\mathbf{u} \cdot \mathbf{eye} \\ -f_x & -f_y & -f_z & \mathbf{f} \cdot \mathbf{eye} \\ 0 & 0 & 0 & 1 \end{pmatrix}$$

A parte 3×3 é a rotação (alinha os eixos do mundo com os eixos da câmera). A última coluna é a translação (move a origem para a posição da câmera). Juntas: "colocam a câmera na origem olhando para -Z".

---

### 3.7 Shadow Mapping

#### O que é um shadow map?

Na vida real, sombras acontecem quando um objeto bloqueia a luz. O GPU não simula fisicamente fótons — ele usa um truque chamado **shadow mapping**.

**Ideia central**: "Se eu estivesse NA POSIÇÃO DA LUZ olhando para a cena, o que eu veria?" Tudo que a luz "vê" está iluminado. Tudo que ela NÃO vê (obstruído) está na sombra.

Habilitado via `renderer.shadowMap.enabled = true` + `PCFSoftShadowMap`.

**Algoritmo em 2 passos**:

1. **Shadow pass** (1º render — do ponto de vista da LUZ):
   - Renderiza a cena usando a câmera interna da SpotLight
   - NÃO gera imagem colorida — apenas grava a **profundidade** (distância) de cada pixel
   - O resultado é o **shadow map**: uma textura de profundidades (depth buffer)

2. **Render pass** (2º render — do ponto de vista da CÂMERA):
   - Para cada pixel visível, projeta sua posição 3D no espaço da luz:
     $$\mathbf{p}_{luz} = M_{shadow} \times \mathbf{p}_{world}$$
   - Compara a profundidade do pixel com o valor armazenado no shadow map:
     - Se $z_{frag} > z_{shadow\_map} + \text{bias}$ → algo está entre esse ponto e a luz → **SOMBRA**
     - Caso contrário → caminho livre → **ILUMINADO**

```
    Luz ──────── Shadow Map (profundidades)
     │
     │  Objeto A (z=3)  ← gravado no shadow map
     │
     │  Objeto B (z=7)  ← z_frag(7) > z_shadow(3) → SOMBRA!
```

#### Shadow Acne e Bias

Devido à resolução finita do shadow map, um objeto pode "fazer sombra em si mesmo" (artefato). O **bias** desloca ligeiramente a comparação de profundidade para evitar isso:

$$\text{em\_sombra} = (z_{frag} > z_{shadow} + \text{bias})$$

`shadow.bias = -0.003` no nosso código.

#### PCF (Percentage-Closer Filtering)

Sem filtro, as sombras têm bordas pixeladas (cada texel do shadow map = sombra/não-sombra binário). O PCF amostra **vários texels vizinhos** no shadow map e calcula uma média → bordas suaves (soft shadows).

`PCFSoftShadowMap` = PCF com kernel maior → sombras ainda mais suaves.

**Configuração**:
- `shadow.mapSize = 2048×2048` → resolução do depth buffer (mais = bordas mais finas)
- `shadow.camera.near/far` → define o frustum da câmera de sombra

---

### 3.8 CylinderGeometry — Construção Paramétrica

#### O que é uma geometria paramétrica?

Em vez de modelar manualmente cada vértice, definimos a superfície por **equações paramétricas** e o computador gera os vértices.

Para um cilindro, os parâmetros são:
- $\theta \in [0, 2\pi)$ — ângulo ao redor do eixo
- $t \in [0, 1]$ — posição vertical (0 = base, 1 = topo)

#### Equações

Para cada vértice $(i, j)$ nos $N_{radial}$ segmentos radiais e $N_{height}$ de altura:

$$\theta = \frac{2\pi \cdot i}{N_{radial}}$$
$$x = R \cos\theta$$
$$z = R \sin\theta$$
$$y = -\frac{h}{2} + h \cdot \frac{j}{N_{height}}$$

Isso são **coordenadas cilíndricas → cartesianas**:

$$(R, \theta, y) \rightarrow (R\cos\theta,\ y,\ R\sin\theta)$$

#### Parâmetros no código

```js
CylinderGeometry(
  CYLINDER_RADIUS,  // R = 2.0
  CYLINDER_RADIUS,  // mesmo raio (top = bottom → cilindro, não cone)
  HEIGHT,           // h = 2.5
  64,               // 64 segmentos radiais (suavidade da curva)
  1,                // 1 segmento de altura (suficiente para cilindro reto)
  true              // openEnded = sem tampas (a luz precisa sair de dentro!)
)
```

Com 64 segmentos radiais, cada "fatia" cobre $360°/64 = 5.6°$ — suficiente para parecer uma curva suave.

---

### 3.9 Material PBR (MeshStandardMaterial)

#### O que é PBR?

**Physically Based Rendering** = modelo de iluminação que segue as leis da física (conservação de energia, reciprocidade de Helmholtz). Resultados mais realistas que modelos antigos (Phong/Lambert).

#### A equação de renderização (simplificada)

$$L_{out}(\omega_o) = L_{emissive} + \int_\Omega f_r(\omega_i, \omega_o) \cdot L_i(\omega_i) \cdot \cos\theta_i \, d\omega_i$$

Em português:
- $L_{out}$ = luz que sai do ponto (o que a câmera "vê")
- $L_{emissive}$ = luz emitida pelo próprio material (brilha sozinho)
- $\int_\Omega$ = soma de contribuições de todas as direções de luz
- $f_r$ = **BRDF** (Bidirectional Reflectance Distribution Function) — "como o material reflete"
- $L_i$ = luz chegando de uma direção
- $\cos\theta_i$ = lei de Lambert (superfícies inclinadas recebem menos luz)

#### BRDF Cook-Torrance (usada pelo MeshStandardMaterial)

$$f_r = \frac{D \cdot F \cdot G}{4 \cdot (\omega_o \cdot n) \cdot (\omega_i \cdot n)}$$

Onde:
- $D$ = **Distribuição Normal** (GGX): "quantos micro-facets apontam na direção certa?" → controlado por **roughness**
- $F$ = **Fresnel** (Schlick): "quanto reflete vs. absorve dependendo do ângulo?" → controlado por **metalness**
- $G$ = **Geometria** (Smith): "quantos micro-facets estão obstruídos por vizinhos?"

#### Parâmetros no nosso cilindro

| Parâmetro | Valor | Efeito |
|-----------|-------|--------|
| `roughness` | 0.3 | Superfície semi-lisa (reflexo parcial, como plástico polido) |
| `metalness` | 0.0 | Dielétrico (não-metálico) — reflete igual em todas as cores |
| `emissive` | 0xffffff | Cor da auto-emissão (branco) |
| `emissiveMap` | textura | Mapa que define ONDE emite (onde há imagem → brilha) |
| `emissiveIntensity` | 0.4 | Força da emissão (simula retroiluminação) |
| `opacity` | 0.85 | Semi-transparente (vê-se levemente "através") |

O emissive faz o filme parecer "iluminado por trás" — como transparências num projetor antigo. Esse canal **ignora sombras**, então o filme brilha mesmo na sombra.

---

### 3.10 Delta Time — Independência de Frame Rate

#### O problema

Suponha que a rotação seja `rotation.y += 0.01` a cada frame:
- Monitor 60 Hz: 0.01 × 60 = 0.6 rad/s
- Monitor 144 Hz: 0.01 × 144 = 1.44 rad/s ← **2.4× mais rápido!**

A animação correria em velocidades diferentes dependendo do hardware.

#### A solução: integração temporal

Em vez de avançar um valor fixo por frame, avançamos proporcional ao **tempo decorrido**:

$$\theta(t + \Delta t) = \theta(t) + \omega \cdot \Delta t$$

Onde:
- $\omega$ = velocidade angular (rad/s) — constante independente do hardware
- $\Delta t$ = tempo entre frames (em segundos) — varia conforme o hardware

Exemplo:
- 60 Hz: $\Delta t = 1/60 = 0.0167s$ → $\theta += \omega \times 0.0167$
- 144 Hz: $\Delta t = 1/144 = 0.0069s$ → $\theta += \omega \times 0.0069$

Em ambos os casos, após 1 segundo: $\theta_{total} = \omega \times 1.0$ ← **idêntico!**

Isso é o **método de Euler explícito** (a integração numérica mais simples).

#### LERP exponencial (suavização de velocidade)

Para que o cilindro não passe instantaneamente de 0 a 15 rad/s (seria brusco), usamos um filtro:

$$\omega_{novo} = \omega_{atual} + (\omega_{alvo} - \omega_{atual}) \times \alpha$$

Com $\alpha = 0.05$:

- A cada frame, a velocidade se aproxima 5% do alvo
- Resultado: transição suave com "inércia" (como um motor acelerando)
- Matematicamente: decaimento exponencial → nunca chega exatamente no alvo, mas fica indistinguível após ~60 frames

---

## 4. Fluxo de Execução

```
main.js → new App(container)
  → new SceneManager()          [cria Scene + AmbientLight]
  → new CameraManager()         [cria PerspectiveCamera]
  → new RendererManager()       [cria WebGLRenderer + shadows]
  → new ZoetropeAnimation(scene)
       → carrega muybridge_classic.jpg
       → _buildFilmCylinder()   [CylinderGeometry + textura]
       → _buildBase()           [Circle + Ring]
       → _buildProjectorLight() [SpotLight + CanvasTexture]
       → _buildScreen()         [PlaneGeometry branca]
  → App.start()
       → requestAnimationFrame loop:
            delta = (t - t_prev) / 1000
            zoetrope.update(time, delta)
              → lê teclado (ArrowUp/Down)
              → atualiza velocidade (LERP)
              → rotaciona group: group.rotation.y += ω×Δt
              → _drawProjection() [scroll contínuo]
              → canvasTexture.needsUpdate = true
            renderer.render(scene, camera)
```

---

## 5. Controles

| Tecla | Ação |
|-------|------|
| `↑` (ArrowUp) | Acelera rotação (+3 rad/s²) |
| `↓` (ArrowDown) | Desacelera (−3 rad/s²) até parar |

Velocidade máxima: 15 rad/s (~2.4 rotações/segundo).

---

## 6. Assets

| Arquivo | Descrição |
|---------|-----------|
| `public/textures/muybridge_classic.jpg` | Strip 3750×200 px, 12 frames, "The Horse in Motion" (Sallie Gardner, 1878) |

A imagem original é uma placa fotográfica com 12 quadros do galope de um cavalo — o primeiro "filme" da história. Os separadores pretos entre quadros são da placa original e servem como obturador natural na projeção.

---

## 7. Dependências

| Pacote | Versão | Uso |
|--------|--------|-----|
| three | 0.176.0 | Biblioteca 3D (WebGL) |
| vite | 6.3.5 | Build tool / dev server |

---

## 8. Conceitos-Chave para a Defesa Oral

1. **Grafo de cena hierárquico**: transformações se propagam pai→filho via multiplicação de matrizes.
2. **UV Mapping cilíndrico**: coordenadas paramétricas (θ, y) → (u, v).
3. **SpotLight.map**: projeção cônica de textura usando a shadowMatrix da luz.
4. **CanvasTexture + needsUpdate**: render-to-texture em CPU; controle frame-a-frame.
5. **Scroll contínuo**: offset = (θ/2π) × largura → projeção solidária ao cilindro.
6. **Shadow mapping**: depth pass da luz + comparação z → determinação sombra/luz.
7. **PCFSoftShadowMap**: filtragem do shadow map para bordas suaves.
8. **PBR (Cook-Torrance)**: modelo fisicamente plausível com roughness/metalness.
9. **Delta time**: integração temporal para independência de frame rate.
10. **Emissive map**: simulação de material retroiluminado sem receber sombras.
