
# ENGLISH Below

Le but de ce projet est de pouvoir mesurer à distance la temperature dans un champ pour prévenir du gel de manière entierement auto-hébergé. 

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
 



