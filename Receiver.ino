#include <SoftwareSerial.h>
const int TAILLE_TRAME = 19; //Taille des trames en octets
const int TAILLE_REPONSE = 11;
const int ACK = 33; //On envoit "!" si la trame reçue est correcte.
const int NACK = 63; //On envoie "?" si la trame reçue est incorrecte.

void setup() {
  Serial.begin(9600);
}

int TrameRecue[TAILLE_TRAME]; //Tableau de 19 cases remplies à chaque nouvelle trame reçue
int TrameReponse[TAILLE_REPONSE] = {37, 38, 68, 68, 65, 80, 42, -1, 64, 58, 64}; //Tableau de 11 cases pour la trame réponse : [%,&,D,D,A,P,*,ACK/NACK,@,:,@]
int i;
int erreur = 1; //Dès qu'un problème survient dans l'analyse de la trame, erreur=1 on passe à la trame suivante.
int debutTrame = 0; //Variable servant à faire défiler les octets reçus et prend 37 (="%") lorque l'on tombe sur un début de trame. 

     //VARIABLES A MODIFIER SELON LA TRAME RECUE

int affichageTexte; //Variable booléenne (1=OUI, 0=NON)
int ledV; //Variable booléenne (0 = éteinte, 1 = allumée)
int ledR; //Variable booléenne (0 = éteinte, 1 = allumée)

     //FONCTION DE VERIFICATION (SOMME DES BITS A 1)

int compteur_bits(int k){ //Fonction qui retourne le nombre de bits à 1 dans le nombre k en entrée (décimal ou hexadécimal)
    int n=0;
    for(int i =0; i<16; i++){
        if(k&(1<<i))
            ++n;
    }
    return n;
}
     
int verification(int tab[TAILLE_TRAME]){
  int somme = 0;
  
  //Comptage dans "Affichage Texte"
  
  if (tab[7] == tab[8]){ // Si on a bien 2 octets identiques pour "Affichage Texte"
    if (tab[7] == 250){ // Si on nous dit FA|FA (FA=250)
      somme = somme + 12; // Il y a 12 bits à 1 dans FAFA
    }
    if (tab[7] == 127){ // Si on nous dit 7F|7F (7F=127)
      somme = somme + 14; // Il y a 14 bits à 1 dans 7F7F
    }
  }
  
  //Comptage dans "LEDs"
  
  if (tab[10] == 170){ // Si on nous dit AA (AA=170)
    somme = somme + 4; // Il y a 4 bits à 1 dans AA
  }
  if (tab[10] == 221){ // Si on nous dit DD (DD=221)
    somme = somme + 6; // Il y a 6 bits à 1 dans DD
  }
  
  //Comptage dans "Résultat"
  
  somme = somme + compteur_bits(tab[12]) + compteur_bits(tab[13]); // Ajoute le nombre de bits à 1 du "Résultat". 
  
  return somme; // Total entre 0 et 36 !
}

void loop() {

     //RECHERCHE D'UNE TRAME ENTRANTE
  
  // Si il y a eu une erreur, il faut envoyer un NACK et recommencer une analyse à partir de la prochaine trame.
  if (erreur = 1){
    TrameReponse[7] = NACK; // Il y a erreur donc NACK.
    for (i=0; i<11; i++){
      Serial.write(TrameReponse[i]); // On envoie 1 par 1 les 11 octets de notre tram réponse avec NACK en 8e position.
    }
    if(Serial.available() > 0){ //Si il y a des octets disponibles en attente de lecture
      // Tant que l'on ne tombe pas sur "%"(=37), on fait défiler les données
      while (debutTrame != 37){ 
        debutTrame = Serial.read(); //Prend 1 par 1 les valeurs en attente. 
      }
      // A la sortie du While, debutTrame = "%"(= 37)
      TrameRecue[0] = debutTrame; // Le premier octet de la trame reçue est "%".
      erreur = 0; // Permet d'activer l'analyse du reste de la trame.
    }
    else delay(10); // Si pas de donnée en attente, laisse du temps pour réceptionner des données. 
  }


     //ANALYSE D'UNE TRAME ENTRANTE
     
  while (erreur != 1){
   for(i=1; i<=TAILLE_TRAME-1; i++){ //Remise à 0 du tableau de réception sauf la 1ere case (qui est "%") pour être sûrs de ne pas relire la trame précédente.
     TrameRecue[i]=0;
   }
   if(Serial.available() > 0) {
     for (i=1; i<=TAILLE_TRAME-1; i++){ //On parcours tout le tableau à partir de la 2e case car la 1ere est déjà remplie avec "%" lors de la recherche de trame.
       TrameRecue[i] = Serial.read(); //On remplit le tableau avec les octets reçus (FIFO : First In, First Out)
     }
     //Vérification que la trame commence bien par %&DDAP* et termine par @:@
     if ((TrameRecue[0]==37)&&(TrameRecue[1]==38)&&(TrameRecue[2]==68)&&(TrameRecue[3]==68)&&(TrameRecue[4]==65)&&(TrameRecue[5]==80)&&(TrameRecue[6]==42)&&(TrameRecue[16]==64)&&(TrameRecue[17]==58)&&(TrameRecue[18]==64)){
      if (TrameRecue[15] == verification(TrameRecue)){ // On vérifie la somme des bits à 1
        
        // LEDs
        
        if(TrameRecue[10] == 170){ // Si on reçoit AA on allume la verte et on éteint la rouge
          ledV = 1; ledR = 0;
        }
        if (TrameRecue[10] == 221){ // Si on reçoit DD on éteint la verte et on allume la rouge
          ledV = 0; ledR = 1;
        }

        // Affichage texte

        if (TrameRecue[7] == TrameRecue[8]){ // Test que les 2 octets à la suite sont bien identiques
          if (TrameRecue[7] == 0xFA){ // Si on reçoit FAFA on n'affiche pas
            affichageTexte = 0;
          }
          else if (TrameRecue[7] == 0x7F){ // Si on reçoit 7F7F on affiche
            affichageTexte = 1;
          }
          else erreur = 1; // Donnée non valide (ni FAFA ni 7F7F)
        }

        // Résultat (à faire)                                  <<<<<<<<<<<<<------------------------------------------------------------------
        
       }
      else erreur = 1; // Vérification incorrecte
      }
     else erreur = 1; // Début et/ou fin de trame incorrect
   }
  }
}
