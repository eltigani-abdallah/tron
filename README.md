# TRON

Moteur de jeu 3D (**C++17**, **Direct3D 11**) compilé en **DLL**, plus un jeu de démonstration
**First-Person Spaceship Shooter**.

| | |
|---|---|
| Engine | `Engine.dll` — ECS, rendu, physique, input, UI, particules, post-process, time scale |
| Game | `Game.exe` — splash/menu, niveau procédural, ennemis, score, win/lose |

Documentation : [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) · [`docs/PLAN_IMPLEMENTATION.md`](docs/PLAN_IMPLEMENTATION.md) · [`docs/SOUTENANCE.md`](docs/SOUTENANCE.md)

---

## Prérequis

- Windows 10/11 x64
- CMake ≥ 3.20
- Visual Studio **2019** ou **2022** (charge de travail *Développement Desktop en C++*)
- Windows SDK (Direct3D 11)

---

## Build

```powershell
cd <chemin>\Tron2
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
```

Visual Studio 2019 :

```powershell
cmake -S . -B build -G "Visual Studio 16 2019" -A x64
cmake --build build --config Debug
```

Release :

```powershell
cmake --build build --config Release
```

Si CMake se plaint du générateur (ex. cache MinGW), supprimer le dossier `build/` puis relancer.

Arrêter `Game.exe` avant de recompiler si le linker indique un fichier verrouillé.

---

## Lancer

```powershell
.\build\bin\Debug\Game.exe
```

Release : `.\build\bin\Release\Game.exe`

`Engine.dll` et le dossier `shaders/` sont placés **à côté** de `Game.exe` (POST_BUILD CMake).
Le répertoire de travail du débogueur Visual Studio est déjà configuré sur ce dossier de sortie.

---

## Contrôles

| Action | Touches |
|---|---|
| Déplacement | ZQSD / WASD / flèches |
| Monter / descendre | E · Espace / Ctrl · C |
| Look | Souris **droite** maintenue |
| Tir | Clic **gauche** |
| Post-process | `1` normal · `2` désat · `3` néon |
| Ralenti | `T` (toggle) · auto sur impacts |
| Pause / quitter partie | Échap / P |
| Menu (après partie) | Espace ou bouton MENU |

### Challenge

- **Victoire** : 6 kills **ou** score ≥ 80
- **Défaite** : HP ≤ 0 **ou** timer 90 s

---

## Structure

```
Tron2/
├── Engine/          # DLL (API publique sous include/Engine/)
├── Game/            # EXE + scripts / états gameplay
├── docs/            # architecture, plan, scénario soutenance
├── CMakeLists.txt
└── README.md
```

**Règle** : aucune logique métier shooter dans `Engine/` — le jeu hérite de `Script` / `State` et compose les composants.

---

## Fonctionnalités démontrées (checklist)

**Moteur (M1–M14)** : meshes procéduraux, shader HLSL, ECS + scripts, composants, state machine,
caméra, input, particules, partition spatiale, frustum culling, time scale, math (vec/mat/quat),
classes/templates, DLL.

**Jeu (J1–J7)** : niveau procédural, ennemis runtime, obstacles/cibles, post-process
saturation/luminosité/contraste, splash+menu, score HUD, début/fin + challenge.

Compteurs live in-game : `DRAW` / `CULL` (frustum culling).

---

## Tests

Suite unitaire légère (sans Direct3D), cible `TronTests` :

```powershell
cmake --build build --config Debug --target TronTests
.\build\bin\Debug\TronTests.exe
```

Couverture actuelle : math (`Vec3` / angles), `Time`, partition spatiale, ECS `World`, règles `GameSession` (win/lose).

---

## Soutenance

Voir le scénario ~15 min : [`docs/SOUTENANCE.md`](docs/SOUTENANCE.md).
