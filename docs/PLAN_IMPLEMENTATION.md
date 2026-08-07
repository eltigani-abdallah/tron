# Plan d’implémentation — Projet TRON

Plan détaillé correspondant aux exigences des PDF *TRON* et *Lancement de projet – Tron*.  
Décisions validées : **Direct3D 11**, **CMake**, périmètre **strictement aligné sur les PDF**.

Ce document décrit **quoi faire**, **dans quel ordre**, et **comment vérifier** chaque exigence.  
L’architecture cible est décrite dans `ARCHITECTURE.md`.

---

## 0. Principes de mise en œuvre

1. Séparer **Engine (DLL)** et **Game (EXE)** dès le premier commit de code.
2. Chaque module : fichiers **`.hpp` / `.cpp`** distincts.
3. Point d’entrée obligatoire : `Game/src/main.cpp`.
4. Rendu **3D** obligatoire via Direct3D 11.
5. Pas de bibliothèque externe hors Windows SDK / Direct3D 11.
6. Priorité PDF : **moins de features, mais stables** ; qualité de code.
7. Focaliser d’abord les fonctionnalités les plus importantes (ordre des phases ci-dessous).

---

## 1. Matrice d’exigences → livrable

### 1.1 Moteur (résultats attendus)

| # | Exigence PDF | Module / livrable | Critère de done |
|---|---|---|---|
| M1 | Rendu simple de meshes procéduraux (Direct3D) | `Render` + meshes procéduraux | Mesh visible à l’écran |
| M2 | Shader basique | HLSL VS/PS compilés | Objet coloré / transformé correctement |
| M3 | Système d’entités et de composants (ECS) | `ECS` | Créer Entity + attacher Components |
| M4 | Composants positions/rotations, meshes, collisions, UI, effets, gameplay | Components listés | Chaque type utilisé par un System ou Script |
| M5 | Manager et machine à états | `StateManager` + States | Transitions Splash/Menu ↔ Game ↔ Fin |
| M6 | Caméras | `Camera` + matrices View/Proj | Vue perspective correcte |
| M7 | Contrôleur clavier/souris | `InputManager` | Déplacement / tir / visée réactifs |
| M8 | Système de particules | `ParticleEmitter` + rendu billboard | Effet tir / explosion visible |
| M9 | Partition spatiale (optimiser collisions) | Grille / structure spatiale | Requêtes voisins utilisées par Physics |
| M10 | Clipper objets non visibles (culling) | Frustum culling | Objets hors frustum non dessinés |
| M11 | Temps flexible / ralentis in-game | `Time` + time scale | Ralenti observable en jeu |
| M12 | Transform (vecteurs, matrices, quaternions) | `Math` + `Transform` | Positions/rotations cohérentes |
| M13 | Classes et templates | Ensemble du code | Templates utilisés (ex. ECS/containers) |
| M14 | Moteur en librairie dynamique | CMake `SHARED` | `Engine.dll` chargée par le jeu |

### 1.2 Jeu (démonstration)

| # | Exigence PDF | Livrable Game | Critère de done |
|---|---|---|---|
| J1 | Niveau complet procédural | Générateur de niveau | Niveau jouable généré au runtime |
| J2 | Apparition d’ennemis | Spawn + scripts | Ennemis apparaissent en partie |
| J3 | Obstacles à viser (ennemis possibles) | Entités ciblables | Hits détectés / score ou destruction |
| J4 | Shader post-process sat / lum / contraste | Pass post-process | Paramètres visibles sur le rendu final |
| J5 | Splash ou Menu en plus du jeu | États dédiés | Écran avant la partie |
| J6 | Score visible pendant la partie | UI texte | Score mis à jour à l’écran |
| J7 | Gameplay début / fin + challenge | Win/Lose + règles | Partie démarre et se termine clairement |

### 1.3 Hors scope (PDF « Pour aller plus loin »)

Ne pas implémenter dans le plan de base :

- Deferred shading (spot light)
- Mode replay
- Matériaux / textures avancés
- IA basique poussée
- Features supplémentaires non listées dans les résultats attendus

---

## 2. Structure de fichiers cible (implémentation)

```
Tron2/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   └── PLAN_IMPLEMENTATION.md
├── Engine/
│   ├── CMakeLists.txt
│   ├── include/Engine/
│   │   ├── Export.hpp
│   │   ├── Core/Application.hpp
│   │   ├── Core/Script.hpp
│   │   ├── Core/ScriptManager.hpp
│   │   ├── Math/Vec3.hpp
│   │   ├── Math/Mat4.hpp
│   │   ├── Math/Quaternion.hpp
│   │   ├── Math/Transform.hpp
│   │   ├── ECS/Entity.hpp
│   │   ├── ECS/Component.hpp
│   │   ├── ECS/World.hpp
│   │   ├── Render/Renderer.hpp
│   │   ├── Render/Mesh.hpp
│   │   ├── Render/Shader.hpp
│   │   ├── Render/Camera.hpp
│   │   ├── Render/FrustumCulling.hpp
│   │   ├── Physics/Collider.hpp
│   │   ├── Physics/PhysicsSystem.hpp
│   │   ├── Physics/SpatialPartition.hpp
│   │   ├── Input/InputManager.hpp
│   │   ├── Particles/ParticleSystem.hpp
│   │   ├── UI/UISystem.hpp
│   │   ├── Time/Time.hpp
│   │   └── States/StateManager.hpp
│   ├── src/                    # miroir .cpp de chaque .hpp
│   └── shaders/
│       ├── Basic.hlsl
│       └── PostProcess.hlsl
└── Game/
    ├── CMakeLists.txt
    ├── include/                # scripts / états du jeu
    └── src/
        └── main.cpp
```

