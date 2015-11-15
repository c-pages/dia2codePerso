#ifndef INTERACTION__H
#define INTERACTION__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include "gui.h"



namespace gui {

/////////////////////////////////////////////////
/// \brief Classe communes aux gadgets interactifs (boutons, fenetres, etc... quasi tous en fait ( sauf Image et texte ?) )
///
/////////////////////////////////////////////////
class Interaction {

public:
    /////////////////////////////////////////////////
    /// \brief les evenements de la souris qui déclenchent une fonction lambda associée.
    ///
    /////////////////////////////////////////////////
    enum EvtSouris {
        onBtnG_DblClique,            ///< Double clique gauche
        onBtnG_Relache,            ///< Relache le bouton gauche de la souris.
        onBtnG_RelacheDehors,            ///< Relache a coté du gadget avec le bouton gauche de la souris après avoir pressé dedans.
        onBtnG_Press,            ///< Presse le bouton gauche de la souris.
        onBtnG_PressDehors,            ///< Presse a coté du gadget avec le bouton gauche de la souris.
        onBtnD_Relache,            ///< Relache le bouton droit de la souris.
        onBtnD_RelacheDehors,            ///< Relache le bouton droit de la souris.
        onBtnD_Press,            ///< Presse le bouton droit de la souris.
        onBtnM_Relache,            ///< Relache le bouton du milieu de la souris.
        onBtnM_RelacheDehors,            ///< Relache le bouton du milieu de la souris.
        onBtnM_Press,            ///< Presse le bouton du milieu de la souris.
        onBtnM_RollUp,            ///< Roule vers le haut le bouton du milieu de la souris.
        onBtnM_RollDown,            ///< Roule vers le bas le bouton du milieu de la souris.
        onBtn_Entre,            ///< Survol
        onBtn_Sort,            ///< Quitte le survol.
        onBtn_changeEtat,            ///< quand BoutonACocher change d'état
        onFen_Ferme,            ///< Quand on ferme une fenetre.
        onFen_Ouvre,            ///< Quand on ferme une fenetre.
        onCha_ChangeValeur,            ///< Quand on change la valeur d'un champ de saisie de texte.
        onCha_ValideValeur            ///< Quand on valide la valeur d'un champ de saisie de texte.
    };

    typedef std::map <EvtSouris, FuncType> EvtsSouris;    ///< les evenemnts de la souris
protected:
    typedef std::map <sf::Keyboard::Key, FuncType> EvtsClavier;    ///< Les differents événement du clavier liés au gadget

/////////////////////////////////////////////////
// Les methodes
/////////////////////////////////////////////////

public:
    /////////////////////////////////////////////////
    /// \brief Constructeur par defaut
    ///
    /////////////////////////////////////////////////
    Action ();

    void lier  ( EvtSouris  evenement, funcType fonction);

    void lier ( sf::Keyboard  touche,  funcType  fonction);

    void delier (EvtSouris  evenement);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param touche		 
    /////////////////////////////////////////////////
    void delier ( sf::Keyboard  touche);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param evenement		 
    /////////////////////////////////////////////////
    void declencher ( EvtSouris  evenement);

    virtual void traiter_evenements (const sf::Event&  evenement);

private:
    gui::AutreClassemembre* m_autreMembre;     ///< le membre popo
    gui::Classemembre* membre;
    map < EvtsSouris , Functype > m_evenementsSouris;    
    map < sf::Keyboard  , Functype > m_evenementsClavier;    ///< mepopo oui ufyuf     
};

} // fin namespace gui

#endif


////////////////////////////////////////////////////////////
/// class Interaction
/// ingroup 
///
/// see 
///
////////////////////////////////////////////////////////////
