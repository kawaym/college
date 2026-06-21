# Sprint 2 — Evento Cinemático (CinematicEvent)

## Conceito

Quando o zoetrópio atinge velocidade máxima, a câmera faz um LERP suave
em direção à **parede de projeção**. A câmera "mergulha" na projeção 2D
do Muybridge, e ao "entrar" na cena projetada, o Sprint 3 revela o cavalo
3D — a animação 2D "ganha vida".

```
Câmera (8, 5, 6)  ───LERP───►  Frente da parede (0, 4, -9.5)
     olhando o zoetrópio          olhando a projeção de perto
                                  → "entra" na cena → 3D (Sprint 3)
```

## Máquina de Estados Finitos

```
IDLE ──(speed ≥ 95% max)──► APPROACHING ──(t=1)──► INSIDE
```

| Estado | Descrição |
|--------|-----------|
| `IDLE` | Câmera parada na posição inicial. Usuário controla zoetrópio (↑/↓). |
| `APPROACHING` | Velocidade máxima atingida. Controles desativados. Câmera LERP em direção à parede. |
| `INSIDE` | Câmera na projeção. Zoetrópio some (`visible=false`). Cena 3D aparece (Sprint 3). |

## Arquivo a Criar

`src/animations/CinematicEvent.js`

## Conceitos CG Envolvidos

### LERP (Interpolação Linear)
$$\vec{P}(t) = \vec{P}_0 \cdot (1-t) + \vec{P}_1 \cdot t, \quad t \in [0,1]$$

- `P₀ = (8, 5, 6)` — posição inicial da câmera
- `P₁ = (0, 4, -9.5)` — frente da parede de projeção
- Three.js: `camera.position.lerpVectors(start, end, t)`

### Easing (smoothstep)
$$s(t) = 3t^2 - 2t^3$$

Suaviza início e fim do movimento. Evita "arranque" e "freada" bruscas.

### View Matrix (lookAt dinâmico)
Durante o LERP, o `lookAt` transiciona do zoetrópio para a parede:
$$\vec{L}(t) = \text{lerp}(\vec{L}_{zoetrope}, \vec{L}_{wall}, t)$$

- `L_zoetrope = (0, 3, -2)` — olhando zoetrópio (posição atual)
- `L_wall = (0, 4, -10)` — olhando a parede de projeção

### Máquina de Estados Finitos
Controle determinístico de fluxo de animação. Cada estado tem:
- Condição de entrada
- Comportamento (update)
- Condição de saída (transição)

## Parâmetros

| Parâmetro | Valor |
|-----------|-------|
| Trigger | `speedNormalized ≥ 0.95` |
| Duração do LERP | ~3 segundos |
| Posição inicial câmera | `(8, 5, 6)` |
| Posição final câmera | `(0, 4, -9.5)` |
| lookAt inicial | `(0, 3, -2)` |
| lookAt final | `(0, 4, -10)` |

## Integração

### Getters já disponíveis em ZoetropeAnimation:
- `getSpeedNormalized()` — velocidade atual / máxima
- `getSpeed()` / `getMaxSpeed()`
- `setControlsEnabled(bool)` — desativa controles do usuário
- `getGroup()` — referência ao grupo (para `visible = false`)

### Modificações necessárias:
- `App.js` — instanciar CinematicEvent, passar câmera + zoetrópio, chamar update()
- `CameraManager.js` — expor câmera (já faz via `get()`)

## Transição para Sprint 3

Ao atingir estado `INSIDE`:
1. `zoetropeGroup.visible = false`
2. Revelar modelo GLTF do cavalo (GLTFLoader + AnimationMixer)
3. SpotLight dramático focado no modelo
4. OrbitControls para inspeção