Noms exacts ajustables tant que la séparation modules / hpp-cpp / main / DLL reste respectée.

---

## 3. Phases d’implémentation

Chaque phase se termine par un **critère de validation** avant de passer à la suivante.

---

### Phase A — Socle projet & DLL

**Objectif** : dépôt compilable, séparation Engine/Game.

Tâches :
- CMake racine + sous-projets `Engine` (SHARED) et `Game` (EXE).
- Macro d’export `ENGINE_API`.
- Fenêtre Win32 minimale.
- `main.cpp` appelle une API moteur (`Run` / `Initialize`).
- Clear screen D3D11 (device, context, swapchain, render target).

**Validation** : l’EXE démarre, charge la DLL, fenêtre noire/colorée clearée.

**Exigences couvertes** : M14 (partiel), base M1.

---

### Phase B — Math & Transform

**Objectif** : fondations mathématiques du sujet.

Tâches :
- `Vec3`, `Mat4`, `Quaternion` (classes ; templates si pertinent).
- `Transform` : position, rotation, scale → matrices monde.
- Tests manuels simples (produits, look-at, etc.).

**Validation** : transformations composables utilisées par la caméra ensuite.

**Exigences** : M12, M13 (amorcé).

---

### Phase C — Rendu meshes + shader basique + caméra

**Objectif** : premier objet 3D à l’écran.

Tâches :
- Mesh procédural (cube / vaisseau simplifié / primitives).
- Vertex buffer / index buffer D3D11.
- Shader HLSL basique (VS + PS).
- Caméra perspective (View + Projection, FOV, near/far).
- Boucle de rendu d’au moins un mesh avec Transform.

**Validation** : mesh 3D visible, caméra orientable.

**Exigences** : M1, M2, M6.

---

### Phase D — ECS + Scripts (Start/Update)

**Objectif** : modèle Entity/Component/System + scripts utilisateur.

Tâches :
- `Entity` (ID), registre de composants, `World`.
- Composants : Transform, Mesh (liaison rendu).
- `Script` de base + `ScriptManager` (Start puis Update chaque frame).
- Le jeu crée une entité, attache un script dérivé.

**Validation** : script utilisateur exécuté chaque frame via le moteur.

**Exigences** : M3, M4 (partiel), architecture scripts du sujet.

---

### Phase E — Game loop complète + State Machine + Manager

**Objectif** : cycle de vie du jeu structuré.

Tâches :
- Boucle : Time → Input → State → Scripts → Systems → Render.
- `StateManager` + états : Splash/Menu, Game, GameOver (Pause optionnel).
- Transitions propres Enter/Update/Exit.

**Validation** : démarrage sur Splash/Menu, passage en Game, fin vers GameOver.

**Exigences** : M5 ; amorce J5, J7.

---

### Phase F — Input clavier/souris (3C Control)

**Objectif** : contrôles joueur.

Tâches :
- `InputManager` (états touches, souris, deltas).
- Binding actions : déplacement, tir, visée.
- Script Character/Controller côté Game utilisant l’input moteur.

**Validation** : vaisseau contrôlable en première personne.

**Exigences** : M7 ; 3C Control/Character du sujet.

---

### Phase G — Collisions simples + partition spatiale

**Objectif** : physique légère exigée.

Tâches :
- `ColliderComponent` (AABB et/ou sphère).
- Détection collisions + notification scripts (`OnCollisionEnter` si prévu).
- Structure de **partition spatiale** (grille uniforme recommandée pour un shooter) pour limiter les paires testées.

**Validation** : collisions joueur/obstacle/projectile ; partition utilisée (pas un O(n²) naïf seul).

**Exigences** : M4 (collisions), M9.

---

### Phase H — Culling

**Objectif** : ne pas traiter/dessiner le hors-écran.

Tâches :
- Extraction frustum depuis View/Projection.
- Test AABB/sphère vs frustum avant draw (et éventuellement avant logique lourde).

**Validation** : objets hors champ exclus du rendu (preuve par compteur draw calls ou debug).

**Exigences** : M10.

---

### Phase I — Particules

**Objectif** : effets dynamiques.

Tâches :
- Données particule : position, vitesse, lifetime.
- `ParticleEmitterComponent` + système Update/Render.
- Rendu billboard face caméra.
- Effets : tir, explosion/étincelles.

**Validation** : effet visible lors d’un tir ou d’une destruction.

**Exigences** : M8, M4 (effets).

---

### Phase J — UI + Score

