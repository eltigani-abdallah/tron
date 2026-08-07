# Architecture — Projet TRON

Document d’architecture du moteur de jeu et du jeu de démonstration.  
Aligné sur les sujets *TRON* et *Lancement de projet – Tron*.  
Stack : **C++**, **Direct3D 11**, **CMake**, séparation **Engine (DLL)** / **Game (EXE)**.

---

## 1. Objectif

Fournir un **moteur de jeu** compilé en **librairie dynamique** (`.dll`), exposant types et systèmes de base, et un **jeu** First-Person Spaceship Shooter qui utilise exclusivement ce moteur pour démontrer ses fonctionnalités.

Le moteur fournit les objets, types et fonctions de base.  
Le jeu les utilise (création d’entités, attachement de composants/scripts, logique de gameplay).

---

## 2. Vue d’ensemble

```
┌─────────────────────────────────────────────────────────────┐
│  Game (EXE)                                                 │
│  main.cpp → Run()                                           │
│  Scripts gameplay, niveau procédural, score, états UI       │
└───────────────────────────┬─────────────────────────────────┘
                            │ lie dynamiquement
                            ▼
┌─────────────────────────────────────────────────────────────┐
│  Engine (DLL)                                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │ Cœur / Loop  │  │ ECS          │  │ State Machine    │  │
│  │ ScriptManager│  │ Entity/Comp  │  │ Splash/Menu/…    │  │
│  └──────────────┘  └──────────────┘  └──────────────────┘  │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │ Rendu D3D11  │  │ Physique     │  │ Input            │  │
│  │ Shaders/PP   │  │ Collisions   │  │ Clavier/Souris   │  │
│  └──────────────┘  └──────────────┘  └──────────────────┘  │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │ Caméra       │  │ Particules   │  │ Temps / UI       │  │
│  └──────────────┘  └──────────────┘  └──────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
                   GPU (Direct3D 11)
```

---

## 3. Séparation Engine / Jeu

| Responsabilité Engine (DLL) | Responsabilité Game (EXE) |
|---|---|
| Lecture entrées clavier/souris | Script qui pilote le vaisseau |
| Chargement / création de meshes | Interface affichant le score |
| Détection de collisions | Événement de fin de niveau |
| Pipeline de rendu (projection, shaders) | Contenu du menu / splash |
| Gestion UI générique (boutons, texte) | Forme et layout du HUD |
| Mouvement générique d’un objet | Règles de score, spawn, win/lose |
| Cycle de vie scripts (Start/Update) | Classes héritant de Script |

**Règle** : aucune logique métier spécifique au shooter dans la DLL. Le jeu ne réimplémente pas le rendu ni l’ECS.

---

## 4. Structure des dépôts / dossiers

```
Tron2/
├── docs/
│   ├── ARCHITECTURE.md
│   ├── PLAN_IMPLEMENTATION.md
│   └── SOUTENANCE.md
├── Engine/                          # librairie dynamique
│   ├── include/Engine/              # API publique (.hpp)
│   │   ├── Core/
│   │   ├── Math/
│   │   ├── ECS/
│   │   ├── Render/
│   │   ├── Physics/
│   │   ├── Input/
│   │   ├── Camera/
│   │   ├── Particles/
│   │   ├── UI/
│   │   ├── Time/
│   │   └── States/
│   ├── src/                         # implémentations (.cpp)
│   └── shaders/                     # HLSL
├── Game/                            # exécutable
│   ├── include/
│   ├── src/
│   │   └── main.cpp                 # point d’entrée obligatoire
│   └── ...
├── CMakeLists.txt
└── README.md
```

Conventions :
- Chaque module public : paire **`.hpp` / `.cpp`**.
- Headers publics sous `Engine/include/Engine/`.
- Export DLL via macro `ENGINE_API` (`dllexport` / `dllimport`).
- Pas de bibliothèque tierce hors **Windows SDK / Direct3D 11**.

---

## 5. Cœur du moteur

### 5.1 Application / Game Loop

Classe centrale (ex. `Application` ou `Engine`) :

