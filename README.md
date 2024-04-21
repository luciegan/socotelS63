## Préambule
Tout d'abord, cette réalisation s'inspire largement des recherches et de la réalisation de [ThomasChappe](https://github.com/ThomasChappe/S63_Arduino) qui s'inspire lui-même du prototype de [Cyril Jovet](https://github.com/sun-exploit) qui s'est sans doute inspiré du code de [revolunet](https://github.com/revolunet/s63). Un grand grand merci à eux !
La solution utilisée pour faire sonner le téléphone est celle de [Alastair Aitchison](https://www.youtube.com/watch?v=cZ2rHqBXO1s), une réelle source d'inspiration !
Les connaissances et les conseils avisés de Raphaël Durand ont été précieux pour franchir les obstacles de mon ignorance.
Pour le code, de longs échanges avec chatGPT ont été nécessaires.

Pour info, je suis débutante dans ce domaine et je n'ai pas de formation particulière ni en programmation, ni en électronique, ni en électricité. Je partage cette expérience en ne garantissant aucunement que ce soit la méthode la plus simple ou la plus optimisée. Mais ça fonctionne et c'est déjà pas si mal. 

## Synthétisation

Le téléphone est configuré pour enregistrer et lire des messages vocaux. Lorsqu'une personne passe devant le téléphone, celui-ci détecte sa présence et déclenche la sonnerie. Lorsque la personne décroche le combiné, elle est accueillie par un message pré-enregistré qui lui expose les différentes options disponibles. Ces options sont sélectionnables à l'aide d'un cadran rotatif.

- **Détection de la présence avec un capteur ultrason :** Un capteur ultrasonique détecte si une personne passe devant le téléphone à moins d'une certaine distance.
- **Activation de la sonnerie du téléphone :** Une fois qu'une personne est détectée, le téléphone commence à sonner pour attirer l'attention.
- **Message d'introduction lors du décrochage du combiné :** Lorsque le combiné est décroché, un message d'introduction est automatiquement lancé pour guider l'utilisateur ou l'utilisatrice.
- **Choix multiples avec le cadran rotatif :** Le téléphone propose plusieurs options à l'utilisateur ou l'utilisatrice via un cadran rotatif :
  - Option 1 : Écouter un ancien message de manière aléatoire.
  - Option 2 : Laisser un nouveau message.
- **Action en fonction du choix de l'utilisateur ou de l'utilisatrice :**
  - Si l'utilisateur choisit l'option 1, le téléphone sélectionne aléatoirement un ancien message à écouter.
  - Si l'utilisateur choisit l'option 2, il est invité à enregistrer un nouveau message.
- **Retour au message d'introduction :** Après que l'utilisateur ou l'utilisatrice ait effectué son choix et accompli l'action correspondante, le téléphone revient au message d'introduction pour une nouvelle interaction.

## Matériel
Voici la liste du matériel utilisé en plus des fils électriques :

**éléments de base :**
- **un téléphone à cadran SOCOTEL S63**
- **un arduino Uno**
- des fils pour les branchements

**Pour le capteur :**
- **un capteur ultrason HC-SR04**

Pour l'écoute et l'enregistrement des messages
- **un [DFRobot Voice Recorder V1](https://wiki.dfrobot.com/Voice_Recorder_Module_Breakout_SKU_DFR0745) ou DFS1101S**
- **2 condensateurs 100nF**
- des résistances pour arriver autour de 680 ohm

**Pour la sonnerie :**
- **un mini L293D**
- une pile 9V

### Le SOCOTEL S63
Je ne suis pas une spécialiste alors je vais résumer brièvement ce que j'ai compris du fonctionnement du Socotel.
Il ne faut absolument pas prendre ce que je dis comme une source fiable !!! Je reste ouverte à plus d'explications et corrections !
Pour simplifier la compréhension pour la suite, j'ai ajouté la lettre S avant les numéros des broches du Socotel.
J'ai également essayé de mettre les couleurs qui correspondent au fils.
Vous pouvez cliquer sur les images pour les agrandir.
Vous remarquerez que j'ai enlevé la jointure métallique entre S11 et S17.


<img src="https://github.com/users/luciegantois/projects/1/assets/165783651/5da8a779-1179-4590-ace8-6a6cc0c0e1f3" width="100"/>

<br>
<img src="https://github.com/users/luciegantois/projects/1/assets/165783651/1c914f69-87ef-4087-84ee-4c7aab5919b2"  width="100"/>
<br>
<br>
**Le combiné**
Le décrochage/raccrochage se réalise par le biais d'un interrupteur entre S7 et S11.
**Les écouteurs**
Les écouteurs principaux du combiné sont connectés respectivement en S3 et en S5.
Les écouteurs supplémentaires sont connectés en S2 et S4.
**Le micro**
Les fils du micro sont reliés en S1 et S7
**Le cadran**
La SI transmet les impulsions.
Les SII et SIII agissent comme un interrupteur pour détecter si le cadran est en mouvement ou non.
La SIV, j'ai pas bien compris à quoi elle servait, je crois que c'est pour l'alimentation du cadran.
**L'alimentation**
Si j'ai bien compris, elle se fait en S11 et S13
**La sonnerie** 
C'est assez flou pour moi mais je crois qu'elle se fait en aussi en S11 et S13 mais qu'on peut en rajouter une en S15 et S17.

Pour le projet, nous n'utiliserons pas toutes les broches et on détournera certaines d'entre elles de leur fonction initiale.


### Le [DFRobot Voice Recorder V1](https://wiki.dfrobot.com/Voice_Recorder_Module_Breakout_SKU_DFR0745)

C'est un module qui permet d'enregistrer, stocker et lire les audios en MP3.
Il a l'énorme avantage de pouvoir ajouter un micro externe.
La capacité de mémoire est de 15Mo ce qui permet de stocker environ 1000 audio de 5 secondes ou 40 minutes d'enregistrement.
Je ne l'ai pas du tout trouvé intuitif en ce qui concerne le codage !

### mini L293D

J'ai fait des tas de recherches sur le fonctionnement de la sonnerie du Socotel S63... Je n'ai rien trouvé de clair et précis qui ne trouvait pas son contraire ailleurs. Jusqu'au jour ou j'ai vu le tuto très clair de [Alastair Aitchison](https://www.youtube.com/watch?v=cZ2rHqBXO1s) ! Je vous invite à le visionner pour mieux comprendre le fonctionnement des vieux téléphones. Les sous-titres générés automatiquement en français sont bien fait.

Finalement, on va utiliser ce module pour inverser les courants dans les broches S15 et S17 du socotel.

## Les branchements

**ATTENTION : il faut enlever la partie métallique qui relie la broche S11 et S17**

DFRobot | ARDUINO | SOCOTEL | CAPTEUR ULTRASON | MINI L293D
-- | -- | -- | -- | --
VCC | 5V | - | VCC |  VCC
GND | GND | S7 ET SII | GND | GND
-| 2 | SI | - | -
-| 4 | SIII | - | -
-| 5 | S11 | - | -
-| 6 | - | - | IN1
-| 7 | - | - | IN2
RX | 10 | - | - | -
TX | 11 | - | - | -
-| 12 | - | ECHO | -
-| 13 | - | TRIG | -
-| - | S15 | - | A-
-| - | S17 | - | A+
SPK- | - | S3 | - | -
SPK+ | - | S5 | - | -

Voici un schéma où les proportions ont disparu où alors, les piles 9V règneraient sur le monde !


![Image](https://github.com/users/luciegantois/projects/1/assets/165783651/c71ae247-e507-4fc2-87ac-1f2664c0bf47)


## Le code

Il est ici : [code socotel S63](https://github.com/luciegan/socotelS63.git)

C'est à peu près tout pour le moment, je continue à bosser dessus alors les choses risquent de bouger.
