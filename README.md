
# ENGLISH Below

Le but de ce projet est de pouvoir mesurer à distance la temperature dans un champ pour prévenir du gel de manière entierement auto-hébergé. Réalisé à partir de la carte de développement TTGO Lora32 1.6 V2_1 et d'une sonde de température SHT40

Pour cela il est décomposé en plusieurs parties :
- Le capteur, sur batterie et a disatnce
- La passerelle, pour recevoir les information du capteur et les envoyer en IP
- Le serveur, pour recevoir, traiter, stoquer et présenter les relevés

La carte de dévellopement choisi est la LoRa32 V2.1_1.6 (https://lilygo.cc/products/lora3) pour son intégration de tous les éléments nécéssaires et son prix correct. En effets, celle-ci integre un ESP32 comme microcontroller, un circuit de charge et regulation de batterie, un ecran, et surtout un module LORA pour la liaison longue distance.
Il en faudra au minimum 2 cartes, une en tant que capteur et une en tant que passerelle

# La passerelle

Le role de la passerelle est de recevoir les signaux Lora envoyés par le (ou les) capteurs et envoyé les valeurs via wifi / ip vers le serveur en MQTT.

La passerelle, du fait qu'elle est en ecoute constante, doit etre alimenté en continue. Je n'ai pas essayé de la mettre sur batterie, mais je ne pense pas qu'une autonomie correct soit envisageable.

La passerelle utilisé lors dans ce projet est la carte LoRa32 V2.1_1.6 avec le systeme OpenMQTTGateway.
Toutes les informations sont disponible sur la page du projet (https://docs.openmqttgateway.com/), et l'instalaltion peut se faire directement en ligne sur cette page : https://docs.openmqttgateway.com/upload/web-install.html

# Le capteur

Le role du capteur est de faire la mesure là où elle est nécéssaires. La raison à l'origine du projet est la mesure de températeure dans les vignes pour avertir du risque de gel (température < 3°c) lors de la sortie des bourgeons pour déployer des solutions antigels.

Il est également basé sur la carte TTGO Lora32 1.6 V2_1, d'une sonde de température SHT40 étanche et une cellule 18650.
Le caddie de courses : 
Lora32 : https://fr.aliexpress.com/item/32872078587.html
SHT40 : https://fr.aliexpress.com/item/1005004718790450.html
Pour la cellule, j'ai utilisé une récupéré dans une batterie de PC précédement. Choisissez en fonction de vos besoin / avez deja

Le code est disponible ci-dessus, fichier .ino a utiliser avec arduino IDE. Il est nécéssaire d'ajouter la carte dans le gestionnaire de carte. Pour cela, suivre ce tuto 

###tuto

il vous faudra egalement installer les bibliotheques suivantes via le gestionnaire de bibliotheque : 
L'écran : Adafruit SSD1306
Le moteur Graphique : Adafruit GFX
Lora : LoRa by Sandeep Mistry
Le capteur : Adafruit SHT4x

Bien penser à sélectionner la carte et surtout la révision de la carte 
Board > TTGO LoRa32-OLED
Board Revision > TTGO Lora32 V2.1 (1.6.1)

Le capteur doit etre raccordé comme suivant : 
Rouge > 3.3v
Noire > GND
Bleu >
Jaune > 

# serveur

Pour le serveur, il y a 3 parties :
- Le broker MQTT pour l'échange des relevés
- Le node red pour recupérer les relever, les mettre en forme, et les envoyer dans la base de donnée InfluxDB. Il est possible d'utiliser Telegraf, mais je n'ai jamais réussi
- La base de donnée InfluxDB pour stoquer dans les temps les relevés
- Le Grafana pour la présentation des résultats
- (en option), What's APP api pour l'envoie automatique d'alertes.
