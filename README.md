# TP IoT 2022 - LoRa & MQTT  
Dorian LE QUERE et Amandine PAILLAT

## 1. Mise en place  

### 1.1 Installation de l'environnement  
Nous avons commencé par installer ESP-IDF et les dépendances nécessaires, puis nous avons configuré notre ESP32 pour qu'il puisse se connecter au Wi-Fi de notre téléphone.  

- Installation et configuration de ESP-IDF :  
  [https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32/get-started/index.html](https://docs.espressif.com/projects/esp-idf/en/v5.4/esp32/get-started/index.html)  

- Documentation de la carte :  
  [http://www.smartcomputerlab.org/](http://www.smartcomputerlab.org/)  

### 1.2 Connexion au Wi-Fi  
- Configuration de l'ESP32 pour se connecter au point d’accès Wi-Fi (iPhone de Dorian).   

### 1.3 Envoi d'un message MQTT sur le broker `test.mosquitto.org`  
- Connexion au broker MQTT.  
- Publication d’un message "POTATOTO" sur le topic "SALAD". 

---

## 2. Communication MQTT  

### 2.1 Définition des paramètres  
Avant de commencer la communication, nous avons défini les paramètres suivants pour notre groupe :  
- Topic MQTT : `SALAD`  
- Mot de passe : `POTATOTO`  

### 2.2 Envoi et réception des messages MQTT  
Groupe 1 (notre groupe)
- Nous avons modifié le script app_main.c pour publier "POTATOTO" sur MQTT.     

Résultat : Notre ESP32 a bien transmis un message MQTT.

À travers ce TP, nous avons compris que MQTT est un protocole simple et efficace pour envoyer des messages entre plusieurs appareils connectés. Il fonctionne avec un système de publication et d’abonnement, ce qui permet de partager des informations sans connexion directe entre les appareils. Nous avons vu qu’il est léger et adapté aux objets connectés, notamment pour transmettre des données rapidement et avec peu de ressources réseau.

---

## 3. Communication LoRa  

### 3.1 Configuration des paramètres LoRa  
Nous avons ajouté la communication LoRa en utilisant la librairie esp-idf-sx127x.  

- Configuration des PIN LoRa :
MISO -> 19
NSS  -> 18
RST  -> 14
MOSI -> 27

Configuration de la fréquence : 
- `Menuconfig -> Lora configuration`  
- `Frequence to Use -> Other -> 868MHz`  

### 3.2 Réception et filtrage des messages LoRa  
Groupe 1 (notre groupe) 
- Réception LoRa :  
- Ajout de la fonction `task_rx()` pour écouter les messages LoRa.
- Affichage du message reçu.
- Vérification que le message reçu correspond bien à "POTATOTO" avant de l'afficher, mais il est en commentaire pour un test. 

Résultat : Nous avons pu publier un message MQTT et recevoir en réponse un message LoRa.  

En travaillant avec LoRa, nous avons découvert qu’il permet d’envoyer des messages sur de longues distances avec très peu d’énergie. Contrairement à MQTT qui passe par Internet, LoRa fonctionne avec des ondes radio, ce qui le rend pratique pour des capteurs éloignés. Nous avons appris à configurer ses paramètres et à recevoir des messages en filtrant ceux qui nous intéressaient. Cela nous a permis de voir comment LoRa et MQTT peuvent être combinés pour relayer des données efficacement. 

## 4. I2C 

I2C est un protocole de communication qui permet à plusieurs composants électroniques d’échanger des informations en utilisant seulement deux fils : un pour les données (SDA) et un pour l’horloge (SCL). Un appareil maître, comme un microcontrôleur, envoie des instructions à des appareils esclaves, qui possèdent chacun une adresse unique. L’horloge permet de synchroniser l’échange des données. Ce protocole est utilisé pour connecter facilement plusieurs composants, comme des capteurs ou des écrans, tout en minimisant le nombre de fils nécessaires.