1. Initialisation des systèmes (fenêtre, D3D11, ECS, scripts, physique, audio éventuel, UI, temps).
2. Entrée dans la **game loop**.
3. À chaque frame :
   - mise à jour du temps (delta, time scale) ;
   - lecture input ;
   - mise à jour machine à états ;
   - `ScriptManager` → `Update` des scripts ;
   - systèmes (physique/collisions, particules, etc.) ;
   - culling ;
   - rendu (meshes, particules, UI, post-process) ;
   - présentation (Present).

Point d’entrée côté jeu : `main` crée la scène / les scripts, puis appelle `Engine::Run()` (ou équivalent).

### 5.2 ScriptManager et cycle de vie (inspiré Unity)

- Classe de base moteur : `Script` (équivalent MonoBehaviour).
- Méthodes : `Start()`, `Update(float dt)`, éventuellement `OnCollisionEnter(...)`.
- Le développeur du jeu crée des classes héritant de `Script`, les attache à des entités.
- Le moteur reconnaît les scripts attachés et appelle leurs fonctions au bon moment.
- Chaque script a son propre cycle de vie ; l’engine gère l’exécution.

Flux typique :
1. Créer une `Entity` (GameObject).
2. Créer / attacher un `Script` (composant).
3. Appeler `Run` sur le moteur.
4. Le moteur initialise les systèmes puis entre en boucle.

---

## 6. ECS (Entity Component System)

Inspiration Unity, responsabilités séparées :

| Concept | Rôle |
|---|---|
| **Entity** | Identifiant unique, sans logique |
| **Component** | Données uniquement (Transform, Mesh, Collider, UI, ParticleEmitter, Script…) |
| **System** | Logique (RenderSystem, PhysicsSystem, ParticleSystem, ScriptSystem…) |

### 6.1 Composants prévus (exigés par le sujet)

- `TransformComponent` — position, rotation, scale (via classe `Transform` : vecteurs, matrices, quaternions).
- `MeshComponent` — mesh procédural ou buffer vertex/index.
- `ColliderComponent` — collisions simples (AABB / sphère).
- `UIComponent` — éléments d’interface (texte, bouton…).
- `ParticleEmitterComponent` — émetteur de particules.
- `Script` / composant gameplay — code utilisateur (Start/Update).
- Composants utilitaires 3C : caméra, contrôleur (données), etc.

### 6.2 Avantages retenus

Réutilisable, flexible, testable, séparation claire des responsabilités.

---

## 7. Machine à états

Organise les phases du jeu. Chaque état expose :

- `Enter()`
- `Update(float dt)`
- `Exit()`

États attendus pour le jeu de démo :

- `SplashState` ou `MenuState`
- `GameState` (partie en cours)
- `PauseState` (optionnel mais naturel)
- `GameOverState` / état de fin (victoire ou défaite)

Un `StateManager` (manager) détient l’état courant et gère les transitions.

---

## 8. Moteur de rendu (Direct3D 11)

### 8.1 Rôle

Convertir une scène 3D en image 2D à l’écran via le pipeline GPU.

Pipeline simplifié :
1. **Vertex Shader** — transformation des sommets.
2. **Rasterizer** — fragments.
3. **Pixel Shader** — couleur.
4. **Framebuffer** — image finale.

### 8.2 Meshes

- Création **procédurale** (et/ou chargement statique si pertinent).
- Buffers Vertex / Index gérés manuellement (API D3D11).

### 8.3 Shaders

- Shader basique obligatoire pour afficher avec Direct3D.
- Compilation manuelle des shaders HLSL.
- Post-processing (côté jeu / pipeline moteur) : **saturation**, **luminosité**, **contraste** du rendu final.

### 8.4 Caméra (3C)

- Caméra libre ou attachée à une entité.
- Pilotée via `Transform` (position, rotation).
- Matrice **View** + matrice **Projection** (perspective : FOV, near/far, ratio).

### 8.5 Culling

Méthode pour ne pas dessiner (ou traiter) les objets non visibles à l’écran (frustum culling).

---

## 9. Physique / collisions

- Collisions **simples** (pas de moteur physique externe).
- Intégration ECS : `ColliderComponent` + système de détection.
- Callbacks possibles vers scripts (`OnCollisionEnter`).
- **Partition spatiale** pour optimiser les tests (ex. grille uniforme ou octree léger) — exigence listée dans le sujet TRON.

