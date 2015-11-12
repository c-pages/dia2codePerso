#ifndef BOUTON__H
#define BOUTON__H

/////////////////////////////////////////////////
// Headers
/////////////////////////////////////////////////
#include "Action.h"
#include "Gadget.h"


/////////////////////////////////////////////////
/// \brief 
///
/////////////////////////////////////////////////
class Bouton : public Action, public Gadget {
    
    /////////////////////////////////////////////////
    // Les methodes
    /////////////////////////////////////////////////
    
public:
    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param parent		 
    /////////////////////////////////////////////////
Bouton (ptr parent);

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /// \param etat		 le nouvel etat
    /////////////////////////////////////////////////
void setEtat (Etat etat);

virtual void actualiser ();

virtual void traiter_evenements (const sf::Event& evenement);

private:
virtual void init ();

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
virtual void init_skin ();

    /////////////////////////////////////////////////
    /// \brief  
    ///
    /////////////////////////////////////////////////
virtual virtual void actualiser_bounds ();

    
    /////////////////////////////////////////////////
    // Les membres
    /////////////////////////////////////////////////
    
private:
    EtatBouton m_etat;    ///<   ( inactif , repos, survol, presse  )    
    sf::Clock m_clock_dblClique;    
    bool m_1erClick;    ///<       
    shared_ptr<Image> m_fond;    ///<       
};

#endif


////////////////////////////////////////////////////////////
/// class Bouton
/// ingroup 
///
/// see 
///
////////////////////////////////////////////////////////////