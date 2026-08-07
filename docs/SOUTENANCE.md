# Scénario de soutenance

Objectif : montrer d’abord le **moteur**, puis le **jeu** qui l’utilise.

Préparer : build Debug, lancer `Game.exe`, clavier AZERTY ou QWERTY OK.

---

## 0. Intro (1 min)

- Deux binaires : `Engine.dll` + `Game.exe` (séparation exigée TRON).
- Stack : C++17, Direct3D 11, CMake, pas de middleware tiers.
- Doc : `ARCHITECTURE.md` (couches) + ce scénario.

---

## 1. Moteur — boucle & états (2 min)

1. Lancer → **Splash** automatique → **Menu**.
2. Expliquer `StateManager` (ChangeState différé, Enter/Exit/Update).
3. Clic **JOUER** → `GameState` reconstruit le `World` (ECS).

Points à citer : `Application::Update` (input → UI → states → physics → late scripts → particles → flush destroy → render + post-process).

---

## 2. Moteur — ECS & scripts (2 min)

- Entités + composants : `Transform`, `Mesh`, `Collider`, `Tag`, `ParticleEmitter`.
- Scripts gameplay dans **Game** uniquement (`ShipControllerScript`, `EnemyScript`, `ProjectileScript`).
- Cycle `Start` / `Update` / `LateUpdate` / `OnCollisionEnter` (style MonoBehaviour).

Montrer un tir : spawn runtime d’entité projectile (mesh + collider trigger + script).

---

## 3. Moteur — rendu & post-process (2 min)

- Mesh cube procédural + `Basic.hlsl` (tint).
- Caméra FPS liée au vaisseau.
- Touches **1 / 2 / 3** : sat / désat / néon (`PostProcess.hlsl`).
- HUD `DRAW` / `CULL` : frustum culling (reculer / regarder le vide → CULL monte).

---

## 4. Moteur — physique & particules (2 min)

- Colliders Sphere / AABB, résolution de pénétration, triggers.
- Partition spatiale (broadphase) avant tests fins.
- Impacts → burst particules + ralenti (`Time::TriggerSlowMotion`).
- Touche **T** : time scale manuel (scripts / particules en temps scaled).

---

## 5. Jeu — boucle de démo (4 min)

1. Arène procédurale (murs, damier, obstacles).
2. Ennemis magenta qui chase ; tir LMB ; look RMB.
3. HUD : score / HP / kills / timer.
4. Objectif : **6 kills** ou **score 80** → écran **VICTORY**.
5. Sinon HP 0 / timer → **GAME OVER**.
6. **SPACE** ou bouton **MENU** → rejouer sans relancer l’exe (utile en soutenance).

---

## 6. Clôture (1–2 min)

Récap checklist M1–M14 / J1–J7 (cochée dans `PLAN_IMPLEMENTATION.md`).

Hors scope volontaire (PDF « pour aller plus loin ») : deferred shading, replay, etc.

Questions du jury : pointer le code Engine vs Game, CMake `SHARED` + copy shaders, absence de logique métier dans la DLL.

---

## Aide mémoire contrôles

| Touche           | Effet                    |
| ---------------- | ------------------------ |
| ZQSD / WASD      | Move                     |
| RMB              | Look                     |
| LMB              | Fire                     |
| 1 2 3            | Post-process             |
| T                | Time scale               |
| Échap / P        | Fin de partie → GameOver |
| SPACE (GameOver) | Retour Menu              |