Les 3C Character (déplacement, gravité éventuelle, collisions) s’appuient sur ces composants.

---

## 10. Input (3C Control)

- `InputManager` : lecture clavier / souris.
- Actions typiques : déplacement (ZQSD / flèches), tir (clic), visée (souris).
- Déclenche les actions du personnage ou de l’UI, sans connaître le gameplay précis.

---

## 11. Particules

Système léger :

- Particule : position, vitesse, durée de vie.
- `ParticleEmitterComponent`.
- Effets : explosion, tir, étincelles…
- Rendu : **billboards** (face caméra) ou petits meshes.

---

## 12. Temps

Système flexible de gestion du temps :

- `deltaTime` réel vs `deltaTime` mis à l’échelle.
- Facteur **time scale** pour produire des **ralentis in-game**.

---

## 13. UI

- Composants / système UI dans le moteur (texte, boutons, sliders au besoin).
- Le jeu définit le contenu : score visible pendant la partie, menus, splash.

---

## 14. Mathématiques

Module `Math` interne (sans lib externe) :

- Vecteurs (`Vec3`, éventuellement `Vec2`/`Vec4`).
- Matrices 4×4.
- Quaternions.
- Classe `Transform` utilisée par le composant Transform et la caméra.

Usage de **classes** et de **templates** là où c’est pertinent (conteneurs ECS, helpers mathématiques, etc.).

---

## 15. Threading (architecture simplifiée du sujet)

Le sujet illustre :

- Thread principal — logique / cœur.
- Thread graphique — partie graphique (optionnel selon implémentation).

Implémentation initiale possible en **single-thread** correct et stable ; séparation logique des modules déjà prête pour un éventuel thread graphique ultérieur, sans sortir du cadre du sujet.

---

## 16. Architecture du jeu de démonstration

Genre : **First-Person Spaceship Shooter** 3D temps réel.

Le jeu doit démontrer le moteur :

| Exigence PDF | Réalisation côté Game |
|---|---|
| Niveau complet procédural | Génération de l’arène / couloir / obstacles |
| Apparition d’ennemis | Scripts + spawn via ECS |
| Obstacles à viser | Entités avec mesh + collider |
| Gameplay début / fin | State Machine + conditions win/lose |
| Splash ou Menu | `SplashState` / `MenuState` |
| Score visible | UI + script de score |
| Post-process sat/lum/contraste | Paramètres branchés sur le pipeline moteur |
| Challenge satisfaisant | Règles de difficulté / objectifs définis dans le jeu |

---

## 17. Frontière des modules (dépendances)

```
Game
 └── Engine (API publique)

Engine interne (dépendances descendantes typiques) :
  Core → ECS, States, Time, Input
  Render → Math, ECS (lecture Mesh/Transform/Camera), Shaders
  Physics → Math, ECS (Colliders), SpatialPartition
  Particles → Math, ECS, Render
  UI → Render, Input
  Scripts → ECS, Time, Physics (événements)
```

Interdiction : le module Game n’inclut pas les `.cpp` internes du moteur ; uniquement les headers publics + linkage DLL.

---

## 18. Build

- **CMake** racine.
- Cible `Engine` : `SHARED` → `Engine.dll` (+ import lib).
- Cible `Game` : exécutable lié à `Engine`, contenant `main.cpp`.
- Shaders HLSL copiés / chargés au runtime.
- Plateforme cible : Windows (Direct3D 11).

---

## 19. Hors périmètre (section « Pour aller plus loin » des PDF)

Non inclus dans l’architecture de base tant que non demandés explicitement :

- Deferred shading (spot light)
- Mode replay
- Système de matériaux / textures avancé
- IA poussée
- Optimisations / features supplémentaires hors liste des résultats attendus

---

## 20. Attendus organisationnels (contexte sujet)

- Projet de groupe (quatre).
- Dépôt GitHub public : `https://github.com/prenom-nom/tron`.
- Soutenance ~15 min : démo du jeu + démonstration du moteur.
- Qualité de code et conventions de groupe prises en compte.

---

*Document de référence architecture — ne remplace pas le plan d’implémentation détaillé (`PLAN_IMPLEMENTATION.md`).*
