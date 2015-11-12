#ifndef FENETRE__H
#define FENETRE__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include "gui.h"
#include "Action.h"
#include "Gadget.h"


/////////////////////////////////////////////////
/// \brief 
///
/////////////////////////////////////////////////
class Fenetre : public Action, public Gadget {
    
    /////////////////////////////////////////////////
    // Les methodes
    /////////////////////////////////////////////////
    
public:
    Fenetre (ptr parent);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param parent		 
    /// \param titre		 
    /// \param redim		 
    /// \param fermable		 
    /// \param deplacable		 
    /////////////////////////////////////////////////
    Fenetre (ptr parent, string titre, bool redim = true, bool fermable = true, bool deplacable = true);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param titre		 
    /////////////////////////////////////////////////
    setTitre (string titre);

    virtual void ajouter (ptr enfant) = 0;

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param enfant		 
    /////////////////////////////////////////////////
    virtual void supprimer (ptr enfant) = 0;

    virtual void init () = 0;

    virtual void actualiser () = 0;

    virtual void traiter_evenements (const sf::Event& evenement) = 0;

private:
    virtual void init_skin () = 0;

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
    virtual void init_interactions () = 0;

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
    virtual void actualiser_bounds () = 0;

    
    /////////////////////////////////////////////////
    // Les membres
    /////////////////////////////////////////////////
    
public:
    static titi popo;    ///<  pas mal    
    static GUI ms_gui;    ///<  le gui commun.    
private:
    bool m_deplacable;    
    bool m_redimensionnable;    
    bool m_fermable;    ///<       
    bool m_deplacement;    
    bool m_redim_D;    
    bool m_redim_G;    
    bool m_redim_H;    
    bool m_redim_B;    ///<       
    sf::Vector2i m_fenetrePosition_debut;    
    sf::Vector2i m_sourisPosition_debut;    
    sf::Vector2i m_fenetreTaille_debut;    ///<       
    shared_ptr<Label> m_lblTitre;    
    shared_ptr<Bouton> m_btnDrag;    
    shared_ptr<Bouton> m_btnFermer;    
    shared_ptr<Bouton> m_btnRedim_BD;    
    shared_ptr<Bouton> m_btnRedim_BG;    
    shared_ptr<Bouton> m_btnRedim_HD;    
    shared_ptr<Bouton> m_btnRedim_HG;    
    shared_ptr<Bouton> m_btnRedim_G;    
    shared_ptr<Bouton> m_btnRedim_D;    
    shared_ptr<Bouton> m_btnRedim_B;    
    shared_ptr<Bouton> m_btnRedim_H;    
    shared_ptr<Image> m_contenuCadre;    
    shared_ptr<Contenant> m_contenu;    
    shared_ptr<Image> m_fond;    ///<       
};

#endif


////////////////////////////////////////////////////////////
/// class Fenetre
/// ingroup 
///
/// see 
///
////////////////////////////////////////////////////////////
