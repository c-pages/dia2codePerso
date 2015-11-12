#ifndef GADGET__H
#define GADGET__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include "Groupe.h"
#include "Transformable.h"
#include "Drawable.h"


/////////////////////////////////////////////////
/// \brief Classe communes à tout les gadgets, gère affichage actualisation, etc...
///
/////////////////////////////////////////////////
class Gadget : public Transformable, public Drawable {
    
    /////////////////////////////////////////////////
    // Les methodes
    /////////////////////////////////////////////////
    
public:
    /////////////////////////////////////////////////
    /// \brief Acceder à l'id du gadget.
    ///
    /////////////////////////////////////////////////
unsigned int getID () const;

    /////////////////////////////////////////////////
    /// \brief Definir l'id du gadget.
    ///
    /// \param id		 la nouvel id
    /////////////////////////////////////////////////
void setID (unsigned int id);

void setActif (bool valeur);

bool getActif () const;

void setVisible (bool valeur);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
bool getVisible () const;

setSkin (shared_ptr<Skin> skin);

Skin getSkin () const;

void setInfo (string info);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
string getInfo () const;

void setParent (ptr parent);

virtual void ajouter (ptr enfant);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param enfant		 
    /////////////////////////////////////////////////
virtual void supprimer (ptr enfant);

sf::Vector2f getPosAbs () const;

void setSize (sf::Vector2i taille);

sf::Vector2i getSize () const;

sf::floatRect getLocalBounds () const;

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
sf::floatRect getGlobalBounds () const;

void Aligner (ptr<Gadget> cible);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param x		 
    /// \param y		 
    /////////////////////////////////////////////////
bool Contient (float x, float y) const;

protected:
void demander_aEtre_supprimer ();

void demander_aEtre_auDessus () const;

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param id		 l'id du gadget enfant à mettre au dessus des autres.
    /////////////////////////////////////////////////
void mettre_auDessus (unsigned int id);

virtual void init ();

virtual void init_skin ();

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
virtual void init_interactions ();

virtual void actualiser ();

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
virtual void actualiser_bounds ();

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param evenement		 
    /////////////////////////////////////////////////
virtual void traiter_evenements (const sf::Event& evenement);

virtual void draw (sf::RenderTarget& target, sf::RenderStates states) const;

    
    /////////////////////////////////////////////////
    // Les membres
    /////////////////////////////////////////////////
    
private:
    unsigned int m_id;    ///<  l'id unique du gadget dans la liste des enfants de son parent. L'id correspond à l'ordre de cette liste => ordre de dessins + ordre pour "contient()".    
    sf::Vector2i m_taille;    ///<  la taille permet d'organiser la disposition des éléments graphiques du gadget.    
    sf::floatRect m_localBounds;    ///<  la rectangle anglobant du gadget en coordonnés locales.    
    sf::floatRect m_globalBounds;    ///<  la rectangle anglobant du gadget en coordonnés globales.    
    Skin m_skin;    ///<  le skin.    
    std::string m_infoBulle;    ///<  texte d'information sur le gadget  à afficher en cas de survol prolongé d'un gadget.    
    bool m_actif;    ///<  le gadget est il actif ?    
    bool m_visible;    ///<  est il visible ? ( si non visible : inactif ?)    
    bool m_aSupprimer;    ///<  si le parent doit supprimer ce gadget.    
    Gadget* m_parent;    ///<  le Gadget parent.    
    Groupe m_enfants;    ///<  les Gadgets enfants.    
    GUI* ms_gui;    ///<  static vers le gui commum    
protected:
    ptr m_this;    ///<  un pointeur partagé vers le gadget (this)    
};

#endif


////////////////////////////////////////////////////////////
/// class Gadget
/// ingroup 
///
/// see 
///
////////////////////////////////////////////////////////////
