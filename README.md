# Golf Project – Simulation 3D

Projet réalisé dans le cadre du cours **Informatique Graphique 3D (CSC_43043_EP)**.  
Auteur : **Pierre Bernadet**

## 🎮 Description

Ce projet consiste en la création d’un **jeu de golf interactif en 3D**.  
Il combine un intérêt personnel pour le golf avec des techniques d’**informatique graphique** : génération procédurale de terrain, rendu temps réel, simulation physique et interface utilisateur.

L’objectif principal est de simuler le mouvement d’une balle de golf sur un parcours généré, avec gestion des collisions, frottements, rebonds et conditions de victoire.

---

## 🌍 Environnement 3D

- **Terrain texturé** avec fonction de hauteur basée sur :
  - combinaisons de gaussiennes,
  - bruit de Perlin atténué par une fonction de lissage cosinus,
  - fonction *smoothstep* pour transitions douces.
- **Green** (zone plane autour du trou).
- **Surface d’eau** animée (vagues par bruit de Perlin + transparence).
- **Skybox panoramique** mappée sur un cube.
- **Végétation** : herbe (billboards) + arbres importés en `.obj`.
- **Drapeau animé** par un shader sinusoïdal (effet de vent).
- **Trou** matérialisé par un disque noir au centre du green.

---

## ⚙️ Simulation physique

- **Intégration du mouvement** avec accélération gravitationnelle.  
- **Collisions terrain/balle** avec correction de position et rebonds.  
- **Frottements** différents selon terrain ou green.  
- **Conditions spéciales** :
  - Balle hors-terrain → réinitialisée.
  - Entrée dans le trou → victoire détectée.  
- **Interface de tir** :
  - Flèche directionnelle colorée selon la puissance.
  - Contrôles :  
    - `A` / `D` → ajuster l’angle horizontal  
    - `W` / `S` → ajuster l’angle vertical  
    - `Q` / `E` → modifier la puissance  
    - `Espace` → tirer  

---

## ➕ Extensions

- Trois clubs disponibles : **fer 7**, **wedge**, **putter** (vitesses et angles différents).  
- Caméras : **vue libre** ou **suivi automatique de la balle**.  
- Interface temps réel via **ImGui** (nombre de coups).  
- Message de félicitations en cas de réussite.  
- Tentative (non finalisée) d’effets visuels : éclaboussure dans l’eau, impact au sol.

---

## 📹 Démonstration

Une vidéo de démonstration est disponible dans le dépôt (gérée avec **Git LFS** car >100 Mo).

---

## 🚀 Lancer le projet

### Prérequis
- OpenGL  
- ImGui  
- Bibliothèques standards C++ (GLFW/GLUT, GLM, stb_image, etc.)  

### Compilation & exécution
```bash
# Exemple (adapter selon ton système / IDE)
mkdir build && cd build
cmake ..
make
./golf_project