**Objectif** : interface et score in-game.

Tâches :
- Système UI moteur (texte, bouton minimal pour menu).
- Affichage score pendant `GameState`.
- Scripts Game mettent à jour le score (hits, kills, etc.).

**Validation** : score visible et mis à jour en partie ; menu utilisable.

**Exigences** : M4 (UI), J5, J6.

---

### Phase K — Post-processing

**Objectif** : shader final sat / lum / contraste.

Tâches :
- Rendu scène vers texture (render target).
- Pass plein écran HLSL : saturation, luminosité, contraste.
- Paramètres exposés (constantes / API moteur) contrôlables pour la démo.

**Validation** : modification visible du rendu final en changeant les paramètres.

**Exigences** : J4.

---

### Phase L — Time scale / ralentis

**Objectif** : gestion flexible du temps.

Tâches :
- Horloge moteur : `realDelta`, `scaledDelta`, `timeScale`.
- Scripts / physique / particules consomment le temps mis à l’échelle.
- Déclenchement d’un ralenti (ex. impact, événement de fin).

**Validation** : ralenti observable sans casser la logique de fin de partie.

**Exigences** : M11.

---

### Phase M — Jeu complet (démonstration)

**Objectif** : prototype First-Person Spaceship Shooter satisfaisant les points « Votre jeu ».

Tâches :
- Génération **procédurale** d’un niveau complet (géométrie + obstacles).
- Apparition d’ennemis (spawn runtime).
- Obstacles / ennemis ciblables (projectiles + collisions).
- Début et fin de partie (objectifs, GameOver / victoire).
- Challenge : densité d’ennemis, score cible, survie, etc. (défini dans le Game uniquement).
- Brancher Splash/Menu, score, post-process, particules, contrôles.

**Validation checklist jeu** :
- [x] Niveau procédural jouable (J1)
- [x] Ennemis apparaissent (J2)
- [x] Cibles / obstacles (J3)
- [x] Post-process actif (J4)
- [x] Splash ou Menu (J5)
- [x] Score à l’écran (J6)
- [x] Début + fin + challenge (J7)

**Exigences** : J1–J7 + démonstration de M1–M14.

---

### Phase N — Intégration, polish minimal, préparation soutenance

**Objectif** : stabilité et démonstration.

Tâches :
- [x] Corriger bugs bloquants (jitter titre, spawn safe, delta après load).
- [x] Vérifier chargement DLL (chemin de sortie CMake + shaders POST_BUILD).
- [x] Scénario de démo 15 min : `docs/SOUTENANCE.md`.
- [x] README : build CMake, lancement, structure, contrôles.

**Validation** : build clean, démo rejouable sans crash (GameOver → Menu).

---

## 4. Ordre de priorité (si retard)

Conformément au PDF (*focalisez-vous sur les fonctionnalités les plus importantes*) :

1. DLL + D3D11 + mesh + shader + caméra  
2. ECS + scripts + game loop + états  
3. Input + collisions  
4. Jeu procédural + ennemis + score + splash/menu  
5. Particules + post-process  
6. Partition spatiale + culling + time scale  

Ne pas démarrer les items « Pour aller plus loin » avant la checklist M/J complète.

---

## 5. Conventions de code (attendus qualité)

- Langue des identifiants : à fixer par le groupe (recommandation : anglais pour le code, français pour la doc).
- Un concept = une paire hpp/cpp.
- Pas de logique gameplay shooter dans `Engine/`.
- Pas de dépendance circulaire Game → Engine internals.
- Nommage cohérent des Components (`XxxComponent`) et Systems (`XxxSystem`).

---

## 6. Build & livraison

| Élément | Attendu |
|---|---|
| Build | CMake ≥ 3.20, compilateur MSVC, Windows |
| Sorties | `Engine.dll`, `Game.exe`, shaders accessibles |
| Repo | GitHub public `https://github.com/prenom-nom/tron` |
| Soutenance | Démo jeu + démonstration moteur (~15 min) |

---

## 7. Suivi d’avancement

Cocher au fur et à mesure :

**Moteur**
- [x] M1 Meshes procéduraux D3D11
- [x] M2 Shader basique
- [x] M3 ECS
- [x] M4 Composants (transform, mesh, collision, UI, effets, gameplay)
- [x] M5 Manager + state machine
- [x] M6 Caméras
- [x] M7 Input clavier/souris
- [x] M8 Particules
- [x] M9 Partition spatiale
- [x] M10 Culling
- [x] M11 Time scale / ralentis
- [x] M12 Transform (vec/mat/quat)
- [x] M13 Classes + templates
- [x] M14 DLL

**Jeu**
- [x] J1 Niveau procédural
- [x] J2 Ennemis
- [x] J3 Obstacles / cibles
- [x] J4 Post-process sat/lum/contraste
- [x] J5 Splash ou Menu
- [x] J6 Score visible
- [x] J7 Début / fin / challenge

---

## 8. Statut

Phases **A → N** livrées. Scénario soutenance : `docs/SOUTENANCE.md`.

*Fin du plan d’implémentation — aligné sur les PDF TRON / Lancement de projet.*
