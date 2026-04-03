# Projet d'horloge binaire

### Qu'est-ce qu'une horloge binaire ?

Avant de commencer la démarche de la construction de l'horloge binaire, il faut tout d'abord comprendre le fonctionnement d'une horloge binaire. L'horloge binaire fonctionne à l'instar d'une horloge digitale classique, mais affiche les nombres correspondants en binaire et pas avec des chiffres. On utilisera ici 16 DEL pour indiquer les heures, minutes et secondes : 4 pour les heures (allant de 0 à 15) et 6 pour les minutes et secondes (de 0 à 63). L'horloge elle-même sera contrôlée avec le microcontrolleur Arduino Nano. 

Pour cette configuration, les composants requis sont alors :
- 1 carte Arduino ou autre microcontrolleur type Arduino 
- 16 DEL (j'ai utilisé trois couleurs différentes)
- Un bouton-poussoir pour allumer et éteindre les DEL
- Un potentiomètre pour varier la luminosité

### Les étapes de la construction de l'horloge

#### 1. Le fonctionnement général

Tout d'abord, j'ai effectué un plan de fonctionnement général où :
1. L'Arduino Nano garde compte du temps avec l'oscillateur internal 
→ *remarque : il est mieux d'utiliser dans ce cas un composant RTS tel qu'un DS1307 ou DS3231, car sinon le temps est entièrement mis à 0 lorsque le microcontrolleur perd l'alimentation. J'avais essayé de le construire avec un de ces composants, mais le mien ne fonctionnait pas quoi qu'on fasse ...*
2. Lorsqu'une seconde passe, l'Arduino appelle une fonction `incrémenter(void)` qui met à jour un tableau de bits correspondant à la matrice des DEL
3. À chaque instant, le tableau de la matrice est transmis aux DEL dans la fonction `loop()` pour afficher le temps en binaire. 
4. L'Arduino réceptionne aussi des potentiels signaux du bouton-interrupteur (qui éteint les DEL en gardant en marche l'Arduino) et du potentiomètre qui sert comme variateur de luminosité.

Ici, on utilise une matrice de DEL au lieu de les connecter individuellement. Puisqu'on en a 16 à garder allumé, et qu'on a 12 broches chez l'Arduino, le multiplexage est la seule façon de réaliser ce projet.

Au lieu de connecter chaque DEL en série avec une broche de l'Arduino, une résistance et la terre, on classe chacune dans une colonne (COL) et une ligne (ROW), où la ligne 
Bien qu'il est impossible de les garder toutes allumées à la fois, l'Arduino peut circuler très rapidement de ligne en ligne, qui sera perçu comme si les DEL sont toutes allumées. 
Pour ce circuit, j'ai organisé la matrice en ordre croissant des nombres binaires:
![](/assets/figure1.png)

#### 2. Le circuit

Avec les composants requis et le fonctionnement d'une matrice, j'ai établi un circuit attaché dans ce projet en PDF.
Les broches `{3, 5, 9, 10}`, comme broches PWM (*pulse-width modulation*) de l'Arduino, seront utilisées comme les broches de ligne (connectées aux cathodes), et les broches `{2, 4, 7, 8}` (connectées à des bandes conductrices sur la plaque d'essai et celles-ci aux anodes des DEL). Les broches PWM permettent de simuler des signaux analogiques (qui varient d'intensité) et ainsi varier l'intensité des DEL. Le bouton et le potentiomètre seront connectés aux broches analogiques A6 et A5, qui permettent de transmettre des signaux analogiques, tels que ceux d'un potentiomètre, au système. 
![](/assets/figure2.png)

#### 3. Prototype et expérimentation

Avant de construire et souder la plaque finale, j'ai d'abord testé une petite version de la matrice DEL sur une autre plaque à essai.
![](/assets/figure3.png)
Une matrice comme celle-ci fonctionne en allumant et éteignant très rapidement (à des intervalles de 2 ms) les lignes (controllant les cathodes) et puis les colonnes (controlant les anodes). Par exemple, pour une matrice 4x4, si je veux allumer simultanément les DEL situées aux positions `(2,2)`, `(3, 2)` et `(3, 4)` :
- J'envoie un signal haut à la ligne 2 d'abord et un signal bas à la colonne 2, allumant la DEL (2,2) ;
- J'envoie un signal bas à la ligne 2, éteignant la DEL, et simultanément j'envoie un signal haut à la ligne 3 et un signal bas à la colonne 2 ;
- Cette action se repète à une fréquence de 1 KHz, et les DELs apparaissent allumées constamment.
Remarque : si on n'éteint pas la DEL (2,2), alors la DEL (2,4) s'allumera dans ce cas car les colonnes 2 et 4 sont envoyées un signal bas.

 

#### 4. Réalisation de la plaque

Pour cette partie, j'ai utilisé une plaque d'essai/prototypage avec des bandes de cuivre auxquelles je peux souder des composants que je veux connecter ensemble. J'ai décidé d'utiliser des bandes séparées perpendiculaires aux autres pour les colonnes, alors que les lignes seraient connectées avec des câbles aux broches de l'Arduino.


#### 5. Codage du microcontrolleur

Le code du microcontrolleur est disponible dans les fichiers du projet.

#### 6. Boitier et dernières retouches


- 
